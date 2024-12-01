#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>


#include "d01a_comp.h"
#include "d01b_comp.h"
#include "radixsort_comp.h"

#include "commonrender.h"





static const std::string s_Filename = "input/01.input";


static SDL_GPUComputePipeline* s_radixPipeline = nullptr;
static SDL_GPUComputePipeline* s_d01aPipeline = nullptr;
static SDL_GPUComputePipeline* s_d01bPipeline = nullptr;

static std::vector<SDL_GPUBuffer*> s_buffers;

int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 01";
}

static std::vector<int> parseInts()
{
    std::vector<int> result;
    std::ifstream file(s_Filename, std::ios::binary);
    if(!file.is_open())
    {
        printf("Failed to open file\n");
        return result;
    }
    int value;
    while(file >> value)
    {
        result.push_back(value);
    }
    return result;
}



static void d01a()
{
    std::vector<int> values = parseInts();
    std::vector<int> left;
    std::vector<int> right;
    for(size_t i = 0; i < values.size(); i += 2)
    {
        left.push_back(values[i + 0]);
        right.push_back(values[i + 1]);
    }
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    int diff = 0;
    for(size_t i = 0; i < left.size(); ++i)
    {
        diff += std::abs(left[i] - right[i]);
    }
    printf("01-a difference: %i\n", diff);
}

static void d01b()
{
    std::vector<int> values = parseInts();
    std::vector<int> left;
    std::vector<int> right;

    for(size_t i = 0; i < values.size(); i += 2)
    {
        left.push_back(values[i + 0]);
        right.push_back(values[i + 1]);
    }
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    int diff = 0;
    for(size_t i = 0; i < left.size(); ++i)
    {
        int count = 0;
        for(size_t j = 0; j < right.size(); ++j)
        {
            if(left[i] == right[j])
                count++;
        }
        diff += count * left[i];
    }
    printf("01-b simularity: %i\n", diff);
}

void d01()
{
    d01a();
    d01b();
}












bool initCompute()
{
    std::vector<int> values = parseInts();
    std::vector<int> left;
    std::vector<int> right;
    for(size_t i = 0; i < values.size(); i += 2)
    {
        left.push_back(values[i + 0]);
        right.push_back(values[i + 1]);
    }

    s_buffers.push_back(createGPUWriteBuffer(left.size() * sizeof(int), "Left"));
    s_buffers.push_back(createGPUWriteBuffer(left.size() * sizeof(int), "LeftSorted"));
    s_buffers.push_back(createGPUWriteBuffer(right.size() * sizeof(int), "Right"));
    s_buffers.push_back(createGPUWriteBuffer(right.size() * sizeof(int), "RightSorted"));

    s_buffers.push_back(createGPUWriteBuffer(1024, "ResultBuffer"));

    uploadGPUBufferOneTimeInInit(s_buffers[0], (uint8_t*)left.data(), left.size() * sizeof(int));
    uploadGPUBufferOneTimeInInit(s_buffers[2], (uint8_t*)right.data(), right.size() * sizeof(int));


    {
        /* compute works differently?
        auto loadShader = []() -> SDL_GPUShader* {
            SDL_GPUDevice* gpuDevice = getGpuDevice();
            SDL_GPUShaderFormat format = SDL_GetGPUShaderFormats(gpuDevice);

            SDL_GPUShaderCreateInfo createinfo = {
                .code_size = sizeof(radixsort_comp),
                .code = (const uint8_t*)radixsort_comp,
                .entrypoint = "main",

                .format = SDL_GPU_SHADERFORMAT_SPIRV,

                .stage = SDL_GPU_SHADERSTAGE_COMPUTE,

                .num_samplers = 0,
                .num_storage_textures = 0,
                .num_storage_buffers = 2u,
                .num_uniform_buffers = 0u,

                .props = 0,
            };
            return SDL_CreateGPUShader(gpuDevice, &createinfo);
        };

        SDL_GPUShader* radixShader = loadShader();
        */
        {
            SDL_GPUComputePipelineCreateInfo newCreateInfo =
            {
                .code_size = sizeof(radixsort_comp),
                .code = (const uint8_t*)radixsort_comp,
                .entrypoint = "main",
                .format = SDL_GPU_SHADERFORMAT_SPIRV,
                .num_readwrite_storage_buffers = 2,
                .threadcount_x = 256,
                .threadcount_y = 1,
                .threadcount_z = 1,
            };

            s_radixPipeline = SDL_CreateGPUComputePipeline(getGpuDevice(), &newCreateInfo);
            if (s_radixPipeline == NULL)
            {
                SDL_Log("Failed to create compute pipeline!");
                return false;
            }
        }
        {
            SDL_GPUComputePipelineCreateInfo newCreateInfo =
            {
                .code_size = sizeof(d01a_comp),
                .code = (const uint8_t*)d01a_comp,
                .entrypoint = "main",
                .format = SDL_GPU_SHADERFORMAT_SPIRV,
                .num_readwrite_storage_buffers = 3,
                .threadcount_x = 1024,
                .threadcount_y = 1,
                .threadcount_z = 1,
            };

            s_d01aPipeline = SDL_CreateGPUComputePipeline(getGpuDevice(), &newCreateInfo);
            if (s_d01aPipeline == NULL)
            {
                SDL_Log("Failed to create compute pipeline!");
                return false;
            }
        }

        {
            SDL_GPUComputePipelineCreateInfo newCreateInfo =
            {
                .code_size = sizeof(d01b_comp),
                .code = (const uint8_t*)d01b_comp,
                .entrypoint = "main",
                .format = SDL_GPU_SHADERFORMAT_SPIRV,
                .num_readwrite_storage_buffers = 3,
                .threadcount_x = 1024,
                .threadcount_y = 1,
                .threadcount_z = 1,
            };

            s_d01bPipeline = SDL_CreateGPUComputePipeline(getGpuDevice(), &newCreateInfo);
            if (s_d01bPipeline == NULL)
            {
                SDL_Log("Failed to create compute pipeline!");
                return false;
            }
        }

    }


    return true;
}

bool initData()
{
    d01();
    return initCompute();
}

void deinitData()
{
    SDL_GPUDevice* gpuDevice = getGpuDevice();
    SDL_ReleaseGPUComputePipeline(gpuDevice, s_radixPipeline);
    SDL_ReleaseGPUComputePipeline(gpuDevice, s_d01aPipeline);
    SDL_ReleaseGPUComputePipeline(gpuDevice, s_d01bPipeline);

    s_radixPipeline = s_d01aPipeline = s_d01bPipeline = nullptr;

    for(SDL_GPUBuffer* buffer : s_buffers)
    {
        SDL_ReleaseGPUBuffer(gpuDevice, buffer);
    }
    s_buffers.clear();


    printf("01-a compute difference: %i\n", s_dataBuffer[0]);
    printf("01-b compute simularity: %i\n", s_dataBuffer[1]);
}

bool renderFrame(SDL_GPUCommandBuffer* cmd, int index)
{
    // Do radix sorting
    for(int i = 0; i < 2; ++i)
    {
        SDL_GPUStorageBufferReadWriteBinding buffers[] = {
            { .buffer = s_buffers[0 + i * 2] },
            { .buffer = s_buffers[1 + i * 2] }
        };
        SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
            cmd,
            nullptr,
            0,
            buffers,
            sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
        );

        SDL_BindGPUComputePipeline(computePass, s_radixPipeline);
        SDL_DispatchGPUCompute(computePass, 1, 1, 1);
        SDL_EndGPUComputePass(computePass);
    }

    // 01a, o1b
    for(int i = 0; i < 2; ++i)
    {
        SDL_GPUStorageBufferReadWriteBinding buffers[] = {
            { .buffer = s_buffers[1] },
            { .buffer = s_buffers[3] },
            { .buffer = s_buffers[4] }
        };
        SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
            cmd,
            nullptr,
            0,
            buffers,
            sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
        );

        SDL_BindGPUComputePipeline(computePass, i == 0 ? s_d01aPipeline : s_d01bPipeline);
        SDL_DispatchGPUCompute(computePass, 1, 1, 1);
        SDL_EndGPUComputePass(computePass);
    }

    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[4], 1024);

    //if(index > 100)
    //    return false;
    return true;
}
