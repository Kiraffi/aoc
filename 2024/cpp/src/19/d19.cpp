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

static const std::string s_Filename = "input/19.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);


static std::unordered_map<char, std::vector<std::string>> s_towels;
static std::vector<std::string> s_lines;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 19";
}

static void parseTowels(const std::string& line)
{
    std::istringstream iss(line);
    std::string tmp;
    while(iss >> tmp)
    {
        tmp = tmp.substr(0, tmp.length() - 1);
        s_towels[tmp[0]].push_back(tmp);
    }
}


static void parse()
{
    std::string line;
    std::istringstream iss(s_input);
    std::getline(iss, line);
    parseTowels(line);
    std::getline(iss, line);

    while(std::getline(iss, line))
    {
        s_lines.push_back(line);
    }
}

bool isValid(const std::string& str, int index)
{
    if(index >= str.length())
    {
        return true;
    }
    int64_t result = 0;
    char c = str[index];
    for(const auto& towel : s_towels[c])
    {
        if((str.length() - index) < towel.length())
        {
            continue;
        }
        if(strncmp(&str[index], towel.c_str(), towel.length()) == 0)
        {
            if(isValid(str, index + towel.length()))
            {
                return true;
            }
        }
    }
    return false;
}

static void a()
{
    int64_t result = 0;
    for(const auto& line : s_lines)
    {
        if(isValid(line, 0))
        {
            ++result;
        }
    }
    printf("19-a Possible designs %" SDL_PRIs64 "\n", result);
}

static std::unordered_map<int, int64_t> s_memoize;

int64_t countValids(const std::string& str, int index)
{
    if(index >= str.length())
    {
        return 1;
    }
    int64_t result = 0;
    auto f = s_memoize.find(index);
    if(f != s_memoize.end())
    {
        result = f->second;
    }
    else
    {
        char c = str[index];
        for(const auto& towel : s_towels[c])
        {
            if((str.length() - index) < towel.length())
            {
                continue;
            }
            if(strncmp(&str[index], towel.c_str(), towel.length()) == 0)
            {
                result += countValids(str, index + towel.length());
            }
        }
    }
    s_memoize[index] = result;
    return result;
}




static void b()
{
    int64_t result = 0;
    for(const auto& line : s_lines)
    {
        result += countValids(line, 0);
        s_memoize.clear();
    }
    printf("19-b Possible designs %" SDL_PRIs64 "\n", result);

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
