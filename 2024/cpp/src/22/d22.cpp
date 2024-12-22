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

static const std::string s_Filename = "input/22.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::vector<int> s_numbers;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 22";
}

static void parse()
{
    int row = 0;
    std::string line;
    std::istringstream iss(s_input);
    while(std::getline(iss, line))
    {
        std::istringstream iss2(line);
        int tmp;
        iss2 >> tmp;
        s_numbers.push_back(tmp);
    }
}

static int64_t prune(int64_t num)
{
    return num % 16777216;
}

static int64_t mix(int64_t a, int64_t b)
{
    return a ^ b;
}

static int64_t step(int64_t num)
{
    num = mix(num, num * 64);
    num = prune(num);

    num = mix(num, num / 32);
    //int64_t t = num;
    num = prune(num); // doesnt affect number
    //assert(num == t);

    num = mix(num, num * 2048);
    num = prune(num); // we can remove this if we prune the result

    return num;
}

static void a()
{
    int64_t result = 0;

    for(int64_t num : s_numbers)
    {
        for(int i = 0; i < 2000; ++i)
        {
            num = step(num);
        }
        result += num;
    }
    printf("21-a 2000th secret numbers %" SDL_PRIs64 "\n", result);
}

static void b()
{
    int64_t result = 0;

    struct BestBanana
    {
        union
        {
            int8_t prizes[4];
            int32_t p32;
        };

    };

    std::unordered_map<int32_t, int64_t> bananaCount;

    for(int j = 0; j < s_numbers.size(); ++j)
    {
        // dont add multiple times.
        std::unordered_set<int32_t> seen;

        int64_t num = s_numbers[j];

        BestBanana currBanana = {};
        int64_t prev = num % 10;

        for(int i = 0; i < 2000; ++i)
        {
            num = step(num);
            int64_t t = num % 10;
            currBanana.p32 <<= 8;
            currBanana.prizes[0] = t - prev;
            // calculate at least the first 4.
            if(i > 4)
            {
                if(!seen.contains(currBanana.p32))
                {
                    bananaCount[currBanana.p32] += t;
                }
                seen.insert(currBanana.p32);
            }
            prev = t;
        }

    }
    for(auto iter : bananaCount)
    {
        result = std::max(result, iter.second);
    }

    printf("22-b Bananas %" SDL_PRIs64 "\n", result);
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
