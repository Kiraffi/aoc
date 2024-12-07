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


#include "../atomic_buffers_reset_comp.h"
#include "calculate_middle_numbers_d05_comp.h"
#include "parse_d05_comp.h"

#include "commons.h"
#include "commonrender.h"

enum BufferEnum : int
{
    BufferInput,

    BufferParsedGates,
    BufferParsedGatesIndices,

    BufferParsedLines,
    BufferParsedLinesIndices,

    BufferIndirect,
    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineAtomicBufferReset,
    PipelineParse,
    PipelineCalculateMiddleNumbersIndirect,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(atomic_buffers_reset_comp), 1, 0, 1 },
    { BUF_N_SIZE(parse_d05_comp), 6, 1, 1024 },
    //{ BUF_N_SIZE(parse_d05_comp), 6, 1, 1024 },
    { BUF_N_SIZE(calculate_middle_numbers_d05_comp), 5, 0, 32 },
};

static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);

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

static void setBit(uint64_t& value, int bit)
{
    value |= uint64_t(1) << uint64_t(bit % 64);
}
static bool isSet(uint64_t value, int bit)
{
    return ((value >> uint64_t(bit % 64)) & 1) == 1;
}

static bool containsValue(int mapIndex, int value)
{
    const auto& v = s_map[mapIndex];
    return std::find(v.begin(), v.end(), value) != v.end();
}

static bool isValidRow(const std::vector<int>& row)
{
    for(int i = 1; i < row.size(); ++i)
    {
        if(!containsValue(row[i], row[i - 1]))
        {
            return false;
        }
    }
    return true;
}

static void a()
{
    int64_t count = 0;
    std::unordered_set<int> visited;
    for(const auto& row : s_rows )
    {
        if(isValidRow(row))
        {
            assert(row.size() % 2);
            count += row[row.size() / 2];
        }
    }
    printf("05-a Sum of valid middle numbers %i times.\n", int(count));
}

static void sortRow(std::vector<int>& row)
{
#if 0
    std::sort(row.begin(), row.end(), [](int& a, int& b)
    {
        return containsValue(a, b);
    });
    // does assert... the row order is not always same
    //assert(isValidRow(row));
#elif 1

    for(int i = 0; i < row.size(); ++i)
    {
        for(int j = 0; j < row.size(); ++j)
        {
            if(i == j)
            {
                continue;
            }
            if(!containsValue(row[i], row[j]))
            {
                std::swap(row[i], row[j]);
            }
        }
    }
    //assert(isValidRow(row));

#endif
}


static void b()
{
    int64_t count = 0;

    for(auto& row : s_rows )
    {
        if(!isValidRow(row))
        {
            sortRow(row);
            assert(row.size() % 2);
            count += row[row.size() / 2];
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
#if 1
    s_buffers[BufferInput] = (createGPUWriteBuffer(s_input.size(), "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(1024, "ResultBuffer"));

    s_buffers[BufferParsedGates] = (createGPUWriteBuffer(2048 * 2 * sizeof(int), "ParsedGates"));
    s_buffers[BufferParsedGatesIndices] = (createGPUWriteBuffer(256 * 2 * sizeof(int), "ParsedGatesIndices"));

    s_buffers[BufferParsedLines] = (createGPUWriteBuffer(1024 * 32 * sizeof(int), "ParsedLines"));
    s_buffers[BufferParsedLinesIndices] = (createGPUWriteBuffer(1024 * 2 * sizeof(int), "ParsedLinesIndices"));

    s_buffers[BufferIndirect] = (createGPUWriteBuffer(1024, "IndirecBuffer", true));


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
    downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], 1024);
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

    printf("05-a compute Sum of valid middle numbers %i times.\n", s_dataBuffer[0]);
    printf("05-b compute Sum of invalid sorted middle numbers %i times.\n", s_dataBuffer[1]);
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
        // Reset atomic buffer
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferResult] }
            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineAtomicBufferReset]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }


        // Parse values
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferInput] },
                { .buffer = s_buffers[BufferParsedGates] },
                { .buffer = s_buffers[BufferParsedGatesIndices] },
                { .buffer = s_buffers[BufferParsedLines] },
                { .buffer = s_buffers[BufferParsedLinesIndices] },
                { .buffer = s_buffers[BufferIndirect] },

            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineParse]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
#if 1
        // Indirect calculate dispatch
        {
            SDL_GPUStorageBufferReadWriteBinding buffers[] = {
                { .buffer = s_buffers[BufferParsedGates] },
                { .buffer = s_buffers[BufferParsedGatesIndices] },
                { .buffer = s_buffers[BufferParsedLines] },
                { .buffer = s_buffers[BufferParsedLinesIndices] },
                { .buffer = s_buffers[BufferResult] },

            };
            SDL_GPUComputePass* computePass = SDL_BeginGPUComputePass(
                cmd,
                nullptr,
                0,
                buffers,
                sizeof(buffers) / sizeof(SDL_GPUStorageBufferReadWriteBinding)
            );

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineCalculateMiddleNumbersIndirect]);
            // Indirect call
            SDL_DispatchGPUComputeIndirect(computePass, s_buffers[BufferIndirect], 0);
            SDL_EndGPUComputePass(computePass);

        }
#endif
    }
    // Get the data from gpu to cpu, doesnt anymore seem to work here
    //downloadGPUBuffer((uint8_t*)s_dataBuffer, s_buffers[BufferResult], 1024);

    //if(index > 100)
    //    return false;
#endif
    return true;
}
