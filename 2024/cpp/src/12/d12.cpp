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

#include "d12_comp.h"

#include "commons.h"
#include "commonrender.h"

static const int ValuesBufferSize = 1024 * 8;

enum BufferEnum : int
{
    BufferInput,
    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineD12,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(d12_comp), 2, 1, 1024 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);


static const std::string s_Filename = "input/12.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::vector<std::string> s_map;
static int s_mapWidth;
static int s_mapHeight;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 12";
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

static int calculateArea(int x, int y, char c, std::vector<std::vector<int>>& visited)
{
    if(x < 0 || y < 0 || x >= s_mapWidth || y >= s_mapHeight)
    {
        return 0;
    }
    if(visited[y][x] != 0)
    {
        return 0;
    }
    if(s_map[y][x] != c)
    {
        return 0;
    }
    visited[y][x] = 1;
    int area = 1;

    area += calculateArea(x - 1, y + 0, c, visited);
    area += calculateArea(x + 1, y + 0, c, visited);
    area += calculateArea(x + 0, y + 1, c, visited);
    area += calculateArea(x + 0, y - 1, c, visited);

    return area;
}

static bool isChar(int x, int y, char c)
{
    if(x < 0 || y < 0 || x >= s_mapWidth || y >= s_mapHeight)
    {
        return false;
    }
    return (s_map[y][x] == c);
}

void addCorner(int x1, int y1, int x2, int y2, std::unordered_set<int>& corners)
{
    char c = s_map[y1][x1];
    if(isChar(x2, y2, c))
    {
        return;
    }
    if(x1 != x2)
    {
        x1 = std::min(x1, x2);
        y1 = std::min(y1, y2);
        corners.insert((x1 + 1) + ((y1 + 0) << 16));
        corners.insert((x1 + 1) + ((y1 + 1) << 16));
    }
    else
    {
        x1 = std::min(x1, x2);
        y1 = std::min(y1, y2);
        corners.insert((x1 + 0) + ((y1 + 1) << 16));
        corners.insert((x1 + 1) + ((y1 + 1) << 16));
    }
}

static int64_t calculatePerimeter(
    int x,
    int y,
    char c,
    std::vector<std::vector<int>>& visited,
    std::unordered_set<int>& corners)
{
    if(x < 0 || y < 0 || x >= s_mapWidth || y >= s_mapHeight)
    {
        return 0;
    }
    if(visited[y][x] == 2)
    {
        return 0;
    }
    if(s_map[y][x] != c)
    {
        return 0;
    }
    visited[y][x] = 2;

/*
doesnt work on shapes like
.XX
X.X
XXX
It will count the first row second char corner same as
second row first character corner, even though they should be

    addCorner(x, y, x + 1, y, corners);
    addCorner(x, y, x - 1, y, corners);
    addCorner(x, y, x, y - 1, corners);
    addCorner(x, y, x, y + 1, corners);
*/

    int64_t result = 0;

    if(!isChar(x + 1, y + 0, c)) ++result;
    if(!isChar(x - 1, y + 0, c)) ++result;
    if(!isChar(x + 0, y + 1, c)) ++result;
    if(!isChar(x + 0, y - 1, c)) ++result;

    result += calculatePerimeter(x - 1, y + 0, c, visited, corners);
    result += calculatePerimeter(x + 1, y + 0, c, visited, corners);
    result += calculatePerimeter(x + 0, y + 1, c, visited, corners);
    result += calculatePerimeter(x + 0, y - 1, c, visited, corners);

    return result;
}

static void drawPerimeter(const std::unordered_set<int>& corners)
{
    for(int j = 0; j < s_mapHeight + 1; ++j)
    {
        for(int i = 0; i < s_mapWidth + 1; ++i)
        {
            if(corners.contains(i + (j << 16)))
            {
                printf("#");
            }
            else
            {
                printf(".");
            }
        }
        printf("\n");
    }
}

static void a()
{
    int64_t count = 0;
    std::vector<std::vector<int>> visited(s_mapHeight, std::vector<int>(s_mapWidth, 0));
    std::unordered_map<char, uint64_t> costs;
    std::unordered_set<int> corners;

    for(int y = 0; y < s_mapHeight; ++y)
    {
        for(int x = 0; x < s_mapWidth; ++x)
        {
            if(visited[y][x] == 0)
            {
                char c = s_map[y][x];
                uint64_t area = calculateArea(x, y, c, visited);
                corners.clear();
                uint64_t perimeter = calculatePerimeter(x, y, c, visited, corners);
                uint64_t cost = area * perimeter;
                count += cost;
/*
                printf("[%i, %i], %c: area = %" SDL_PRIs64 ", perimeter: %" SDL_PRIs64 " = %" SDL_PRIs64 "\n",
                    x, y, c, area, perimeter, cost);
*/
                //drawPerimeter(corners);
            }
        }
    }
    printf("12-a Cost %" SDL_PRIs64 "\n", count);
}


static void markRegion(int x, int y, char c, int index, std::vector<std::vector<int>>& visited)
{
    if(x < 0 || y < 0 || x >= s_mapWidth || y >= s_mapHeight)
    {
        return;
    }
    if(visited[y][x] != -1)
    {
        return;
    }
    if(s_map[y][x] != c)
    {
        return;
    }
    visited[y][x] = index;
    markRegion(x + 1, y + 0, c, index, visited);
    markRegion(x - 1, y + 0, c, index, visited);
    markRegion(x + 0, y - 1, c, index, visited);
    markRegion(x + 0, y + 1, c, index, visited);
}

int64_t calculateMarkedArea(int regionIndex, const std::vector<std::vector<int>>& visited)
{
    int64_t result = 0;
    for(int y = 0; y < s_mapHeight; ++y)
    {
        for(int x = 0; x < s_mapWidth; ++x)
        {
            if(visited[y][x] == regionIndex)
            {
                ++result;
            }
        }
    }
    return result;
}

int64_t countEdges(int x, int y, int regionIndex, const std::vector<std::vector<int>>& visited)
{
    int64_t result = 0;
    for(int yy = 0; yy < s_mapHeight; ++yy)
    {
        bool topOn = false;
        bool botOn = false;
        for(int xx = 0; xx < s_mapWidth; ++xx)
        {
            if(visited[yy][xx] != regionIndex)
            {
                if(topOn)
                    result++;
                if(botOn)
                    result++;
                topOn = botOn = false;
                continue;
            }
            else if(yy == 0)
            {
                topOn = true;
            }
            else if(yy == s_mapHeight - 1)
            {
                botOn = true;
            }
            if(yy > 0)
            {
                if((visited[yy - 1][xx] != regionIndex) != topOn)
                {
                    if(topOn)
                    {
                        result++;
                    }
                    topOn = !topOn;
                }
            }

            if(yy < s_mapHeight - 1)
            {
                if((visited[yy + 1][xx] != regionIndex) != botOn)
                {
                    if(botOn)
                    {
                        result++;
                    }
                    botOn = !botOn;
                }
            }
        }

        if(topOn)
            result++;
        if(botOn)
            result++;
    }

    for(int xx = 0; xx < s_mapWidth; ++xx)
    {
        bool leftOn = false;
        bool rightOn = false;
        for(int yy = 0; yy < s_mapHeight; ++yy)
        {
            if(visited[yy][xx] != regionIndex)
            {
                if(leftOn)
                    result++;
                if(rightOn)
                    result++;
                leftOn = rightOn = false;
                continue;
            }
            else if(xx == 0)
            {
                leftOn = true;
            }
            else if(xx == s_mapWidth - 1)
            {
                rightOn = true;
            }
            if(xx > 0)
            {
                if((visited[yy][xx - 1] != regionIndex) != leftOn)
                {
                    if(leftOn)
                    {
                        result++;
                    }
                    leftOn = !leftOn;
                }
            }

            if(xx < s_mapWidth - 1)
            {
                if((visited[yy][xx + 1] != regionIndex) != rightOn)
                {
                    if(rightOn)
                    {
                        result++;
                    }
                    rightOn = !rightOn;
                }
            }
        }

        if(leftOn)
            result++;
        if(rightOn)
            result++;
    }

    return result;

}

static void b()
{
    int64_t count = 0;
    std::vector<std::vector<int>> visited(s_mapHeight, std::vector<int>(s_mapWidth, -1));
    int64_t sumOfArea = 0;
    int regionIndex = 0;
    for(int y = 0; y < s_mapHeight; ++y)
    {
        for(int x = 0; x < s_mapWidth; ++x)
        {
            if(visited[y][x] == -1)
            {
                ++regionIndex;
                char c = s_map[y][x];
                // marking visiteds.
                markRegion(x, y, c, regionIndex, visited);
                int edges = countEdges(x, y, regionIndex, visited);
                int area = calculateMarkedArea(regionIndex, visited);
                //printf("b: edges: %i\n", edges);

                count += edges * area;
            }
        }
    }
    printf("12-b edges %" SDL_PRIs64 "\n", count);
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
    for(int i = 0; i < computeDebugNumbers; i += 4)
    {
        int value0 = s_dataBuffer[i + 4];
        int value1 = s_dataBuffer[i + 5];
        int value2 = s_dataBuffer[i + 6];
        int value3 = s_dataBuffer[i + 7];
        printf("%i, %i, %i, %i\n", value0, value1, value2, value3);
    }

    printf("03-a compute safe: %i\n", s_dataBuffer[0]);
    printf("03-b compute safe: %i\n", s_dataBuffer[1]);
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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD12]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }

#endif
    return true;
}
