#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>




#include "d01a_comp.h"
#include "d01b_comp.h"
#include "findnumbers_comp.h"
#include "parsenumbers_comp.h"
#include "radixsort_comp.h"

#include "simplesort_comp.h"

#include "commons.h"
#include "commonrender.h"


enum BufferEnum : int
{
    BufferInput,

    BufferInputNumbersPositions,
    BufferInputNumbersPositionsSorted,

    BufferLeft,
    BufferLeftSorted,

    BufferRight,
    BufferRightSorted,

    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineInputFindNumbers,
    PipelineInputParseNumbers,

    PipelineRadix,
    PipelineSimpleSort,

    Pipelined01a,
    Pipelined01b,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(findnumbers_comp), 2, 1, 256 },
    { BUF_N_SIZE(parsenumbers_comp), 4, 0, 256 },
    { BUF_N_SIZE(radixsort_comp), 2, 1, 256 },
    { BUF_N_SIZE(simplesort_comp), 2, 1, 1024 },
    { BUF_N_SIZE(d01a_comp), 3, 0, 1024 },
    { BUF_N_SIZE(d01b_comp), 3, 0, 1024 },
};

static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);


static const std::string s_Filename = "input/01.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);


static int s_dataBuffer[1024] = {};

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



static void a()
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

static void b()
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

static void doCpu()
{
    a();
    b();
}












bool initCompute()
{
    s_buffers[BufferInput] = (createGPUWriteBuffer(s_input.length(), "Input"));

    s_buffers[BufferInputNumbersPositions] = (createGPUWriteBuffer(2000 * sizeof(int), "InputPositions"));
    s_buffers[BufferInputNumbersPositionsSorted] = (createGPUWriteBuffer(2000 * sizeof(int), "InputPositionsSorted"));

    s_buffers[BufferLeft] = (createGPUWriteBuffer(1024 * sizeof(int), "Left"));
    s_buffers[BufferLeftSorted] = (createGPUWriteBuffer(1024 * sizeof(int), "LeftSorted"));
    s_buffers[BufferRight] = (createGPUWriteBuffer(1024 * sizeof(int), "Right"));
    s_buffers[BufferRightSorted] = (createGPUWriteBuffer(1024 * sizeof(int), "RightSorted"));

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


    printf("01-a compute difference: %i\n", s_dataBuffer[0]);
    printf("01-b compute simularity: %i\n", s_dataBuffer[1]);
}

bool renderFrame(SDL_GPUCommandBuffer* cmd, int index)
{
    struct DataSize
    {
        int dataNumbers = 1000;
        int dataSize2;
    };
    DataSize dataSize = {
        .dataNumbers = 2000,
        .dataSize2 = (int)s_input.length(),
    };
    SDL_PushGPUComputeUniformData(cmd, 0, &dataSize, sizeof(dataSize));
    {
        // Find number positions
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferInput] },
                { .buffer = s_buffers[BufferInputNumbersPositions] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineInputFindNumbers]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }

        // Radix sort number positions
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferInputNumbersPositions] },
                { .buffer = s_buffers[BufferInputNumbersPositionsSorted] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineSimpleSort]);
            //SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineRadix]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }

        // parse number
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferInput] },
                { .buffer = s_buffers[BufferInputNumbersPositionsSorted] },
                { .buffer = s_buffers[BufferLeft] },
                { .buffer = s_buffers[BufferRight] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineInputParseNumbers]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }
    dataSize.dataNumbers = 1000;
    SDL_PushGPUComputeUniformData(cmd, 0, &dataSize, sizeof(dataSize));

    {
        // Do radix sorting for the numbers
        for(int i = 0; i < 2; ++i)
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferLeft + i * 2] },
                { .buffer = s_buffers[BufferLeftSorted + i * 2] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );
            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineSimpleSort]);
            //SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineRadix]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);
        }

        // 01a, o1b
        for(int i = 0; i < 2; ++i)
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferLeftSorted] },
                { .buffer = s_buffers[BufferRightSorted] },
                { .buffer = s_buffers[BufferResult] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[Pipelined01a + i]);
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
