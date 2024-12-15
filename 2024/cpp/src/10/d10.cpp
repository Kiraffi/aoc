#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>

#include "d10_comp.h"

#include "commons.h"
#include "commonrender.h"

static const int ValuesBufferSize = 1024;


enum BufferEnum : int
{
    BufferInput,
    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineD10,

    PipelineCount
};

static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(d10_comp), 2, 1, 1024 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);

static const std::string s_Filename = "input/10.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::vector<std::string> s_map;



static int s_mapWidth = 0;
static int s_mapHeight = 0;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 10";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    while(std::getline(iss, line))
    {
        s_map.push_back(line);
    }
    s_mapWidth = s_map[0].length();
    s_mapHeight = s_map.size();
}


static int64_t findTrailheads(
    int x,
    int y,
    int expectedNumber,
    int visitedNumber,
    std::vector<std::vector<int>>& visited)
{
    if(x < 0 || x >= s_mapWidth || y < 0 || y >= s_mapHeight)
    {
        return 0;
    }
    int number = s_map[y][x] - '0';
    if(number != expectedNumber)
    {
        return 0;
    }
    if(number == 9)
    {
        if(visited[y][x])
        {
            return 0;
        }
        visited[y][x] = visitedNumber;
        return 1;
    }

    int64_t sum = 0;
    sum += findTrailheads(x - 1, y, expectedNumber + 1, visitedNumber, visited);
    sum += findTrailheads(x + 1, y, expectedNumber + 1, visitedNumber, visited);
    sum += findTrailheads(x, y - 1, expectedNumber + 1, visitedNumber, visited);
    sum += findTrailheads(x, y + 1, expectedNumber + 1, visitedNumber, visited);
    return sum;
}


static void a()
{
    std::vector<int> numbers;

    int64_t count = 0;

    for(int y = 0; y < s_mapHeight; ++y)
    {
        for(int x = 0; x < s_mapWidth; ++x)
        {
            if(s_map[y][x] == '0')
            {
                std::vector<std::vector<int>> visited(s_mapHeight, std::vector<int>(s_mapWidth, 0));
                int64_t heads = findTrailheads(x, y, 0, 1, visited);
                count += heads;

            }
        }
    }

    printf("10-a Trailheads %" SDL_PRIs64 "\n", count);
}

static void b()
{
    int64_t count = 0;
    for(int y = 0; y < s_mapHeight; ++y)
    {
        for(int x = 0; x < s_mapWidth; ++x)
        {
            if(s_map[y][x] == '0')
            {
                std::vector<std::vector<int>> visited(s_mapHeight, std::vector<int>(s_mapWidth, 0));
                int64_t heads = findTrailheads(x, y, 0, 0, visited);
                count += heads;

            }
        }
    }

    printf("10-b ratings of trailheads %" SDL_PRIs64 "\n", count);
}

static void doCpu()
{
    parse();
    a();
    b();
}












bool initCompute()
{
    s_buffers[BufferInput] = (createGPUWriteBuffer(s_input.size(), "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(ValuesBufferSize * sizeof(int), "ResultBuffer"));

    // upload the input data to a buffer
    uploadGPUBufferOneTimeInInit(s_buffers[BufferInput], (uint8_t*)s_input.data(), s_input.size());

#if 1
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
void gpuReadEndBuffers()
{
    // Get the data from gpu to cpu
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], ValuesBufferSize * sizeof(int));
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
    int computeDebugNumbers = s_dataBuffer[2];
    printf("Compute debug number count: %i\n", computeDebugNumbers);
    for(int i = 0; i < computeDebugNumbers; ++i)
    {
        printf("%i\n", s_dataBuffer[i + 4]);
    }

    printf("10-a compute Trailheads: %i\n", s_dataBuffer[0]);
    printf("10-b compute ratings of trailheads: %i\n", s_dataBuffer[1]);

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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD10]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }

#endif
    return true;
}
