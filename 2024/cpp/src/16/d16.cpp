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

static const std::string s_Filename = "input/16.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static int s_dataBuffer[ValuesBufferSize] = {};


static std::vector<std::string> s_map;

static V2 s_start = {};
static V2 s_end = {};


struct Reindeer
{
    V2 m_pos;
    int m_dir;
};

const char* getTitle()
{
    return "AOC 2024 day 16";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    std::string movement;
    while(std::getline(iss, line))
    {
        size_t pos = line.find('S');
        if(pos != std::string::npos)
        {
            s_start.m_x = pos;
            s_start.m_y = s_map.size();
        }
        pos = line.find('E');
        if(pos != std::string::npos)
        {
            s_end.m_x = pos;
            s_end.m_y = s_map.size();
        }
        s_map.push_back(line);
    }
}

static char getChar(V2 loc, const std::vector<std::string>& map)
{
    if(loc.m_y < 0 || loc.m_y >= map.size() || loc.m_x < 0 || loc.m_x >= map[loc.m_y].size())
    {
        return '#';
    }
    return map[loc.m_y][loc.m_x];
}

static void drawMap(const std::vector<std::string>& map)
{
    for(const auto& s : map)
    {
        printf("%s\n", s.c_str());
    }
}

static void addVisited(V2 p, std::unordered_set<int>& visited)
{
    int index = p.m_x + (p.m_y << 10);
    visited.insert(index);
}

static bool hasVisited(V2 p, std::unordered_set<int>& visited)
{
    int index = p.m_x + (p.m_y << 10);
    return visited.contains(index);
}
struct Point
{
    V2 m_pos;
    int m_dir;
};

static bool isWall(V2 pos)
{
    return s_map[pos.m_y][pos.m_x] == '#';
}

static bool isGoal(V2 pos)
{
    return s_map[pos.m_y][pos.m_x] == 'E';
}

/*
static int64_t findPath(Reindeer r)
{
    int64_t result = -1;
    std::unordered_set<int> visited;
    addVisited(r.m_pos, visited);

    std::vector<Point> run;
    run.push_back(Point{r, 0});
    run.push_back(Point{Reindeer{r.m_pos, 1}, 1000});
    run.push_back(Point{Reindeer{r.m_pos, 3}, 1000});


    while(run.size() > 0)
    {
        Point p = run.back();
        V2 moveDir = {};
        switch(p.m_r.m_dir)
        {
            case 0: moveDir.m_x = 1; break;
            case 1: moveDir.m_y = 1; break;
            case 2: moveDir.m_x = -1; break;
            case 3: moveDir.m_y = -1; break;
            default: break;
        }
        p.m_r.m_pos.m_x += moveDir.m_x;
        p.m_r.m_pos.m_y += moveDir.m_y;

        if(isWall(p.m_r.m_pos))
        {
            continue;
        }

        if(hasVisited(p.m_r.m_pos, visited))
        {
            continue;
        }
        if(isGoal(p.m_r.m_pos))
        {
            if(result == -1)
            {
                result = p.m_score;
            }
            else
            {
                result = std::min(int64_t(p.m_score), result);
            }
        }
        addVisited(p.m_r.m_pos, visited);

    }

    return result;
}
*/

bool isValid(V2 pos, const std::vector<std::string>& map)
{
    char c = map[pos.m_y][pos.m_x];
    if(c == '#' || c == '*')
    {
        return false;
    }

    return true;
}

static V2 getDir(int d)
{
    switch(d)
    {
        case 0: return {1, 0}; break;
        case 1: return {0, 1}; break;
        case 2: return {-1, 0}; break;
        case 3: return {0, -1}; break;
    }
    return {};
}

static int getDirFrom(V2 newPos, V2 oldPos)
{
    if(newPos.m_x > oldPos.m_x)
    {
        return 0;
    }
    if(newPos.m_y > oldPos.m_y)
    {
        return 1;
    }
    if(newPos.m_x < oldPos.m_x)
    {
        return 2;
    }
    return 3;
}

static int64_t findPath(std::vector<std::string>& map)
{
    int64_t result = 0x0fff'ffff'ffff'ffff;
    std::vector<std::vector<int64_t>> scoreMap(map.size(), std::vector<int64_t>(map[0].size(), result));

    std::vector<std::vector<char>> markMap(map.size(), std::vector<char>(map[0].size(), 0));

    std::vector<int> scores;
    scores.resize(1024*1024);

    std::vector<Point> points;
    points.resize(1024 * 1024);
    points[0] = {s_start, 0};

    int currentPoint = 0;

    auto markRoute = [&]()
    {
        for(int i = 0; i <= currentPoint; ++i)
        {
            V2 pos = points[i].m_pos;
            markMap[pos.m_y][pos.m_x] = 1;
        }
    };

    auto clearMarkMap = [&]()
    {
        for(auto &m : markMap)
        {
            for(auto &n : m)
            {
                n = 0;
            }
        }
    };
    auto setMap = [&](V2 pos, char c)
    {
        map[pos.m_y][pos.m_x] = c;
    };

    int mapFoundCount = 0;

    auto calculateScore = [&]() -> int64_t
    {
        int64_t currentScore = 0;
        int currentDir = 0;
        for(int i = 1; i <= currentPoint; ++i)
        {
            int dir = getDirFrom(points[i].m_pos, points[i - 1].m_pos);
            currentScore += dir == currentDir ? 1 : 1001;
            currentDir = dir;
        }
        return currentScore;

    };

    auto move = [&]()
    {
        V2 pos = points[currentPoint].m_pos;
        if(points[currentPoint].m_dir < 4)
        {
            int dir = points[currentPoint].m_dir;
            V2 newPos = pos + getDir(dir);
            if(!isValid(newPos, map))
            {
                ++points[currentPoint].m_dir;
                return;
            }

            currentPoint++;
            points[currentPoint] = Point{ newPos, 0 };
            int64_t currScore = calculateScore();
            int64_t &mapScore = scoreMap[newPos.m_y][newPos.m_x];
            if(currScore <= mapScore + 1000 && currScore <= result) // && currScore <= 85420)
            {

                mapScore = std::min(currScore, mapScore);
                setMap(pos, '*');
            }
            else
            {
                --currentPoint;
                ++points[currentPoint].m_dir;
                return;
            }

        }

    };
    auto backup = [&]()
    {
        V2 pos = points[currentPoint].m_pos;
        setMap(pos, s_map[pos.m_y][pos.m_x]);
        --currentPoint;
        currentPoint = std::max(currentPoint, 0);
        points[currentPoint].m_dir++;
    };

    int moveCount = 0;
    auto drawMap = [&]()
    {

        printf("%i\n", moveCount);
        for(const auto& s : map)
        {
            printf("%s\n", s.c_str());
        }
        printf("\n");
    };
    while(points[0].m_dir < 4)
    {
        //drawMap();
        ++moveCount;

        Point& newP = points[currentPoint];
        if(newP.m_dir >= 4)
        {
            backup();
            continue;
        }

        if(s_map[newP.m_pos.m_y][newP.m_pos.m_x] == 'E')
        {
            int64_t currentScore = calculateScore();
            if(currentScore < result)
            {
                mapFoundCount = 1;
                clearMarkMap();
                markRoute();
            }
            else if(currentScore == result)
            {
                ++mapFoundCount;
                markRoute();
            }
            result = std::min(result, currentScore);
            backup();

            continue;
        }

        move();
    }
    int markCount = 0;
    for(const auto& m : markMap)
    {
        for(char c : m)
        {
            if(c == 1)
            {
                markCount++;
            }
        }
    }
    printf("found %i routes\n", mapFoundCount);
    printf("Marks %i\n", markCount);

    return result;
}

static void a()
{
    std::vector<std::string> map = s_map;
    int64_t count = 0;
    count = findPath(map);
    printf("16-a Smallest score %" SDL_PRIs64 "\n", count);
}


static void b()
{
    int64_t count = 0;


    printf("16-b Box gps positions %" SDL_PRIs64 "\n", count);
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
