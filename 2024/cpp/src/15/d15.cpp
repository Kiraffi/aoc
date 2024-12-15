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
    int m_x;
    int m_y;
};

static const std::string s_Filename = "input/15.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static int s_dataBuffer[1024] = {};


static std::vector<std::string> s_map;

static V2 s_fish = {};

static std::string s_movement;

const char* getTitle()
{
    return "AOC 2024 day 15";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    bool parsingMap = true;
    std::string movement;
    while(std::getline(iss, line))
    {
        if(line.empty())
        {
            parsingMap = false;
        }
        if(parsingMap)
        {
            size_t pos = line.find('@');
            if(pos != std::string::npos)
            {
                s_fish.m_x = pos;
                s_fish.m_y = s_map.size();
            }
            s_map.push_back(line);
        }
        else
        {
            movement += line;
        }
    }
    s_movement = movement;
}

static char getChar(V2 loc, const std::vector<std::string>& map)
{
    if(loc.m_y < 0 || loc.m_y >= map.size() || loc.m_x < 0 || loc.m_x >= map[loc.m_y].size())
    {
        return '#';
    }
    return map[loc.m_y][loc.m_x];
}

static void swapLoc(V2 a, V2 b, std::vector<std::string>& map)
{
    std::swap(map[a.m_y][a.m_x], map[b.m_y][b.m_x]);
}


static int getHighestSetBit(const std::bitset<128>& bitSet)
{
    int x = 127;
    while(x >= 0 && !bitSet.test(x))
    {
        --x;
    }
    return x;
}

static int getLowestSetBit(const std::bitset<128>& bitSet)
{
    int x = 0;
    while(x < 128 && !bitSet.test(x))
    {
        ++x;
    }
    return x;
}

static void moveDir(int rowIndex, const std::bitset<128>& moveRow, const V2& dir, std::vector<std::string>& map)
{
    SDL_bits_h_
    std::bitset<128> newRow = {};
    int newRowIndex = rowIndex + dir.m_y;
    if(dir.m_y != 0)
    {
        if(!moveRow.any())
        {
            return;
        }
        for(int x = 0; x < map[rowIndex].size(); ++x)
        {
            if(moveRow.test(x))
            {
                char c = getChar(V2{x, newRowIndex}, map);
                if(c == '#')
                {
                    return;
                }
                if(c == '.')
                {
                    continue;
                }
                if(c == '[')
                {
                    newRow.set(x + 1);
                }
                if(c == ']')
                {
                    newRow.set(x - 1);
                }
                newRow.set(x);
            }
        }
        moveDir(newRowIndex, newRow, dir, map);
        for(int x = 0; x < map[rowIndex].size(); ++x)
        {
            if(moveRow.test(x))
            {
                swapLoc(V2{x, rowIndex}, V2{x, newRowIndex}, map);
            }
        }
        return;
    }

    // move X
    newRow = moveRow;

    // should never be
    if(newRow == 0)
    {
        assert(false);
        return;
    }

    int bitIndex = dir.m_x > 0 ? getHighestSetBit(newRow) + 1: getLowestSetBit(newRow) - 1;
    V2 newLoc = {bitIndex, rowIndex + dir.m_y};
    char c = getChar(newLoc, map);

    int oldBitIndex = dir.m_x > 0 ? getHighestSetBit(newRow): getLowestSetBit(newRow);
    V2 oldLoc = {oldBitIndex, rowIndex};

    if(c == '#')
    {
        assert(false);
        return;
    }
    if(c == '.')
    {
        swapLoc(newLoc, oldLoc, map);
        return;
    }
    newRow.set(bitIndex);
    moveDir(newRowIndex, newRow, dir, map);

    swapLoc(newLoc, oldLoc, map);
}

static bool canMoveDir(int rowIndex, const std::bitset<128>& moveRow, const V2& dir, std::vector<std::string>& map)
{
    std::bitset<128> newRow = {};
    int newRowIndex = rowIndex + dir.m_y;
    if(dir.m_y != 0)
    {
        for(int x = 0; x < map[rowIndex].size(); ++x)
        {
            if(moveRow.test(x))
            {
                char c = getChar(V2{x, newRowIndex}, map);
                if(c == '#')
                {
                    return false;
                }
                if(c == '.')
                {
                    continue;
                }
                if(c == '[')
                {
                    newRow.set(x + 1);
                }
                if(c == ']')
                {
                    newRow.set(x - 1);
                }
                newRow.set(x);
            }
        }
        if(!newRow.any())
        {
            return true;
        }
        return canMoveDir(newRowIndex, newRow, dir, map);
    }
    // move X
    newRow = moveRow;

    // should never be
    if(!newRow.any())
    {
        assert(false);
        return false;
    }

    int bitIndex = dir.m_x > 0 ? getHighestSetBit(newRow) + 1: getLowestSetBit(newRow) - 1;
    V2 newLoc = {bitIndex, newRowIndex};
    char c = getChar(newLoc, map);

    if(c == '#')
    {
        return false;
    }
    if(c == '.')
    {
        return true;
    }
    newRow.set(bitIndex);
    return canMoveDir(newRowIndex, newRow, dir, map);
}

static void move(V2& fish, char movementDir, std::vector<std::string>& map)
{
    V2 dir = {};

    switch(movementDir)
    {
        case '<': dir.m_x = -1; break;
        case '>': dir.m_x = +1; break;
        case '^': dir.m_y = -1; break;
        case 'v': dir.m_y = +1; break;
        default: return;
    }
    std::bitset<128> moveRow = {};
    moveRow.set(fish.m_x);
    if(canMoveDir(fish.m_y, moveRow, dir, map))
    {
        moveDir(fish.m_y, moveRow, dir, map);
        fish.m_x += dir.m_x;
        fish.m_y += dir.m_y;
    }
}

static void drawMap(const std::vector<std::string>& map)
{
    for(const auto& s : map)
    {
        printf("%s\n", s.c_str());
    }
}

static void a()
{
    int64_t count = 0;

    std::vector<std::string> copyMap = s_map;
    V2 fish = s_fish;

    //drawMap(copyMap);


    for(char c : s_movement)
    {
        //printf("\n%c\n", c);
        move(fish, c, copyMap);
        //drawMap(copyMap);
    }
    //printf("\n");
    //drawMap(copyMap);

    for(int j = 1; j < copyMap.size() - 1; ++j)
    {
        for(int i = 1; i < copyMap[j].size() - 1; ++i)
        {
            if(getChar(V2{i, j}, copyMap) == 'O')
            {
                count += j * 100 + i;
            }
        }
    }

    printf("15-a Box gps positions %" SDL_PRIs64 "\n", count);
}


static void b()
{
    int64_t count = 0;

    std::vector<std::string> copyMap;

    for(const auto& s : s_map)
    {
        copyMap.push_back({});
        std::string& str = copyMap.back();
        for(char c : s)
        {
            if(c == '#')
            {
                str.append("##");
            }
            if(c == 'O')
            {
                str.append("[]");
            }
            if(c == '.')
            {
                str.append("..");
            }
            if(c == '@')
            {
                str.append("@.");
            }
        }
    }


    V2 fish = s_fish;
    fish.m_x *= 2;

    //drawMap(copyMap);

    for(char c : s_movement)
    {
        //printf("\n%c %i\n", c, moveCount);
        move(fish, c, copyMap);
        //drawMap(copyMap);
    }
    //printf("\n");
    //drawMap(copyMap);

    for(int j = 1; j < copyMap.size() - 1; ++j)
    {
        for(int i = 1; i < copyMap[j].size() - 1; ++i)
        {
            if(getChar(V2{i, j}, copyMap) == '[')
            {
                count += j * 100 + i;
            }
        }
    }

    printf("15-b Box gps positions %" SDL_PRIs64 "\n", count);
}

static void doCpu()
{
    parse();
    a();
    b();
}












bool initCompute()
{
#if 0
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
void gpuReadEndBuffers()
{

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

    // Get the data from gpu to cpu
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], 1024);

#endif
    return true;
}
