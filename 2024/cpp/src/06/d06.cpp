#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>

#include "../atomic_buffers_reset_comp.h"
#include "d06_comp.h"

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
    PipelineD06,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(atomic_buffers_reset_comp), 1, 0, 1 },
    { BUF_N_SIZE(d06_comp), 2, 1, 1024 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);


static const std::string s_Filename = "input/06.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static std::vector<std::string> s_map;
static int s_mapSizeX = 0;
static int s_mapSizeY = 0;
static int s_startX = 0;
static int s_startY = 0;

static int s_dataBuffer[1024] = {};



struct Position
{
    int x;
    int y;
    int dir;
    int tmp;

    void operator+=(const Position& other)
    {
        x += other.x;
        y += other.y;
    }

    void operator-=(const Position& other)
    {
        x -= other.x;
        y -= other.y;
    }

    bool isValidPos() const
    {
        return x >= 0
            && y >= 0
            && x < s_mapSizeX
            && y < s_mapSizeY;
    }
    bool isBlocked() const
    {
        if(!isValidPos())
        {
            return false;
        }
        return s_map[y][x] == '#';
    }
    static Position getDir(int dir)
    {
        switch(dir)
        {
            case 0: return Position{+0, -1, 0};
            case 1: return Position{+1, +0, 1};
            case 2: return Position{+0, +1, 2};
            case 3: return Position{-1, +0, 3};

        }
        assert(false);
        return Position{};
    }
};


const char* getTitle()
{
    return "AOC 2024 day 06";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    while(std::getline(iss, line))
    {
        s_map.push_back(line);
    }
    s_mapSizeY = s_map.size();
    s_mapSizeX = s_map[0].size();

    s_startX = 0;
    s_startY = 0;

    for(const auto& row : s_map)
    {
        auto iter = row.find('^');

        if(iter != std::string::npos)
        {
            s_startX = iter;
            break;
        }
        ++s_startY;
    }

}

static void moveForward(Position& pos)
{
    Position tmp = pos;
    tmp.dir = (tmp.dir + 3) % 4;
    do
    {
        tmp.dir = (tmp.dir + 1) % 4;
        tmp.x = pos.x;
        tmp.y = pos.y;
        tmp += Position::getDir(tmp.dir);
    } while (tmp.isBlocked());
    pos = tmp;
}

static void jumpForward(Position& pos, const Position& wall,
    const std::vector<std::vector<std::vector<Position>>>& jumpMap)
{
    Position goal = jumpMap[pos.y][pos.x][pos.dir];
    goal.dir = pos.dir;
    switch(pos.dir)
    {
        case 0: goal.y = pos.x == wall.x && pos.y > wall.y ? std::max(wall.y, goal.y) : goal.y; break;
        case 1: goal.x = pos.y == wall.y && pos.x < wall.x ? std::min(wall.x, goal.x) : goal.x; break;
        case 2: goal.y = pos.x == wall.x && pos.y < wall.y ? std::min(wall.y, goal.y) : goal.y; break;
        case 3: goal.x = pos.y == wall.y && pos.x > wall.x ? std::max(wall.x, goal.x) : goal.x; break;
    }

    if(goal.isValidPos() && goal.isBlocked())
    {
        goal -= Position::getDir(pos.dir);
        goal.dir = (pos.dir + 1) % 4;
    }
    Position tmpGoal = goal;
    moveForward(tmpGoal);
    goal.dir = tmpGoal.dir;
    pos = goal;
}

static void a()
{
    int64_t count = 0;

    std::vector<std::vector<bool>> visited;
    visited.resize(s_map.size(), std::vector<bool>(s_map[0].size(), false));
    Position pos{ s_startX, s_startY, 0 };

    while(pos.isValidPos())
    {
        visited[pos.y][pos.x] = true;
        moveForward(pos);
    }
    for(const auto& row : visited)
    {
        for(bool hasVisited : row)
        {
            if(hasVisited)
            {
                ++count;
            }
        }
    }

    printf("06-a Distinct positions: %i\n", int(count));
}

static void b()
{
    int64_t count = 0;
    static const int MapSizeBytes = 140 * 140;
    assert(s_mapSizeX * s_mapSizeY <= MapSizeBytes);
#define USE_JUMP_MAP 1
#if USE_JUMP_MAP
    std::vector<std::vector<std::vector<Position>>> jumpMap;
    jumpMap.resize(s_mapSizeY, std::vector<std::vector<Position>>(
        s_mapSizeX, std::vector<Position>(4)));
    for(int y = 0; y < s_mapSizeY; ++y)
    {
        for(int x = 0; x < s_mapSizeX; ++x)
        {
            for(int dir = 0; dir < 4; ++dir)
            {
                Position tmp = {x, y, dir};
                if(!tmp.isValidPos() || tmp.isBlocked())
                {
                    continue;
                }
                Position movDir = Position::getDir(tmp.dir);
                while(tmp.isValidPos() && !tmp.isBlocked())
                {
                    tmp += movDir;
                }

                Position goal = tmp;
                while(tmp.x != x || tmp.y != y)
                {
                    if(tmp.isValidPos())
                    {
                        jumpMap[tmp.y][tmp.x][tmp.dir] = goal;
                    }
                    tmp -= movDir;
                }
                jumpMap[y][x][dir] = goal;
            }
        }

    }
#endif // USE_JUMP_MAP
    Position pos{s_startX, s_startY, 0};
    char obstaclePositions[MapSizeBytes] = {};
    while(pos.isValidPos())
    {
        Position wall = pos;
        moveForward(wall);
        if(!wall.isValidPos())
        {
            break;
        }
        if(wall.isBlocked())
        {
            moveForward(pos);
            continue;
        }
        {
            char& wallTile = obstaclePositions[wall.y * s_mapSizeX + wall.x];
            if(wallTile != 0)
            {
                moveForward(pos);
                continue;
            }
            wallTile = 1;
        }
        char original = '#';
        std::swap(s_map[wall.y][wall.x], original);

        char visited[MapSizeBytes] = {};

        Position tmp = pos;
#if USE_JUMP_MAP
        jumpForward(tmp, wall, jumpMap);
#endif

        while(tmp.isValidPos())
        {
            char& c = visited[tmp.y * s_mapSizeX + tmp.x];
            // if(c) if we start with jump forward, we should only need to check if visited
            if((c >> tmp.dir) & 1)
            {
                ++count;
                break;
            }
            c |= (1 << tmp.dir);
#if USE_JUMP_MAP
            jumpForward(tmp, wall, jumpMap);
#else
            moveForward(tmp);
#endif // USE_JUMP_MAP
        }
        std::swap(s_map[wall.y][wall.x], original);

        moveForward(pos);
    }

    printf("06-b Different positions for obstruction: %i\n", int(count));
}

static void doCpu()
{
    parse();
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

void gpuReadEndBuffers()
{
    // Get the data from gpu to cpu
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], 1024);
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

    printf("06-a compute Distinct positions: %i\n", s_dataBuffer[0]);
    printf("06-b compute Different positions for obstruction: %i\n", s_dataBuffer[1]);
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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD06]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }
#endif
    return true;
}
