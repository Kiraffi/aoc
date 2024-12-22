#include <algorithm>
#include <bit> //std::popcount, std::countr
#include <bitset>
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


/*
#include "d02_comp.h"
#include "d01b_comp.h"
#include "findnumbers_comp.h"
#include "parsenumbers_comp.h"
#include "radixsort_comp.h"
*/
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
    PipelineD02,

    PipelineCount
};

/*
static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(atomic_buffers_reset_comp), 1, 0, 1 },
    { BUF_N_SIZE(d04_calculate_xmas_comp), 2, 1, 256 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);
*/

struct V2
{
    void operator+=(V2 other) { m_x += other.m_x; m_y += other.m_y; }
    //V2 operator+(V2 other) { return {m_x + other.m_x, m_y + other.m_y };}
    int m_x;
    int m_y;
};

V2 operator+(V2 lhs, V2 rhs) { return {lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y };}

static const std::string s_Filename = "input/18.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::vector<V2> s_blocks;

static int s_dataBuffer[ValuesBufferSize] = {};

static const int s_mapWidth = 71;
static const int s_mapHeight = 71;

const char* getTitle()
{
    return "AOC 2024 day 18";
}


static void parse()
{
    std::string line;
    std::istringstream iss(s_input);
    while(std::getline(iss, line))
    {
        std::istringstream iss2(line);
        char c;
        V2 v;
        iss2 >> v.m_x;
        iss2 >> c;
        iss2 >> v.m_y;
        s_blocks.push_back(v);
    }
}

static void printMap(const std::vector<std::string>& map)
{
    for(const auto& s : map)
    {
        printf("%s\n", s.c_str());
    }
}

using StrMap = std::vector<std::string>;
using FloodMap = std::vector<std::vector<int>>;

static bool setStep(int x, int y, int stepCount, const StrMap& map, FloodMap& floodMap)
{
    if(x < 0 || y < 0 || x >= s_mapWidth || y >= s_mapHeight)
    {
        return false;
    }
    if(map[y][x] == '#')
    {
        return false;
    }
    int& tile = floodMap[y][x];
    int value = tile;
    tile = std::min(tile, stepCount);
    return value != tile;
}

static void floodFillMap(const StrMap& map, FloodMap& outFloodMap)
{
    outFloodMap[0][0] = 0;
    int stepCount = 0;
    bool changed = true;
    while(changed)
    {
        changed = false;
        for(int j = 0; j < s_mapHeight; ++j)
        {
            for(int i = 0; i < s_mapWidth; ++i)
            {
                if(outFloodMap[j][i] == stepCount)
                {
                    changed = setStep(i + 1, j + 0, stepCount + 1, map, outFloodMap) || changed;
                    changed = setStep(i - 1, j + 0, stepCount + 1, map, outFloodMap) || changed;
                    changed = setStep(i + 0, j - 1, stepCount + 1, map, outFloodMap) || changed;
                    changed = setStep(i + 0, j + 1, stepCount + 1, map, outFloodMap) || changed;
                }
            }
        }
        ++stepCount;
    }
}

static void dropBlocks(int count, StrMap& map)
{
    for(int i = 0; i < count; ++i)
    {
        V2 v = s_blocks[i];
        map[v.m_y][v.m_x] = '#';
    }
}

static void a()
{
    int64_t result = 0;
    StrMap map(s_mapHeight, std::string(s_mapWidth, '.'));
    dropBlocks(1024, map);

    FloodMap floodMap(s_mapHeight, std::vector<int>(s_mapWidth, 0x3fff'ffff));
    floodFillMap(map, floodMap);;

    //printMap(map);
    printf("18-a Smallest steps %i\n", floodMap[s_mapHeight - 1][s_mapWidth - 1]);
    //printf("18-a Smallest steps %" SDL_PRIs64 "\n", result);
}

static void b()
{
    int64_t result = 0;

    int64_t min = 0;
    int64_t max = s_blocks.size() - 1;

    while(min + 1 < max)
    {
        result = (min + max) / 2;
        StrMap map(s_mapHeight, std::string(s_mapWidth, '.'));
        dropBlocks(result, map);

        FloodMap floodMap(s_mapHeight, std::vector<int>(s_mapWidth, 0x3fff'ffff));
        floodFillMap(map, floodMap);
        if(floodMap[s_mapHeight - 1][s_mapWidth - 1] == 0x3fff'ffff)
        {
            max = result;
        }
        else
        {
            min = result;
        }
    }

    printf("18-b %i,%i blocks path\n", s_blocks[result].m_x, s_blocks[result].m_y);

    //printf("18-b Smallet value to print same output as input %" SDL_PRIs64 "\n", result);

}

static void doCpu()
{
    parse();
    a();
    b();
}












bool initCompute()
{
    s_buffers[BufferInput] = (createGPUWriteBuffer(((s_input.size() + 15) / 16) * 16, "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(ValuesBufferSize * sizeof(int), "ResultBuffer"));

    // upload the input data to a buffer
    uploadGPUBufferOneTimeInInit(s_buffers[BufferInput], (uint8_t*)s_input.data(), s_input.size());

#if 0
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


    printf("03-a compute safe: %i\n", s_dataBuffer[0]);
    printf("03-b compute safe: %i\n", s_dataBuffer[1]);

}

bool renderFrame(SDL_GPUCommandBuffer* cmd, int index)
{
#if 0
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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD02]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }
#endif
    return true;
}
