#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
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


static const std::string s_Filename = "input/03.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::vector<char> s_input;


static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 03";
}

static std::vector<char> readInputFile()
{
    std::vector<char> result;
    std::ifstream file(s_Filename, std::ios::binary);
    if(!file.is_open())
    {
        printf("Failed to open file\n");
        return result;
    }
    result.assign(std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
    return result;
}


static void d03a()
{
    std::vector<char> input = readInputFile();
    std::string s = input.data();
    int64_t sums = 0;
    int startIndex = 0;
    while(startIndex < s.length())
    {
        int pos = s.find("mul(", startIndex);
        if(pos < s.length() && pos >= 0)
        {
            startIndex = pos + 1;
            pos += 4;
            int num1 = 0;
            int num2 = 0;
            bool valid = false;
            while(isdigit(s[pos]))
            {
                num1 = num1 * 10 + (s[pos] - '0');
                valid = true;
                pos++;
            }
            if(!valid)
            {
                continue;
            }
            if(s[pos] != ',')
            {
                continue;
            }
            ++pos;
            valid = false;
            while(isdigit(s[pos]))
            {
                num2 = num2 * 10 + (s[pos] - '0');
                valid = true;
                pos++;
            }
            if(!valid)
            {
                continue;
            }
            if(s[pos] != ')')
            {
                continue;
            }
            sums += num1 * num2;
        }
        else
        {
            break;
        }
    }

    printf("03-a sum of muls: %i\n", int(sums));
}

static void d03b()
{
    std::vector<char> input = readInputFile();
    std::string s = input.data();
    int64_t sums = 0;

    bool enabled = true;

    int startIndex = 0;
    int nextDont = s.find("don't()", startIndex);
    while(startIndex < s.length())
    {
        if(!enabled)
        {
            int nextDo = s.find("do()", startIndex);
            if(nextDo >= 0 && nextDo < s.length())
            {
                startIndex = nextDo + 1;
                enabled = true;
                continue;
            }
            break;
        }
        if(nextDont < startIndex)
        {
            nextDont = s.find("don't()", startIndex);
        }
        int pos = s.find("mul(", startIndex);

        if(enabled && nextDont >= 0 && nextDont < pos)
        {
            enabled = false;
            startIndex = nextDont + 1;
            continue;
        }

        if(pos < s.length() && pos >= 0)
        {
            startIndex = pos + 1;
            pos += 4;
            int num1 = 0;
            int num2 = 0;
            bool valid = false;
            while(isdigit(s[pos]))
            {
                num1 = num1 * 10 + (s[pos] - '0');
                valid = true;
                pos++;
            }
            if(!valid)
            {
                continue;
            }
            if(s[pos] != ',')
            {
                continue;
            }
            ++pos;
            valid = false;
            while(isdigit(s[pos]))
            {
                num2 = num2 * 10 + (s[pos] - '0');
                valid = true;
                pos++;
            }
            if(!valid)
            {
                continue;
            }
            if(s[pos] != ')')
            {
                continue;
            }
            sums += num1 * num2;
        }
        else
        {
            break;
        }
    }

    printf("03-b sum of muls: %i\n", int(sums));
}

void d03()
{
    d03a();
    d03b();
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
        s_pipelines[PipelineD02] = createComputePipeline(d02_comp, sizeof(d02_comp), 256, 2, 1);
    }
#endif
    return true;
}

bool initData()
{
    d03();
#if 0

    return initCompute();
#else
    return true;
#endif
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

    //if(index > 100)
    //    return false;
#endif
    return true;
}
