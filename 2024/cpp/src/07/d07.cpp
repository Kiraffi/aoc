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

static const std::string s_Filename = "input/07.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static std::vector<std::vector<int64_t>> s_numbers;

static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 07";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    while(std::getline(iss, line))
    {
        std::istringstream iss2(line);

        std::vector<int64_t> numbers;
        int64_t num;
        iss2 >> num;
        numbers.push_back(num);
        char tmp;
        iss2 >> tmp;
        while(iss2 >> num)
        {
            numbers.push_back(num);
        }
        if(numbers.size() > 0)
        {
            s_numbers.push_back(numbers);
        }
    }
}

static bool testNumbersA(const std::vector<int64_t>& numbers)
{
    // binary, 0 means add, 1 means mul
    uint64_t maxCases = (uint64_t(1) << uint64_t(numbers.size() - 2));
    for(uint64_t testCases = 0; testCases < maxCases; ++testCases)
    {
        uint64_t sum = numbers[1];
        uint64_t localCases = testCases;
        for(uint64_t index = 2; index < numbers.size(); ++index)
        {
            if(sum > numbers[0])
            {
                break;
            }
            sum = (localCases & 1)
                ? sum * numbers[index]
                : sum + numbers[index];
            localCases >>= 1;
        }

        if(sum == numbers[0])
        {
            /*
            localCases = testCases;
            printf("%" SDL_PRIu64, numbers[1]);
            for(uint64_t index = 2; index < numbers.size(); ++index)
            {
                char c = (localCases & 1)
                    ? '*'
                    : '+';
                localCases >>= 1;
                printf(" %c %" SDL_PRIu64, c, numbers[index]);
            }

            printf(" = %" SDL_PRIu64 "\n", numbers[0]);
            */
            return true;
        }
    }
    return false;
}

static uint64_t concatNumbers(uint64_t l, uint64_t r)
{
    uint64_t mult = 1;
    while(r / mult > 0)
    {
        mult *= 10;
    }
    uint64_t result = l * mult + r;
    return result;
}

static bool testNumbersB(const std::vector<int64_t>& numbers)
{
    // 3 cases, 0 means add, 1 means mul, 2 means concat
    uint64_t maxCases = std::pow(3, uint64_t(numbers.size() - 2));
    for(uint64_t testCases = 0; testCases < maxCases; ++testCases)
    {
        uint64_t sum = numbers[1];
        uint64_t localCases = testCases;
        for(uint64_t index = 2; index < numbers.size(); ++index)
        {
            if(sum > numbers[0])
            {
                break;
            }

            switch(localCases % 3)
            {
                case 0: sum = sum + numbers[index]; break;
                case 1: sum = sum * numbers[index]; break;
                case 2: sum = concatNumbers(sum, numbers[index]); break;
            }
            localCases /= 3;
        }

        if(sum == numbers[0])
        {
            /*
            localCases = testCases;
            printf("%" SDL_PRIu64, numbers[1]);
            for(uint64_t index = 2; index < numbers.size(); ++index)
            {
                char c;
                switch(localCases % 3)
                {
                    case 0: c = '+'; break;
                    case 1: c = '*'; break;
                    case 2: c = '|'; break;
                }
                localCases /= 3;
                printf(" %c %" SDL_PRIu64, c, numbers[index]);
            }

            printf(" = %" SDL_PRIu64 "\n", numbers[0]);
            */
            return true;
        }
    }
    return false;
}


static void a()
{
    int64_t sum = 0;
    for(const auto& numbers : s_numbers)
    {

        if(testNumbersA(numbers))
        {
            sum += numbers[0];
        }
    }

    printf("07-a Sum of valid operator numbers %" SDL_PRIs64 "\n", sum);
}

static void b()
{
    int64_t sum = 0;
    for(const auto& numbers : s_numbers)
    {
        if(testNumbersB(numbers))
        {
            sum += numbers[0];
        }
    }

    printf("07-b Sum of valid operator numbers %" SDL_PRIs64 "\n", sum);
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
    s_input = readInputFile();

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
