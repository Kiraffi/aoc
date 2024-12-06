#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>


#include "atomic_buffers_reset_comp.h"
#include "d04_calculate_xmas_comp.h"

#include "commons.h"
#include "commonrender.h"


enum BufferEnum : int
{
    BufferInput,
    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineAtomicBufferReset,
    PipelineD04,

    PipelineCount
};

static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(atomic_buffers_reset_comp), 1, 0, 1 },
    { BUF_N_SIZE(d04_calculate_xmas_comp), 2, 1, 256 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);


static const std::string s_Filename = "input/04.input";

static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);


static std::vector<std::string> s_input2d;

static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 04";
}

static void parse2d()
{
    int startChar = 0;
    for(int i = 0; i < s_input.length(); ++i)
    {
        if(s_input[i] == '\n')
        {
            s_input2d.push_back(s_input.substr(startChar, i - startChar));
            startChar = i + 1;
        }
    }
}

char isChar(int x, int y, char c)
{
    if(y < 0 || y >= s_input2d.size())
    {
        return false;
    }
    if(x < 0 || x >= s_input2d[y].size())
    {
        return false;
    }

    return (s_input2d[y])[x] == c;
}

bool checkWord(int x, int y, int dirX, int dirY, const std::string& findWord)
{
    int len = findWord.length();
    for(int i = 0; i < len; ++i)
    {
        if(!isChar(x + dirX * i, y + dirY * i, findWord[i]))
        {
            return false;
        }
    }

    return true;
}

bool checkMasFromA(int x, int y, int dirX, int dirY)
{
    static std::string s_findWord = "MAS";
    return checkWord(x - dirX, y - dirY, dirX, dirY, s_findWord);
}

bool checkX_MAS(int x, int y)
{
    static std::string s_findWord = "MAS";
    int count = 0;

    if(checkMasFromA(x, y,  1,  1)) count++;
    if(checkMasFromA(x, y, -1,  1)) count++;
    if(checkMasFromA(x, y,  1, -1)) count++;
    if(checkMasFromA(x, y, -1, -1)) count++;

    return count >= 2;
}

static void a()
{
    static std::string s_findWord = "XMAS";

    int64_t count = 0;
    for(int y = 0; y < s_input2d.size(); ++y)
    {
        for(int x = 0; x < s_input2d[y].length(); ++x)
        {
            if((s_input2d[y])[x] == 'X')
            {
                if(checkWord(x, y,  1,  0, s_findWord)) count++;
                if(checkWord(x, y, -1,  0, s_findWord)) count++;
                if(checkWord(x, y,  0,  1, s_findWord)) count++;
                if(checkWord(x, y,  0, -1, s_findWord)) count++;

                if(checkWord(x, y,  1,  1, s_findWord)) count++;
                if(checkWord(x, y, -1,  1, s_findWord)) count++;
                if(checkWord(x, y,  1, -1, s_findWord)) count++;
                if(checkWord(x, y, -1, -1, s_findWord)) count++;
            }
        }
    }
    printf("04-a XMAS appears %i times.\n", int(count));
}

static void b()
{
    int64_t count = 0;
    for(int y = 0; y < s_input2d.size(); ++y)
    {
        for(int x = 0; x < s_input2d[y].length(); ++x)
        {
            if((s_input2d[y])[x] == 'A')
            {
                if(checkX_MAS(x, y)) count++;
            }
        }
    }
    printf("04-b X-MAS appears %i times.\n", int(count));
}

static void doCpu()
{
    parse2d();
    a();
    b();
}












bool initCompute()
{
#if 1

    s_buffers[BufferInput] = (createGPUWriteBuffer(s_input.size(), "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(1024, "ResultBuffer"));

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


    printf("04-a compute XMAS appears: %i\n", s_dataBuffer[0]);
    printf("04-b compute X-MAS appears: %i\n", s_dataBuffer[1]);

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
        // Reset atomic buffer
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferResult] }
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
        // Calculate XMAS and X-MAS
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferInput] },
                { .buffer = s_buffers[BufferResult] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD04]);
            SDL_DispatchGPUCompute(computePass, 256 / 16, 256 / 16, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }

    // Get the data from gpu to cpu
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], 1024);

    //if(index > 100)
    //    return false;
#endif
    return true;
}
