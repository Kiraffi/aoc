#include <algorithm>
#include <bit> //std::popcount
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


#include "d14_comp.h"
#include "d14_shared.h"

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
    PipelineD14,

    PipelineCount
};

static ComputePipelineInfo s_pipelineInfos[] =
{
    { BUF_N_SIZE(d14_comp), 2, 1, 1024 },
};
static_assert(sizeof(s_pipelineInfos) / sizeof(ComputePipelineInfo) == PipelineCount);

struct V2
{
    int m_x;
    int m_y;
};

struct Robot
{
    V2 m_pos;
    V2 m_vel;
};

static std::vector<Robot> s_robots;

static const std::string s_Filename = "input/14.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);




static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 14";
}

V2 static parseNumbers(const std::string& line)
{
    V2 result;
    std::istringstream iss(line);

    auto parseNumb = [](const std::string& line, char afterChar)
    {
        auto iter = line.find(afterChar);
        if(iter == std::string::npos)
        {
            return 0;
        }

        std::istringstream iss(line.substr(iter + 1));
        int numb;
        iss >> numb;
        return numb;
    };
    std::string tmp;
    iss >> tmp;
    result.m_x = parseNumb(tmp, '=');
    iss >> tmp;
    result.m_y = parseNumb(tmp, ',');

    return result;
}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);

    while(std::getline(iss, line))
    {
        Robot robot = {};

        std::istringstream iss(line);
        std::string tmp;
        iss >> tmp;
        robot.m_pos = parseNumbers(tmp);
        iss >> tmp;
        robot.m_vel = parseNumbers(tmp);
        s_robots.push_back(robot);
    }
}

static void a()
{
    int64_t count = 0;

    int64_t tl = 0;
    int64_t tr = 0;
    int64_t bl = 0;
    int64_t br = 0;


    std::vector<Robot> robots = s_robots;

    static const int RoomSizeWidth = 101;
    static const int RoomSizeHeight = 103;

    for(Robot& r : robots)
    {
        r.m_pos.m_x += r.m_vel.m_x * 100;
        r.m_pos.m_y += r.m_vel.m_y * 100;

        r.m_pos.m_x %= RoomSizeWidth;
        r.m_pos.m_y %= RoomSizeHeight;

        r.m_pos.m_x += RoomSizeWidth;
        r.m_pos.m_y += RoomSizeHeight;

        r.m_pos.m_x %= RoomSizeWidth;
        r.m_pos.m_y %= RoomSizeHeight;

        if(r.m_pos.m_x < RoomSizeWidth / 2 && r.m_pos.m_y < RoomSizeHeight / 2) tl++;
        if(r.m_pos.m_x < RoomSizeWidth / 2 && r.m_pos.m_y > RoomSizeHeight / 2) bl++;
        if(r.m_pos.m_x > RoomSizeWidth / 2 && r.m_pos.m_y < RoomSizeHeight / 2) tr++;
        if(r.m_pos.m_x > RoomSizeWidth / 2 && r.m_pos.m_y > RoomSizeHeight / 2) br++;
    }
    count = tl * tr * bl * br;

    printf("14-a Safety factor %" SDL_PRIs64 "\n", count);
}


static void b()
{
    int64_t count = 0;

    std::vector<Robot> robots = s_robots;

    static const int RoomSizeWidth = 101;
    static const int RoomSizeHeight = 103;

    uint64_t roomRow[(2 * RoomSizeHeight)] = {};
    uint64_t roomCol[(2 * RoomSizeWidth)] = {};

    for(; count < 10000; ++count)
    {
        int highestRow = 0;
        int highestCol = 0;

        memset(roomRow, 0, sizeof(roomRow));
        memset(roomCol, 0, sizeof(roomCol));
        for(Robot& r : robots)
        {
            r.m_pos.m_x += r.m_vel.m_x + RoomSizeWidth;
            r.m_pos.m_y += r.m_vel.m_y + RoomSizeHeight;

            r.m_pos.m_x %= RoomSizeWidth;
            r.m_pos.m_y %= RoomSizeHeight;

            roomRow[r.m_pos.m_y * 2 + r.m_pos.m_x / 64] |= (uint64_t(1) << (uint64_t(r.m_pos.m_x % 64)));

            roomCol[r.m_pos.m_x * 2 + r.m_pos.m_y / 64] |= (uint64_t(1) << (uint64_t(r.m_pos.m_y % 64)));

        }
        for(int j = 0; j < RoomSizeHeight; ++j)
        {
            int row = std::popcount(roomRow[j * 2 + 0]);
            row += std::popcount(roomRow[j * 2 + 1]);
            highestRow = std::max(highestRow, row);
            //printf("\n");
        }


        for(int i = 0; i < RoomSizeWidth; ++i)
        {
            int col = std::popcount(roomCol[i * 2 + 0]);
            col += std::popcount(roomCol[i * 2 + 1]);

            highestCol = std::max(highestCol, col);
        }

        //printf("%" SDL_PRIs64 ", highest row: %i,  highest col: %i\n", count, highestRow, highestCol);
        if(highestCol > 25 && highestRow > 25)
        {
            break;
        }
    }
    printf("14-b Picture on image %" SDL_PRIs64 "\n", count + 1);
}

static void doCpu()
{
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

    int computeDebugNumbers = s_dataBuffer[4];
    printf("Compute debug number count: %i\n", computeDebugNumbers);

/*
    D14TempData* dataPtr = (D14TempData*)(s_dataBuffer + 8);

    for(int i = 0; i < computeDebugNumbers; ++i)
    {
        printf("Pos {%i, %i}, Vel {%i, %i}\n", dataPtr->posX, dataPtr->posY, dataPtr->velX, dataPtr->velY );
        dataPtr++;
    }
*/
    printf("14-a compute safety factor: %i\n", s_dataBuffer[0]);
    printf("14-b compute safe: %i\n", s_dataBuffer[2]);

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

            SDL_BindGPUComputePipeline(computePass, s_pipelines[PipelineD14]);
            SDL_DispatchGPUCompute(computePass, 1, 1, 1);
            SDL_EndGPUComputePass(computePass);

        }
    }

#endif
    return true;
}
