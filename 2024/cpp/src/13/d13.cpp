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

#include "d13_comp.h"
#include "int64_shared.h"

#include "commons.h"
#include "commonrender.h"

static const int ValuesBufferSize = 4096;

enum BufferEnum : int
{
    BufferInput,
    BufferResult,

    BufferCount
};

enum PipelineEnum
{
    PipelineD13,

    PipelineCount
};


static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(d13_comp), 2, 1, 1024 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);


struct V2
{
    int64_t m_x;
    int64_t m_y;
};

struct Machine
{
    V2 m_aButton;
    V2 m_bButton;
    V2 m_prize;
};

static std::vector<Machine> s_machines;

static const std::string s_Filename = "input/13.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);




static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 13";
}

static V2 parseButton(const std::string& line)
{
    V2 result = {};
    std::istringstream iss(line);
    std::string tmp;
    iss >> tmp;
    iss >> tmp;
    iss >> tmp;
    std::istringstream iss2(tmp.substr(2));
    iss2 >> result.m_x;
    iss >> tmp;
    std::istringstream iss3(tmp.substr(2));
    iss3 >> result.m_y;

    assert(result.m_x < 100 && result.m_y < 100);
    return result;

}

static V2 parsePrize(const std::string& line)
{
    V2 result = {};
    std::istringstream iss(line);
    std::string tmp;
    iss >> tmp;
    iss >> tmp;
    std::istringstream iss2(tmp.substr(2));
    iss2 >> result.m_x;
    iss >> tmp;
    std::istringstream iss3(tmp.substr(2));
    iss3 >> result.m_y;

    return result;

}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    int lineNumb = 0;
    Machine tmp = {};
    while(std::getline(iss, line))
    {

        switch(lineNumb % 4)
        {
            case 0: tmp.m_aButton = parseButton(line); break;
            case 1: tmp.m_bButton = parseButton(line); break;
            case 2: tmp.m_prize = parsePrize(line); break;
            case 3: s_machines.push_back(tmp); break;
            default: break;
        }

        lineNumb++;
    }
    s_machines.push_back(tmp);
}

static void a()
{
    int64_t count = 0;
    for(const Machine& m : s_machines)
    {
        int aPresses = 0;
        int bPresses = 0;
        bool found = false;

        int lowestCost = 10000000;
        int solutionCount = 0;

        int64_t ix = m.m_prize.m_x / m.m_aButton.m_x;
        int64_t iy = m.m_prize.m_y / m.m_aButton.m_y;
        int64_t i = std::min(ix, iy);
        int foundi = 0;
        int foundj = 0;
        while(i >= 0)
        {
            int64_t j = (m.m_prize.m_x - (m.m_aButton.m_x * i)) / m.m_bButton.m_x;

            V2 locA = { i * m.m_aButton.m_x, i * m.m_aButton.m_y };
            V2 locB = { j * m.m_bButton.m_x, j * m.m_bButton.m_y };

            V2 loc = { locA.m_x + locB.m_x, locA.m_y + locB.m_y };

            if(loc.m_x == m.m_prize.m_x && loc.m_y == m.m_prize.m_y)
            {
                int cost = i * 3 + j * 1;
                foundi = i;
                foundj = j;
                lowestCost = std::min(cost, lowestCost);
                found = true;
                ++solutionCount;
            }

            --i;
        }
        if(found)
        {
            count += lowestCost;
/*
        printf("A: %i, %i, b:%i %i, p: %i, %i, lowest cost: %i, i: %i, j: %i\n",
            m.m_aButton.m_x,
            m.m_aButton.m_y,
            m.m_bButton.m_x,
            m.m_bButton.m_y,
            m.m_prize.m_x,
            m.m_prize.m_y,
            lowestCost,
*/
        }
    }
    printf("13-a Cost %" SDL_PRIs64 "\n", count);
}

static bool compare(const Machine& m, int index, V2 prize)
{
    int64_t j = (prize.m_x - (m.m_aButton.m_x * index)) / m.m_bButton.m_x;

    V2 locA = { index * m.m_aButton.m_x, index * m.m_aButton.m_y };
    V2 locB = { j * m.m_bButton.m_x, j * m.m_bButton.m_y };

    V2 loc = { locA.m_x + locB.m_x, locA.m_y + locB.m_y };

    return loc.m_x == prize.m_x && loc.m_y == prize.m_y;
}

static void b()
{
    int64_t count = 0;
    for(const Machine& m : s_machines)
    {
        V2 prize = m.m_prize;
        prize.m_x += 10000000000000;
        prize.m_y += 10000000000000;

        V2 aButton = m.m_aButton;
        V2 bButton = m.m_bButton;
        // check if y grows when we replace aButtons with bButtons.
        bool aXPerYSmallerThanB = ((100000 * aButton.m_x) / aButton.m_y) < ((100000 * bButton.m_x) / bButton.m_y);

        int aPresses = 0;
        int bPresses = 0;
        bool found = false;

        int64_t lowestCost = 10000000000000;



        int64_t i0 = 0;
        int64_t i2 = prize.m_x / m.m_aButton.m_x;

        int64_t j = 0;

        if(compare(m, 0, prize))
        {
            j = (prize.m_x - (m.m_aButton.m_x * 0)) / m.m_bButton.m_x;

            int64_t cost = 0 * 3 + j * 1;
            lowestCost = cost; //std::min(cost, lowestCost);
            found = true;
        }
        else if(compare(m, i2, prize))
        {
            j = (prize.m_x - (m.m_aButton.m_x * i2)) / m.m_bButton.m_x;
            int64_t cost = i2 * 3 + j * 1;
            lowestCost = cost; //std::min(cost, lowestCost);
            found = true;

        }
        else
        {
            while(i0 < i2)
            {
                int64_t mid = (i0 + i2) / 2;
                j = (prize.m_x - (m.m_aButton.m_x * mid)) / m.m_bButton.m_x;

                V2 locA = { mid * m.m_aButton.m_x, mid * m.m_aButton.m_y };
                V2 locB = { j * m.m_bButton.m_x, j * m.m_bButton.m_y };

                V2 loc = { locA.m_x + locB.m_x, locA.m_y + locB.m_y };

                if(loc.m_x == prize.m_x && loc.m_y == prize.m_y)
                {
                    int64_t cost = mid * 3 + j * 1;
                    i0 = mid;
                    lowestCost = cost; //std::min(cost, lowestCost);
                    found = true;
                    break;
                }
                // add the remaining ys, to get correct amount.
                int64_t yValue = loc.m_y;
                yValue += ((prize.m_x - loc.m_x) * m.m_bButton.m_y + m.m_bButton.m_y - 1) / m.m_bButton.m_x;

                if(yValue > prize.m_y == aXPerYSmallerThanB)
                {
                    i2 = mid;
                }
                else
                {
                    i0 = mid + 1;
                }
            }
        }

        if(found)
        {
            count += lowestCost;
        }
    }
    printf("13-b cost %" SDL_PRIs64 "\n", count);
}


static void doCpu()
{
    uvec2 a1(1122342234, 1234223);
    uvec2 b1(223432464, 1264363);
    uvec4 c = mul128(a1, b1);

    uint64_t aa = get64(a1);
    uint64_t bb = get64(b1);
    uint64_t c1 = get64(c.a);
    uint64_t c2 = get64(c.b);

    __uint128_t cc = __uint128_t(aa) * __uint128_t(bb);
    __uint128_t cc1 = __uint128_t(cc) & __uint128_t(0xffff'ffff'ffff'ffffu);
    __uint128_t cc2 = __uint128_t(cc) >> __uint128_t(64);

    parse();
    a();
    b();
}












bool initCompute()
{
    s_buffers[BufferInput] = (createGPUWriteBuffer(((s_input.size() + 7) >> 3) * 8, "Input"));
    s_buffers[BufferResult] = (createGPUWriteBuffer(ValuesBufferSize * sizeof(int), "ResultBuffer"));

    // upload the input data to a buffer
    uploadGPUBufferOneTimeInInit(s_buffers[BufferInput], (uint8_t*)s_input.data(), s_input.size());

#if 1
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
    int computeDebugNumbers = s_dataBuffer[1];

    printf("Compute debug number count: %i\n", computeDebugNumbers);
#if 0
    for(int i = 0; i < computeDebugNumbers; i += 12)
    {
        uint64_t v0 = *((uint64_t*)(&s_dataBuffer[i + 4]));
        uint64_t v1 = *((uint64_t*)(&s_dataBuffer[i + 6]));
        uint64_t v2 = *((uint64_t*)(&s_dataBuffer[i + 8]));
        uint64_t v3 = *((uint64_t*)(&s_dataBuffer[i + 10]));
        uint64_t v4 = *((uint64_t*)(&s_dataBuffer[i + 12]));
        uint64_t v5 = *((uint64_t*)(&s_dataBuffer[i + 14]));
        printf("prizex: %" SDL_PRIu64 ", prizey: %" SDL_PRIu64 ", = %"
            SDL_PRIu64 ", (%" SDL_PRIu64 ", %" SDL_PRIu64 ") gpu, ddx(%" SDL_PRIu64 ")\n",
            v0, v1, v2, v3, v4, v5);
        /*
        printf("prizex: %i, prizey: %i, value: %i, gpu (%i, %i) a: [%i, %i], b: [%i, %i]\n",
            s_dataBuffer[i + 4],
            s_dataBuffer[i + 5],
            s_dataBuffer[i + 6],
            s_dataBuffer[i + 7],
            s_dataBuffer[i + 8],
            s_dataBuffer[i + 9],
            s_dataBuffer[i + 10],
            s_dataBuffer[i + 11],
            s_dataBuffer[i + 12]);
        */
    }
#endif

    printf("13-a compute cost: %i\n", s_dataBuffer[0]);
    printf("13-b compute cost: %" SDL_PRIu64 "\n", get64(*((uvec2*)(&s_dataBuffer[2]))));

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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD13]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }

#endif
    return true;
}
