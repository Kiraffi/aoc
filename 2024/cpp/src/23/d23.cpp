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

static const std::string s_Filename = "input/23.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::unordered_map<uint16_t, std::unordered_set<uint16_t>> s_pairMap;
static std::unordered_set<uint16_t> s_computer;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 23";
}

static void parse()
{
    int row = 0;
    std::string line;
    std::istringstream iss(s_input);
    while(std::getline(iss, line))
    {
        std::istringstream iss2(line);
        std::string l = line.substr(0, 2);
        std::string r = line.substr(3);
        uint16_t lv = uint16_t(l[0]) | (uint16_t(l[1] << 8));
        uint16_t rv = uint16_t(r[0]) | (uint16_t(r[1] << 8));
        s_pairMap[lv].insert(rv);
        s_pairMap[rv].insert(lv);
        s_computer.insert(lv);
        s_computer.insert(rv);
        /*
        if(!s_pairMap[l].contains(r) && !s_pairMap[r].contains(l))
        {
            printf("%s - %s\n", l.c_str(), r.c_str());
        }
        */
    }
}

static std::string getValueAsString(uint16_t value)
{
    std::string result;
    result += char(value & 0xff);
    result += char(value >> 8);
    return result;
}


static void a()
{
    int64_t result = 0;

    std::unordered_set<uint16_t> seen;
    std::vector<std::unordered_set<uint16_t>> combos;

    for(const auto& s : s_computer)
    {
        if(seen.contains(s))
        {
            continue;
        }
        seen.insert(s);
        const auto& v1Map = s_pairMap[s];
        for(uint16_t ss : v1Map)
        {
            if(seen.contains(ss))
            {
                continue;
            }
            const auto& v2Map = s_pairMap[ss];
            for(const auto& sss : s_computer)
            {
                if(seen.contains(sss))
                {
                    continue;
                }
                if(v1Map.contains(sss) && v2Map.contains(sss))
                {
                    combos.push_back({s, ss, sss});
                }
            }
        }
    }

    for(const auto& v : combos)
    {
        std::string s[3];
        int index = 0;
        int amount = 0;
        for(uint16_t value : v)
        {
            s[index] = getValueAsString(value);
            if((value & 0xff) == 't')
            {
                ++amount;
            }
            ++index;
        }
        if(amount)
        {
            //printf("%s, %s, %s\n", s[0].c_str(), s[1].c_str(), s[2].c_str());
            result++;
        }
    }
    // a, b, c and a, c, b
    result /= 2;

    printf("22-a Three computer groups having computer starting with 't' %" SDL_PRIs64 "\n", result);
}

static void b()
{
    //int64_t result = 0;

    std::vector<std::unordered_set<uint16_t>> combos;


    for(const auto& v : s_pairMap)
    {
        const auto& value = v.first;

        auto& vMap = v.second;
        bool found = false;
        for(auto& c : combos)
        {
            bool allContains = true;
            for(uint16_t comboValue : c)
            {

                if(!vMap.contains(comboValue))
                {
                    allContains = false;
                    break;
                }
            }
            if(allContains)
            {
                found = true;
                c.insert(value);
                break;
            }
        }
        if(!found)
        {
            combos.push_back({value});
        }
    }

    size_t highestAmount = 0;
    std::unordered_set<std::string> best;

    for(const auto& hset : combos)
    {
        if(hset.size() >= highestAmount)
        {
            best.clear();
            for(uint16_t value : hset)
            {
                std::string s = getValueAsString(value);
                //printf("%s, ", s.c_str());
                best.insert(s);
            }
            //printf("\n");
            highestAmount = hset.size();
        }
    }

    std::vector<std::string> bestStr(best.begin(), best.end());
    std::sort(bestStr.begin(), bestStr.end());
    std::string s;
    for(const auto& ss : bestStr)
    {
        s += ss;
        s += ",";
    }
    s = s.substr(0, s.size() - 1);
    printf("23-b: Password: %s\n", s.c_str());
    //printf("23-b  %" SDL_PRIs64 "\n", result);
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
