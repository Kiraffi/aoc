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

static const std::string s_Filename = "input/08.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static std::vector<std::string> s_map;

static int s_mapWidth = 0;
static int s_mapHeight = 0;

static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 08";
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

struct Position2D
{
    int x;
    int y;
};

template<typename T>
static void drawAntinodes(T&& func)
{
    std::unordered_map<char, std::vector<Position2D>> characters;

    for(int y = 0; y < s_mapHeight; ++y)
    {
        for(int x = 0; x < s_mapWidth; ++x)
        {
            if(s_map[y][x] != '.')
            {
                characters[s_map[y][x]].push_back({x, y});
            }
        }
    }

    for(const auto& charPair : characters)
    {
        char c = charPair.first;
        const auto& chars = charPair.second;
        for(int j = 0; j < chars.size(); ++j)
        {
            for(int i = j + 1; i < chars.size(); ++i)
            {
                int xDiff = chars[i].x - chars[j].x;
                int yDiff = chars[i].y - chars[j].y;

                func(chars[i].x, chars[i].y, xDiff, yDiff);
                func(chars[j].x, chars[j].y, -xDiff, -yDiff);
            }
        }
    }
}

int64_t calculateAntinodes(const std::vector<std::string>& antinodes)
{
    int64_t count = 0;
    printf("\n");
    for(const auto row : antinodes)
    {
        for(const auto c : row)
        {
            if(c == '#')
            {
                ++count;1064
            }
        }
        printf("%s\n", row.c_str());
    }
    return count;
}

static void a()
{
    int64_t count = 0;
    std::vector<std::string> antinodes(s_mapHeight, std::string(s_mapWidth, '.'));

    drawAntinodes([&antinodes](int x, int y, int dx, int dy)
    {
        x += dx;
        y += dy;
        if(x >= 0 && x < s_mapWidth && y >= 0 && y < s_mapHeight)
        {
            antinodes[y][x] = '#';
        }
    });
    count = calculateAntinodes(antinodes);

    printf("08-a Unique antinodes %" SDL_PRIs64 "\n", count);
}

static void b()
{
    int64_t count = 0;
    std::vector<std::string> antinodes(s_mapHeight, std::string(s_mapWidth, '.'));

    drawAntinodes([&antinodes](int x, int y, int dx, int dy)
    {
        while(x >= 0 && x < s_mapWidth && y >= 0 && y < s_mapHeight)
        {
            antinodes[y][x] = '#';
            x += dx;
            y += dy;
        }
    });
    count = calculateAntinodes(antinodes);

    printf("08-b Unique harmonic antinodes %" SDL_PRIs64 "\n", count);
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
