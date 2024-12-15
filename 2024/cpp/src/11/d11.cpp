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

static const int ValuesBufferSize = 1024;

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


static int s_dataBuffer[ValuesBufferSize] = {};

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
        count += simulateStone(n, 25);
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

std::unordered_map<uint64_t, uint64_t> s_memoization;

uint64_t recurse(uint64_t value, int iterationsLeft)
{
    if(iterationsLeft == 0)
    {
        return 1;
    }
    uint64_t t = (uint64_t(value) << uint64_t(8)) + iterationsLeft;
    auto f = s_memoization.find(t);
    if(f != s_memoization.end())
    {
        return f->second;
    }
    uint64_t result = 0;
    if(value == 0)
    {
        result = recurse(1, iterationsLeft - 1);
    }
    else if((countNumbers(value) % 2) == 0)
    {
        result = recurse(getLeftNum(value), iterationsLeft - 1);
        result += recurse(getRightNum(value), iterationsLeft - 1);
    }
    else
    {
        result = recurse(value * 2024, iterationsLeft - 1);
    }
    s_memoization[t] = result;
    return result;
}

static void b()
{
    /*
    std::vector<std::vector<std::vector<uint64_t>>> stones;
    for(int i = 0; i < 10; ++i)
    {
        stones.push_back({});
        stones[i].push_back({});
        stones[i][0].push_back(i);
        stones[i].push_back({});
        stones[i][1].push_back(i > 0 ? i * 2024 : 1);
        int j = 1;
        if(i == 0)
        {
            j = 2;
            stones[i].push_back({});
            stones[i][2].push_back(2024);
        }
        while(true)
        {
            bool allUnderTen = true;
            for(int k = 0; k < stones[i][j].size(); ++k)
            {
                int n = stones[i][j][k];
                if(n >= 10)
                {
                    allUnderTen = false;
                    if(stones[i].size() < j + 2)
                    {
                        stones[i].push_back({});
                    }

                    if(n == 0)
                    {
                        stones[i][j+1].push_back(1);
                    }
                    else if((countNumbers(n) % 2) == 0)
                    {
                        stones[i][j + 1].push_back(getLeftNum(n));
                        stones[i][j + 1].push_back(getRightNum(n));
                    }
                    else
                    {
                        stones[i][j+1].push_back(n * 2024);
                    }
                }
            }
            if(allUnderTen)
            {
                break;
            }
            ++j;
        }

    }
    struct NumIter
    {
        uint64_t m_num;
        int m_iterations;
    };
    std::vector<NumIter> numsLeft;

    static const int ITERATIONS = 75;

    int64_t count = 0;
    for(uint64_t n : s_numbers)
    {
        numsLeft.push_back(NumIter{n, 0});
        bool done = false;
        while(!numsLeft.empty())
        {
            NumIter numIter = numsLeft.back();
            numsLeft.pop_back();
            if(numIter.m_iterations == 75)
            {
                count += 1;
                continue;
            }
            if(numIter.m_num < 10)
            {
                if(ITERATIONS - numIter.m_iterations < stones[numIter.m_num].size())
                {
                    count += stones[numIter.m_num][ITERATIONS - numIter.m_iterations].size();
                    continue;
                }
                for(auto v : stones[numIter.m_num].back())
                {
                    numsLeft.push_back(NumIter{v, numIter.m_iterations + 1});
                }
                continue;
            }

            if((countNumbers(numIter.m_num) % 2) == 0)
            {
                numsLeft.push_back(NumIter{getLeftNum(numIter.m_num), numIter.m_iterations + 1});
                numsLeft.push_back(NumIter{getRightNum(numIter.m_num), numIter.m_iterations + 1});
            }
            else
            {
                numsLeft.push_back(NumIter{numIter.m_num * 2024, numIter.m_iterations + 1});
            }

        }


        //count += simulateStone(n, 25);
    }
    */
    uint64_t count = 0;
    for(uint64_t n : s_numbers)
    {
        count += recurse(n, 75);

    }
    printf("11-b stones %" SDL_PRIs64 "\n", count);
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
