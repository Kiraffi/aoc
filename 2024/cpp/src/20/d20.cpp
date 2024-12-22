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

static const std::string s_Filename = "input/20.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

using StrMap = std::vector<std::string>;
using FloodMap = std::vector<std::vector<int>>;

static StrMap s_map;

static int s_dataBuffer[ValuesBufferSize] = {};

static int s_mapWidth = 0;
static int s_mapHeight = 0;

static V2 s_start = {};
static V2 s_end = {};

const char* getTitle()
{
    return "AOC 2024 day 20";
}


static void parse()
{
    int row = 0;
    std::string line;
    std::istringstream iss(s_input);
    while(std::getline(iss, line))
    {
        int col = line.find('S');
        if(col != std::string::npos)
        {
            s_start.m_x = col;
            s_start.m_y = row;
        }
        col = line.find('E');
        if(col != std::string::npos)
        {
            s_end.m_x = col;
            s_end.m_y = row;
        }
        s_map.push_back(line);
        ++row;
    }
    s_mapWidth = s_map[0].size();
    s_mapHeight = s_map.size();
}

static void printSteps(const FloodMap& map)
{
    for(int j = 0; j < s_mapHeight; ++j)
    {
        for(int i = 0; i < s_mapWidth; ++i)
        {
            if(s_map[j][i] == '#')
            {
                printf("#");
            }
            else
            {
                printf("%i", map[j][i] % 10);
            }
        }
        printf("\n");
    }
}

static void printMap(const std::vector<std::string>& map)
{
    for(const auto& s : map)
    {
        printf("%s\n", s.c_str());
    }
}

bool isValidMoveTile(int x, int y)
{
    if(x < 0 || y < 0 || x >= s_mapWidth || y >= s_mapHeight)
    {
        return false;
    }
    if(s_map[y][x] == '#')
    {
        return false;
    }
    return true;
}



static bool setStep(int x, int y, int stepCount, const StrMap& map, FloodMap& floodMap)
{
    if(!isValidMoveTile(x, y))
    {
        return false;
    }
    int& tile = floodMap[y][x];
    int value = tile;
    tile = std::min(tile, stepCount);
    return value != tile;
}

static bool floodFillMapStep(int stepCount, const StrMap& map, FloodMap& outFloodMap)
{
    bool changed = false;
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
    return changed;
}

static FloodMap getEmptyFloodMap()
{
    FloodMap floodMap(s_mapHeight, std::vector<int>(s_mapWidth, 0x3fff'ffff));
    floodMap[s_end.m_y][s_end.m_x] = 0;
    return floodMap;
}

static int64_t fullFloodFrom(int64_t index, FloodMap& map)
{
    while(floodFillMapStep(index, s_map, map))
    {
        ++index;
    }
    return map[s_start.m_y][s_start.m_x];
}

static int64_t cheat(int64_t cheatAmount, const FloodMap& floodMap)
{
    int64_t result = 0;
    for(int y = 1; y < s_mapHeight - 1; ++y)
    {
        for(int x = 1; x < s_mapWidth - 1; ++x)
        {
            if(!isValidMoveTile(x, y))
            {
                continue;
            }
            int64_t distanceToEnd = floodMap[y][x];
            for(int j = y - cheatAmount; j <= y + cheatAmount; ++j)
            {
                int yCheatAmount = std::abs(j - y);
                int yCheatLeft = cheatAmount - yCheatAmount;
                for(int i = x - yCheatLeft; i <= x + yCheatLeft; ++i)
                {
                    if(!isValidMoveTile(i, j))
                    {
                        continue;
                    }
                    int64_t xCheatAmount = std::abs(i - x);
                    int64_t cheatUsedAmount = xCheatAmount + yCheatAmount;
                    int64_t oldValue = floodMap[j][i];
                    int64_t save = oldValue - (distanceToEnd + cheatUsedAmount);
                    if(save >= 100)
                    {
                        result++;
                    }
                }
            }
        }
    }
    return result;
}

static void a()
{
    int64_t result = 0;
    std::vector<int> saves;
    FloodMap floodMap = getEmptyFloodMap();
    int64_t noCheatMoves = fullFloodFrom(0, floodMap);

    //printSteps(floodMap);

    result = cheat(2, floodMap);

    //printMap(map);
    printf("20-a Cheat amounts %" SDL_PRIs64 "\n", result);
}

static void b()
{
    int64_t result = 0;
    std::vector<int> saves;
    FloodMap floodMap = getEmptyFloodMap();
    int64_t noCheatMoves = fullFloodFrom(0, floodMap);

    //printSteps(floodMap);

    result = cheat(20, floodMap);

    //printMap(map);
    printf("20-b Cheat amount %" SDL_PRIs64 "\n", result);

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
