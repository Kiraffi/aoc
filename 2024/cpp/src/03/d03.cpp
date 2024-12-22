#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>

#include "../atomic_buffers_reset_comp.h"
#include "d03_calculate_sums_comp.h"
#include "parse_sums_dos_donts_comp.h"

#include "commons.h"
#include "commonrender.h"


enum BufferEnum : int
{
    BufferInput,

    BufferSums,
    BufferDos,
    BufferDonts,

    BufferAtomicResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineAtomicBufferReset,
    PipelineParse,
    PipelineCalculateSums,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(atomic_buffers_reset_comp), 1, 0, 1 },
    { BUF_N_SIZE(parse_sums_dos_donts_comp), 5, 1, 256 },
    { BUF_N_SIZE(d03_calculate_sums_comp), 4, 0, 256 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);

static const std::string s_Filename = "input/03.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);


static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 03";
}

static int getMul(int pos)
{
    int num1 = 0;
    int num2 = 0;
    bool valid = false;
    while(isdigit(s_input[pos]))
    {
        num1 = num1 * 10 + (s_input[pos] - '0');
        valid = true;
        pos++;
    }
    if(!valid)
    {
        return 0;
    }
    if(s_input[pos] != ',')
    {
        return 0;
    }
    ++pos;
    valid = false;
    while(isdigit(s_input[pos]))
    {
        num2 = num2 * 10 + (s_input[pos] - '0');
        valid = true;
        pos++;
    }
    if(!valid)
    {
        return 0;
    }
    if(s_input[pos] != ')')
    {
        return 0;
    }
    return num1 * num2;
}

static void a()
{
    int64_t sums = 0;
    int startIndex = 0;
    while(startIndex < s_input.length())
    {
        int pos = s_input.find("mul(", startIndex);
        if(pos < s_input.length() && pos >= 0)
        {
            startIndex = pos + 1;
            sums += getMul(pos + 4);
        }
        else
        {
            break;
        }
    }

    printf("03-a sum of muls: %i\n", int(sums));
}

static void b()
{
    int64_t sums = 0;

    bool enabled = true;

    int startIndex = 0;
    int nextDont = s_input.find("don't()", startIndex);
    while(startIndex < s_input.length())
    {
        if(!enabled)
        {
            int nextDo = s_input.find("do()", startIndex);
            if(nextDo >= 0 && nextDo < s_input.length())
            {
                startIndex = nextDo + 1;
                enabled = true;
                continue;
            }
            break;
        }
        if(nextDont < startIndex)
        {
            nextDont = s_input.find("don't()", startIndex);
        }
        int pos = s_input.find("mul(", startIndex);

        if(enabled && nextDont >= 0 && nextDont < pos)
        {
            enabled = false;
            startIndex = nextDont + 1;
            continue;
        }

        if(pos < s_input.length() && pos >= 0)
        {
            startIndex = pos + 1;
            sums += getMul(pos + 4);
        }
        else
        {
            break;
        }
    }

    printf("03-b sum of muls: %i\n", int(sums));
}

static void doCpu()
{
    a();
    b();
}












bool initCompute()
{
#if 1
    s_buffers[BufferInput] = (createGPUWriteBuffer(((s_input.size() + 15) / 16) * 16, "Input"));

    s_buffers[BufferSums] = (createGPUWriteBuffer(4096 * sizeof(int), "Sums"));
    s_buffers[BufferDos] = (createGPUWriteBuffer(1024 * sizeof(int), "Dos"));
    s_buffers[BufferDonts] = (createGPUWriteBuffer(1024 * sizeof(int), "Donts"));

    s_buffers[BufferAtomicResult] = (createGPUWriteBuffer(1024, "ResultBuffer"));

    // upload the input data to a buffer
    uploadGPUBufferOneTimeInInit(s_buffers[BufferInput], (uint8_t*)s_input.data(), s_input.size());

    // Create compute pipelines
    {
        for(int i = 0; i < PipelineCount; ++i)
        {
            s_pipelines[i] = createComputePipeline(s_pipelineInfos[i]);
        }
    }
#endif
    return true;
}

bool initData()
{
    if((s_input.size() % 4) != 0)
    {
        int count = 4 - (s_input.size() % 4);
        // cannot append \0...
        for(int i = 0; i < count; ++i)
            s_input.append("\1");
    }
    doCpu();
    return initCompute();
}

void deinitData()
{
    SDL_GPUDevice* gpuDevice = getGpuDevice();
    for(SDL_GPUComputePipeline* pipeline : s_pipelines)
    {
        if(pipeline != nullptr)
            SDL_ReleaseGPUComputePipeline(gpuDevice, pipeline);
    }

    for(SDL_GPUBuffer* buffer : s_buffers)
    {
        if(buffer != nullptr)
            SDL_ReleaseGPUBuffer(gpuDevice, buffer);
    }

    printf("03-a compute sum of muls: %i\n", s_dataBuffer[4]);
    printf("03-b compute sum of muls: %i\n", s_dataBuffer[5]);

}

void gpuReadEndBuffers()
{

}
bool renderFrame(SDL_GPUCommandBuffer* cmd, int index)
{
#if 1
    struct DataSize
    {
        int inputBytes;
    };

    DataSize dataSize = {
        .inputBytes = (int)s_input.size(),
    };
    SDL_PushGPUComputeUniformData(cmd, 0, &dataSize, sizeof(dataSize));
    {
        // Atomic reset
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferAtomicResult] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineAtomicBufferReset]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
        // Parse sums, dos and donts
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferInput] },
                { .buffer = s_buffers[BufferSums] },
                { .buffer = s_buffers[BufferDos] },
                { .buffer = s_buffers[BufferDonts] },
                { .buffer = s_buffers[BufferAtomicResult] },
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineParse]);
            SDL_DispatchGPUCompute(computePass, (s_input.size() + 255) / 256, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
        // Calculate sums
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferSums] },
                { .buffer = s_buffers[BufferDos] },
                { .buffer = s_buffers[BufferDonts] },
                { .buffer = s_buffers[BufferAtomicResult] },
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineCalculateSums]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }

    }

    // Get the data from gpu to cpu
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferAtomicResult], 1024);

#endif
    return true;
}
