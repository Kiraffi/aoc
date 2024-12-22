#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */

#define _USE_MATH_DEFINES // M_PI for msvc
#include <cmath>
#include <stdlib.h>
#include <vector>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>

#include "commonrender.h"
#include "gputexture.h"

#include <render/SDL_render_debug_font.h>


static const int s_WindowWidth = 800;
static const int s_WindowHeight = 600;

static const int s_FontTextureWidth = 1024;
static const int s_FontTextureHeight = 1024;

struct WindowState
{
    uint32_t m_drawablew;
    uint32_t m_drawableh;
};

struct AppState
{
    SDL_GPUDevice* m_device;
    SDL_Window* m_window;
    SDL_Time m_startTime;
    SDL_Time m_previousTime;
    int64_t m_frameIndex;
    bool m_running;
};

enum TextureTypes : int
{
    TextureTypeFontTexture,

    TextureTypeCount,
};

enum RenderTextureTypes : int
{
    RenderTextureColor,
    RenderTextureDepth,

    RenderTextureCount
};

static GpuTexture s_renderTargets[RenderTextureCount] = {};
static GpuTexture s_textures[TextureTypeCount] = {};

static WindowState s_windowState = {};
static AppState s_appState = {};

static bool s_finished = false;

static SDL_GPUTransferBuffer* s_transferDownloadBuffer = nullptr;

static std::vector<SDL_GPUBuffer*> s_createdGpuBuffers;

static SDL_Window* getWindow()
{
    return s_appState.m_window;
}

static void clearRenderTargets()
{
    for (GpuTexture& renderTarget : s_renderTargets)
    {
        if (renderTarget.m_texture)
        {
            SDL_ReleaseGPUTexture(s_appState.m_device, renderTarget.m_texture);
        }
        renderTarget = GpuTexture{};
    }
}

static void destroyTextures()
{
    for (GpuTexture& texture : s_textures)
    {
        if(texture.m_texture)
        {
            SDL_ReleaseGPUTexture(s_appState.m_device, texture.m_texture);
        }
        texture = GpuTexture{};
    }
}

static void shutdownGPU(void)
{
    {
        for(int i = 0; i < 4; ++i)
        {
            SDL_Delay(100);
            SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(s_appState.m_device);
            gpuReadEndBuffers();
            SDL_SubmitGPUCommandBuffer(cmd);
        }
    }
    deinitData();

    clearRenderTargets();
    destroyTextures();
    SDL_GPUDevice* gpuDevice = s_appState.m_device;
    SDL_ReleaseWindowFromGPUDevice(gpuDevice, getWindow());

    SDL_ReleaseGPUTransferBuffer(s_appState.m_device, s_transferDownloadBuffer);

    SDL_DestroyGPUDevice(gpuDevice);
    s_appState.m_device = nullptr;
}

/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
static void quit(int rc)
{
    shutdownGPU();
    SDL_DestroyWindow(getWindow());
    SDL_Quit();
    exit(rc);
}


static SDL_GPUTexture* createDepthTexture(
    uint32_t drawablew, uint32_t drawableh, SDL_GPUTextureFormat format)
{
    SDL_GPUTextureCreateInfo createinfo;

    createinfo.type = SDL_GPU_TEXTURETYPE_2D;
    createinfo.format = format;
    createinfo.width = drawablew;
    createinfo.height = drawableh;
    createinfo.layer_count_or_depth = 1;
    createinfo.num_levels = 1;
    createinfo.sample_count = SDL_GPU_SAMPLECOUNT_1; //s_renderState.m_sampleCount;
    createinfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    createinfo.props = 0;

    SDL_GPUTexture* result = SDL_CreateGPUTexture(s_appState.m_device, &createinfo);
    return result;
}


static SDL_GPUTexture* createColorTexture(
    uint32_t drawablew, uint32_t drawableh, SDL_GPUTextureFormat format)
{
    SDL_GPUTextureCreateInfo createinfo;

    createinfo.type = SDL_GPU_TEXTURETYPE_2D;
    createinfo.format = format;
    createinfo.width = drawablew;
    createinfo.height = drawableh;
    createinfo.layer_count_or_depth = 1;
    createinfo.num_levels = 1;
    createinfo.sample_count = SDL_GPU_SAMPLECOUNT_1; //s_renderState.m_sampleCount;
    createinfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    createinfo.props = 0;

    SDL_GPUTexture* result = SDL_CreateGPUTexture(s_appState.m_device, &createinfo);
    return result;
}

static GpuTexture createTexture(uint32_t w, uint32_t h, SDL_GPUTextureFormat format)
{
    GpuTexture result = {};
    SDL_GPUTexture* texture = nullptr;

    if (format == SDL_GPU_TEXTUREFORMAT_D32_FLOAT)
    {
        texture = createDepthTexture(w, h, format);
    }
    else
    {
        texture = createColorTexture(w, h, format);
    }
    if (texture == nullptr)
    {
        return result;
    }

    result.m_width = w;
    result.m_height = h;
    result.m_sdlGpuTextureFormatAsInt = int(format);
    result.m_texture = texture;

    return result;
}

static bool createRenderTargets(uint32_t w, uint32_t h)
{
    clearRenderTargets();

    s_renderTargets[RenderTextureColor] = createTexture(w, h, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB);
    s_renderTargets[RenderTextureDepth] = createTexture(w, h, SDL_GPU_TEXTUREFORMAT_D32_FLOAT);



    for (const GpuTexture& renderTarget : s_renderTargets)
    {
        if(!renderTarget.isValid())
        {
            return false;
        }
    }

    return true;
}

static std::vector<uint8_t> createFontTexture()
{
    std::vector<uint8_t> pixelData(s_FontTextureWidth * s_FontTextureHeight * 4, 0);

    // 32 chars per row, using 16 pixels with 8 pixel padding on all directions = 32 pixels per char

    auto fn = [&pixelData](int charIndex, int endCharIndex, int fontOffsetData)
    {
        for(;charIndex < endCharIndex; ++charIndex)
        {
            int x = (charIndex % 32) * 32 + 8;
            int y = (charIndex / 32) * 32 + 8;
            for(int row = 0; row < 8; ++row)
            {
                uint8_t rowPixels = SDL_RenderDebugTextFontData[(charIndex - fontOffsetData) * 8 + row];
                for(int col = 0; col < 8; ++col)
                {
                    if((rowPixels >> col) & 1)
                    {
                        // 2x2
                        for(int py = 0; py < 2; ++py)
                        {
                            uint8_t* pData = &pixelData[(((row * 2) + y + py) * s_FontTextureWidth + x + (col * 2)) * 4];

                            // 2x2, rgba = 4 channels
                            for(int px = 0; px < 2 * 4; ++px)
                            {
                                pData[px] = 255;
                            }
                        }
                    }
                }
            }
        }
    };
    fn(33, 127, 33);
    fn(161, 257, (161 - 127) + 33);

    return pixelData;
}

static bool createTextures()
{
    if(!s_textures[TextureTypeFontTexture].isValid())
    {
        s_textures[TextureTypeFontTexture] = createTexture(s_FontTextureWidth, s_FontTextureHeight, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB);
        auto fontData = createFontTexture();
        uploadGPUTextureOneTimeInInit(s_textures + TextureTypeFontTexture, fontData.data(), fontData.size() * sizeof(fontData[0]));
    }

    return true;
}

static void createBuffers()
{
    SDL_GPUTransferBufferCreateInfo transferBufferDesc = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD,
        .size = 1024 * 1024,
        .props = 0
    };
    SDL_GPUTransferBuffer* bufTransfer = SDL_CreateGPUTransferBuffer(
        s_appState.m_device,
        &transferBufferDesc
    );
    s_transferDownloadBuffer = bufTransfer;
}

static void render(SDL_Window* window)
{
    SDL_GPUTexture* swapchainTexture;
    uint32_t drawablew, drawableh;

    /* Acquire the swapchain texture */

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(s_appState.m_device);
    if (!cmd)
    {
        SDL_Log("Failed to acquire command buffer :%s", SDL_GetError());
        quit(2);
    }
    if (!SDL_AcquireGPUSwapchainTexture(
            cmd,
            getWindow(),
            &swapchainTexture,
            &drawablew,
            &drawableh))
    {
        SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
        quit(2);
    }

    s_appState.m_frameIndex++;

    if (swapchainTexture == nullptr)
    {
        /* No swapchain was acquired, probably too many frames in flight */
        SDL_SubmitGPUCommandBuffer(cmd);
        return;
    }

    /* Resize the depth buffer if the window size changed */

    if (s_windowState.m_drawablew != drawablew || s_windowState.m_drawableh != drawableh)
    {
        createRenderTargets(drawablew, drawableh);
    }
    s_windowState.m_drawablew = drawablew;
    s_windowState.m_drawableh = drawableh;

    /* Set up the pass */

    s_appState.m_running = renderFrame(cmd, s_appState.m_frameIndex);


    /* Blit to resolve */
    {
        SDL_GPUBlitInfo blitInfo;
        SDL_zero(blitInfo);
        //blitInfo.source.texture = s_renderTargets[RenderTextureColor].m_texture;
        //blitInfo.source.w = s_renderTargets[RenderTextureColor].m_width;
        //blitInfo.source.h = s_renderTargets[RenderTextureColor].m_height;

        blitInfo.source.texture = s_textures[TextureTypeFontTexture].m_texture;
        blitInfo.source.w = s_textures[TextureTypeFontTexture].m_width;
        blitInfo.source.h = s_textures[TextureTypeFontTexture].m_height;

        blitInfo.destination.texture = swapchainTexture;
        blitInfo.destination.w = drawablew;
        blitInfo.destination.h = drawableh;

        blitInfo.load_op = SDL_GPU_LOADOP_DONT_CARE;
        blitInfo.filter = SDL_GPU_FILTER_LINEAR;

        SDL_BlitGPUTexture(cmd, &blitInfo);
    }

    /* Submit the command buffer! */
    SDL_SubmitGPUCommandBuffer(cmd);
}


static bool initRenderState()
{
    SDL_GPUDevice* gpuDevice = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV,
        true,
        nullptr
    );
    if(gpuDevice == nullptr)
        return false;


    /* Claim the windows */
    SDL_ClaimWindowForGPUDevice(
        gpuDevice,
        getWindow()
    );
    s_appState.m_device = gpuDevice;

#if VSYNC_OFF
    // VSync off
    SDL_SetGPUSwapchainParameters(
        gpuDevice,
        getWindow(),
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        SDL_GPU_PRESENTMODE_IMMEDIATE);
#endif

    /* create a depth texture for the window */
    Uint32 drawablew, drawableh;
    SDL_GetWindowSizeInPixels(getWindow(), (int*) &drawablew, (int*) &drawableh);
    createRenderTargets(drawablew, drawableh);
    createTextures();

    createBuffers();

    return true;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    SDL_Time time = SDL_GetTicksNS();
    AppState* as = (AppState*)appstate;
    SDL_Time timeDiff = time - as->m_previousTime;


    render(getWindow());

    if(!as->m_running)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (appstate != nullptr)
    {
        AppState* as = (AppState*)appstate;
        shutdownGPU();
        SDL_DestroyWindow(as->m_window);
    }
}

static SDL_AppResult handleKeyEvent(SDL_Scancode key_code)
{
    switch (key_code)
    {
        /* Quit. */
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_Q:
            return SDL_APP_SUCCESS;
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            return handleKeyEvent(event->key.scancode);
    }
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppInit(void** appState, int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return SDL_APP_FAILURE;
    }

    *appState = &s_appState;

    s_appState.m_window = SDL_CreateWindow(
        getTitle(),
        s_WindowWidth,
        s_WindowHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );
    s_appState.m_frameIndex = -1;

    if(s_appState.m_window == nullptr)
    {
        return SDL_APP_FAILURE;
    }

    if(!initRenderState())
    {
        return SDL_APP_FAILURE;
    }
    if(!initData())
    {
        return SDL_APP_FAILURE;
    }

    SDL_Time time = SDL_GetTicksNS();
    s_appState.m_startTime = s_appState.m_previousTime = time;
    s_appState.m_running = true;

    return SDL_APP_CONTINUE;
}

SDL_GPUBuffer* createGPUWriteBuffer(uint32_t size, const char* debugName, bool isIndirect)
{
    SDL_GPUBufferUsageFlags usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE;
    if(isIndirect)
    {
        usage |= SDL_GPU_BUFFERUSAGE_INDIRECT;
    }
    SDL_GPUBufferCreateInfo bufferDesc = {
        .usage = usage,
        .size = size,
        .props = 0
    };
    SDL_GPUBuffer* result = SDL_CreateGPUBuffer(
        s_appState.m_device,
        &bufferDesc
    );
    SDL_SetGPUBufferName(s_appState.m_device, result, debugName);
    s_createdGpuBuffers.push_back(result);
    return result;
}

bool uploadGPUBufferOneTimeInInit(SDL_GPUBuffer* dstGpuBuffer, uint8_t* data, uint32_t size)
{
    SDL_GPUTransferBufferCreateInfo transferBufferDesc = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = size,
        .props = 0
    };
    SDL_GPUTransferBuffer* bufTransfer = SDL_CreateGPUTransferBuffer(
        s_appState.m_device,
        &transferBufferDesc
    );

    {
        void* map = SDL_MapGPUTransferBuffer(s_appState.m_device, bufTransfer, false);
        SDL_memcpy(map, data, size);
        SDL_UnmapGPUTransferBuffer(s_appState.m_device, bufTransfer);
    }
    {
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(s_appState.m_device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
        SDL_GPUTransferBufferLocation bufLocation = {
            .transfer_buffer = bufTransfer,
            .offset = 0
        };

        SDL_GPUBufferRegion dstRegion = {
            .buffer = dstGpuBuffer,
            .offset = 0,
            .size = size
        };
        SDL_UploadToGPUBuffer(copyPass, &bufLocation, &dstRegion, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(s_appState.m_device, bufTransfer);
    }
    return true;

}

bool uploadGPUTextureOneTimeInInit(GpuTexture* dstGpuTexture, uint8_t* data, uint32_t size)
{
        SDL_GPUTransferBufferCreateInfo transferBufferDesc = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = size,
        .props = 0
    };
    SDL_GPUTransferBuffer* bufTransfer = SDL_CreateGPUTransferBuffer(
        s_appState.m_device,
        &transferBufferDesc
    );

    {
        void* map = SDL_MapGPUTransferBuffer(s_appState.m_device, bufTransfer, false);
        SDL_memcpy(map, data, size);
        SDL_UnmapGPUTransferBuffer(s_appState.m_device, bufTransfer);
    }
    {
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(s_appState.m_device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
        SDL_GPUTransferBufferLocation bufLocation = {
            .transfer_buffer = bufTransfer,
            .offset = 0
        };

        SDL_GPUTextureTransferInfo srcInfo = {};
        SDL_zero(srcInfo);
        srcInfo.transfer_buffer = bufTransfer;
        srcInfo.rows_per_layer = dstGpuTexture->m_height;
        srcInfo.pixels_per_row = dstGpuTexture->m_width;

        SDL_GPUTextureRegion dstRegion = {};
        SDL_zero(dstRegion);
        dstRegion.texture = dstGpuTexture->m_texture;
        dstRegion.x = 0;
        dstRegion.y = 0;
        dstRegion.w = dstGpuTexture->m_width;
        dstRegion.h = dstGpuTexture->m_height;
        dstRegion.d = 1;

        SDL_UploadToGPUTexture(copyPass, &srcInfo, &dstRegion, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(s_appState.m_device, bufTransfer);
    }
    return true;

}

bool downloadGPUBuffer(uint8_t* dstData, SDL_GPUBuffer* srcGpuBuffer, uint32_t size)
{
    {
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(s_appState.m_device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
        SDL_GPUTransferBufferLocation bufLocation = {
            .transfer_buffer = s_transferDownloadBuffer,
            .offset = 0
        };

        SDL_GPUBufferRegion srcRegion = {
            .buffer = srcGpuBuffer,
            .offset = 0,
            .size = size
        };
        SDL_DownloadFromGPUBuffer(copyPass, &srcRegion, &bufLocation);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
    }

    {
        void* map = SDL_MapGPUTransferBuffer(s_appState.m_device, s_transferDownloadBuffer, false);
        SDL_memcpy(dstData, map, size);
        SDL_UnmapGPUTransferBuffer(s_appState.m_device, s_transferDownloadBuffer);
    }

    return true;

}

SDL_GPUComputePipeline* createComputePipeline(const ComputePipelineInfo& info)
{
    SDL_GPUComputePipelineCreateInfo newCreateInfo =
    {
        .code_size = info.m_codeSize,
        .code = (const uint8_t*)info.m_code,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .num_readwrite_storage_buffers = info.m_bufferAmount,
        .num_uniform_buffers = info.m_uniformBufferAmount,
        .threadcount_x = info.m_worgroupSizeX,
        .threadcount_y = std::max(info.m_worgroupSizeY, 1u),
        .threadcount_z = 1,
    };

    SDL_GPUComputePipeline* pipeline = SDL_CreateGPUComputePipeline(getGpuDevice(), &newCreateInfo);
    if (pipeline == nullptr)
    {
        SDL_Log("Failed to create compute pipeline!");
    }
    return pipeline;
}


SDL_GPUDevice* getGpuDevice()
{
    return s_appState.m_device;
}