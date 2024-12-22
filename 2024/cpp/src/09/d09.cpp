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

#include "d09_comp.h"

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
    PipelineD09,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(d09_comp), 2, 1, 1024 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);

static const std::string s_Filename = "input/09.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);




static int s_mapWidth = 0;
static int s_mapHeight = 0;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 09";
}

static void parse()
{
}


static void a()
{
    std::vector<int> numbers;


    int64_t count = 0;


    int currentIndex = 0;
    for(auto numb : s_input)
    {
        int value = numb - '0';
        if((currentIndex % 2) == 0)
        {
            for(int i = 0; i < value; ++i)
            {
                numbers.push_back(currentIndex / 2);
            }
        }
        else
        {
            for(int i = 0; i < value; ++i)
            {
                numbers.push_back(-1);
            }
        }
        ++currentIndex;
    }

    int index = 0;
    int rIndex = numbers.size() - 1;
    while(index < rIndex)
    {
        if(numbers[index] == -1)
        {
            while(rIndex >= 0 && numbers[rIndex] == -1)
            {
                --rIndex;
            }
            if(rIndex < index)
            {
                break;
            }
            numbers[index] = numbers[rIndex];
            --rIndex;
        }
        ++index;
    }
/*
    numbers.resize(rIndex + 1);
    for(int numb : numbers)
    {
        if(numb != -1)
            printf("%i", numb);
        else
            printf("-");
    }
    printf("\n");
*/

    count = 0;
    index = 0;
    for(index = 0; index < rIndex + 1; index += 1)
    {
        count += index * numbers[index];

    }
    /*
    {
        uint64_t tmpValue = 0;
        int currentIndex = 0;
        uint64_t mulIndex = 0;
        for(auto numb : s_input)
        {
            uint64_t value = numb - '0';
            if((currentIndex % 2) == 0)
            {
                for(int i = 0; i < value; ++i)
                {
                    tmpValue += ((mulIndex + i) * value);
                }
            }
            ++currentIndex;
            mulIndex += value;

        }

        printf("09-a originals %" SDL_PRIs64 "\n", tmpValue);
    }
    */
    printf("09-a Checksum %" SDL_PRIs64 "\n", count);
}

static void b()
{
    int64_t count = 0;
    std::vector<int> numbers;

    struct NumberInfo
    {
        int m_number;
        int m_index;
        int m_amount;
        int m_padding;
    };
    std::vector<NumberInfo> numberInfos;

    int currentIndex = 0;
    for(auto numb : s_input)
    {
        int value = numb - '0';
        if((currentIndex % 2) == 0)
        {
            if(value > 0 )
            {
                numberInfos.push_back(NumberInfo{
                    .m_number = currentIndex / 2,
                    .m_index = (int)numbers.size(),
                    .m_amount = value
                });

            }
            for(int i = 0; i < value; ++i)
            {
                numbers.push_back(currentIndex / 2);
            }
        }
        else
        {
            for(int i = 0; i < value; ++i)
            {
                numbers.push_back(-1);
            }
        }
        ++currentIndex;
    }

    for(int rIndex = numberInfos.size() - 1; rIndex >= 0; --rIndex)
    {
        int freeSpace = 0;
        NumberInfo& info = numberInfos[rIndex];
        for(int i = 0; i < numbers.size(); ++i)
        {
            if(i >= info.m_index)
            {
                break;
            }
            if(numbers[i] == -1)
            {
                ++freeSpace;
            }
            else
            {
                freeSpace = 0;
            }
            if(freeSpace >= info.m_amount)
            {
                while(freeSpace > 0)
                {
                    numbers[i - freeSpace + 1] = info.m_number;
                    numbers[info.m_index + freeSpace - 1] = -1;
                    --freeSpace;
                }
                break;
            }
        }
    }
/*
    printf("B end:\n");
    for(int numb : numbers)
    {
        if(numb != -1)
            printf("%i", numb);
        else
            printf("-");
    }
    printf("\n");
*/
    count = 0;
    for(int index = 0; index < numbers.size(); ++index)
    {
        if(numbers[index] != -1)
        {
            count += index * numbers[index];
        }
    }


    printf("09-b Unique harmonic antinodes %" SDL_PRIs64 "\n", count);
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
    s_buffers[BufferInput] = (createGPUWriteBuffer(((s_input.size() + 15) / 16) * 16, "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(ValuesBufferSize * sizeof(int), "ResultBuffer"));

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

    int64_t a = *(((int64_t*)s_dataBuffer) + 0);
    int64_t b = *(((int64_t*)s_dataBuffer) + 1);

/*
    for(int i = 4; i < ValuesBufferSize; i += 8)
    {
        printf("%i: value: ", i);
        for(int j = 0; j < 8; ++j)
        {
            printf("%i, ", s_dataBuffer[i + j]);
            if((j % 2) != 0)
            {
                printf("  ");
            }
        }
        printf("\n");
    }
*/

    printf("09-a Compute Antinodes %" SDL_PRIs64 "\n", a);
    printf("09-b Compute Harmpnic Antinodes %" SDL_PRIs64 "\n", b);

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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD09]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }
#endif
    return true;
}
