#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>


#include "d02_comp.h"
/*
#include "d01b_comp.h"
#include "findnumbers_comp.h"
#include "parsenumbers_comp.h"
#include "radixsort_comp.h"
*/
#include "commonrender.h"
#include "commons.h"

enum BufferEnum : int
{
    BufferInput,
    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineD02,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(d02_comp), 2, 1, 256 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);


static const std::string s_Filename = "input/02.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 02";
}

static std::vector<std::vector<int>> parseInts()
{
    std::vector<std::vector<int>> result;
    std::ifstream file(s_Filename, std::ios::binary);
    if(!file.is_open())
    {
        printf("Failed to open file\n");
        return result;
    }
    std::string line;
    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        result.push_back({});
        int value;
        while(iss >> value)
            result.back().push_back(value);
    }
    return result;
}

static bool testSafeA(std::vector<int> values)
{
    int prev = values[0];
    int dir = values[1] > values[0] ? 1 : -1;
    int i = 1;
    for(; i < values.size(); ++i)
    {
        int newDir = values[i] - prev;
        if(newDir > 3 || newDir < -3 || newDir == 0)
        {
            return false;
        }

        if(newDir * dir <= 0)
        {
            return false;
        }

        prev = values[i];
    }
    return true;
}

static int testSafe(std::vector<int> values)
{
    int growing = 0;
    int dec = 0;

    for(int i = 0; i < values.size() - 1; ++i)
    {
        int diff = values[i] - values[i + 1];
        if(diff > 0 && diff <= 3)
            growing++;
        if(diff < 0 && diff >= -3)
            dec++;
    }

    return std::max(growing, dec);
}


static void a()
{
    std::vector<std::vector<int>> values = parseInts();
    int safe = 0;

    for(auto line : values)
    {
        if(testSafe(line) == line.size() - 1)
        {
            safe++;
        }
    }


    printf("02-a safe ones: %i\n", safe);
}

static void b()
{
    std::vector<std::vector<int>> values = parseInts();
    int safe = 0;

    for(auto line : values)
    {

        if(testSafe(line) == line.size() - 1)
        {
            safe++;
            continue;
        }

        for(int i = 0; i < line.size(); ++i)
        {
            std::vector<int> copyline = line;
            copyline.erase(copyline.begin() + i, copyline.begin() + i + 1);
            if(testSafe(copyline) == copyline.size() - 1)
            {
                safe++;
                break;
            }
        }
    }



    printf("02-b safe ones: %i\n", safe);
}

static void doCpu()
{
    a();
    b();
}












bool initCompute()
{
    s_buffers[BufferInput] = (createGPUWriteBuffer(s_input.length(), "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(1024, "ResultBuffer"));

    // upload the input data to a buffer
    uploadGPUBufferOneTimeInInit(s_buffers[BufferInput], (uint8_t*)s_input.data(), s_input.length());

    // Create compute pipelines
    {
        for(int i = 0; i < PipelineCount; ++i)
        {
            s_pipelines[i] = createComputePipeline(s_pipelineInfos[i]);
        }
    }

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


    printf("02-a compute safe: %i\n", s_dataBuffer[0]);
    printf("02-b compute safe: %i\n", s_dataBuffer[1]);

}

bool renderFrame(SDL_GPUCommandBuffer* cmd, int index)
{
    struct DataSize
    {
        int inputBytes;
    };

    DataSize dataSize = {
        .inputBytes = (int)s_input.length(),
    };
    SDL_PushGPUComputeUniformData(cmd, 0, &dataSize, sizeof(dataSize));
    {
        // Get values
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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD02]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }

    // Get the data from gpu to cpu
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], 1024);

    //if(index > 100)
    //    return false;

    return true;
}
