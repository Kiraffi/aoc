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

static const std::string s_Filename = "input/11.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::vector<uint64_t> s_numbers;


static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 11";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    while(std::getline(iss, line))
    {
        uint64_t numb;
        std::istringstream iss2(line);
        while(iss2 >> numb)
        {
            s_numbers.push_back(numb);
        }
    }
}

int countNumbers(uint64_t n)
{
    int result = 0;
    while(n)
    {
        ++result;
        n /= 10;
    }
    return result;
}

uint64_t getLeftNum(uint64_t n)
{
    int count = 0;
    uint64_t c = n;
    while(c)
    {
        ++count;
        c /= 10;
    }
    c = n;
    for(int i = 0; i < count / 2; ++i)
    {
        c /= 10;
    }

    return c;
}

uint64_t getRightNum(uint64_t n)
{
    int count = 0;
    uint64_t c = n;
    while(c)
    {
        ++count;
        c /= 10;
    }
    c = 1;
    for(int i = 0; i < count / 2; ++i)
    {
        c *= 10;
    }

    return n % c;
}

uint64_t simulateStone(uint64_t n, int amount)
{
    std::vector<uint64_t> numbers;
    std::vector<uint64_t> numbers2;
    numbers.push_back(n);

    int64_t count = 0;
    for(int i = 0; i < amount; ++i)
    {
        auto& numbersIn = (i % 2) == 0 ? numbers : numbers2;
        auto& numbersOut = (i % 2) == 0 ? numbers2 : numbers;
        numbersOut.clear();

        for(uint64_t n : numbersIn)
        {
            if(n == 0)
            {
                numbersOut.push_back(1);
            }
            else if((countNumbers(n) % 2) == 0)
            {
                numbersOut.push_back(getLeftNum(n));
                numbersOut.push_back(getRightNum(n));
            }
            else
            {
                numbersOut.push_back(n * 2024);
            }
        }
    }
    auto& numbersIn = (amount % 2) == 0 ? numbers : numbers2;
    return numbersIn.size();
}

static void a()
{
    int64_t count = 0;

    for(uint64_t n : s_numbers)
    {
        count += simulateStone(n, 75);
    }
/*
    for(int i = 0; i < maxBlinks; ++i)
    {
        auto& numbersIn = (i % 2) == 0 ? numbers : numbers2;
        auto& numbersOut = (i % 2) == 0 ? numbers2 : numbers;
        numbersOut.clear();

        for(uint64_t n : numbersIn)
        {
            if(n == 0)
            {
                numbersOut.push_back(1);
            }
            else if((countNumbers(n) % 2) == 0)
            {
                numbersOut.push_back(getLeftNum(n));
                numbersOut.push_back(getRightNum(n));
            }
            else
            {
                numbersOut.push_back(n * 2024);
            }
        }
    }
    auto& numbersIn = (maxBlinks % 2) == 0 ? numbers : numbers2;

    for(uint64_t n : numbersIn)
    {
        printf("%" SDL_PRIs64 ", ", n);
    }
    printf("\n");
    */
    printf("11-a Stones %" SDL_PRIs64 "\n", count);
}

static void b()
{
    int64_t count = 0;
    printf("11-b ratings of trailheads %" SDL_PRIs64 "\n", count);
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
