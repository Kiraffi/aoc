#include <algorithm>
#include <cassert>
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


static const std::string s_Filename = "input/05.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);


static std::unordered_map<int, std::vector<int>> s_map;
static std::vector<std::vector<int>> s_rows;

static int s_dataBuffer[1024] = {};

const char* getTitle()
{
    return "AOC 2024 day 05";
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    while(std::getline(iss, line))
    {
        if(line.empty())
        {
            break;
        }
        std::istringstream iss2(line);
        int v0;
        iss2 >> v0;
        char tmp;
        iss2 >> tmp;
        int v1;
        iss2 >> v1;
        s_map[v1].push_back(v0);
    }
    while(std::getline(iss, line))
    {
        std::istringstream iss2(line);
        int v;
        s_rows.push_back({});
        while(iss2 >> v)
        {
            s_rows.back().push_back(v);
            char c;
            iss2 >> c;
        }
    }
}

void setBit(uint64_t& value, int bit)
{
    value |= uint64_t(1) << uint64_t(bit % 64);
}
bool isSet(uint64_t value, int bit)
{
    return ((value >> uint64_t(bit % 64)) & 1) == 1;
}

bool containsValue(int mapIndex, int value)
{
    const auto& v = s_map[mapIndex];
    return std::find(v.begin(), v.end(), value) != v.end();
}

static void a()
{
    int64_t count = 0;
    std::unordered_set<int> visited;
    for(const auto& row : s_rows )
    {
        bool valid = true;
        for(int i = 1; i < row.size(); ++i)
        {
            visited.clear();
            if(!containsValue(row[i], row[i - 1]))
            {
                valid = false;
                break;
            }
        }
        if(valid)
        {
            assert(row.size() % 2);
            count += row[row.size() / 2];
        }
    }
    printf("05-a Sum of valid middle numbers %i times.\n", int(count));
}

static void b()
{
    int64_t count = 0;

    for(auto& row : s_rows )
    {
        for(int i = 1; i < row.size(); ++i)
        {
            if(!containsValue(row[i], row[i - 1]))
            {
                std::sort(row.begin(), row.end(), [](int& a, int& b)
                {
                    return containsValue(a, b);
                });
                assert(row.size() % 2);
                count += row[row.size() / 2];
                break;
            }
        }
    }
    printf("05-b Sum of invalid sorted middle numbers %i times.\n", int(count));
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
        s_pipelines[PipelineD02] = createComputePipeline(d02_comp, sizeof(d02_comp), 256, 2, 1);
    }
#endif
    return true;
}

bool initData()
{
    doCpu();
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
