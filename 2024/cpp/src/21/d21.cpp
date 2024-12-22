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

static const std::string s_Filename = "input/21.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

static std::vector<std::string> s_codes;
static std::vector<int> s_numbers;

static std::unordered_map<int, std::vector<std::string>> s_robotMoveMap;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 21";
}

static void parse()
{
    int row = 0;
    std::string line;
    std::istringstream iss(s_input);
    while(std::getline(iss, line))
    {
        s_codes.push_back(line);
        std::istringstream iss2(line.substr(0, line.length() - 1));
        int tmp;
        iss2 >> tmp;
        s_numbers.push_back(tmp);
    }
}


static int getDoorCol(char c)
{
    switch(c)
    {
        case '7': case '4': case '1':           return 0;
        case '8': case '5': case '2': case '0': return 1;
        case '9': case '6': case '3': case 'A': return 2;
        default: break;
    }
    assert(false);
    return -1;
}

static int getDoorRow(char c)
{
    switch(c)
    {
        case '7': case '8': case '9': return 0;
        case '4': case '5': case '6': return 1;
        case '1': case '2': case '3': return 2;
                  case '0': case 'A': return 3;
        default: break;
    }
    assert(false);
    return -1;
}

static int getRobotCol(char c)
{
    switch(c)
    {
                  case '<': return 0;
        case '^': case 'v': return 1;
        case 'A': case '>': return 2;
        default: break;
    }
    assert(false);
    return -1;
}

static int getRobotRow(char c)
{
    switch(c)
    {
                  case '^': case 'A':  return 0;
        case '<': case 'v': case '>':  return 1;
        default: break;
    }
    assert(false);
    return -1;
}

static int getMapIndex(int from, int to)
{
    int index = from + to * 1000;
    return index;
}

static V2 getCoordFromIndex(int index)
{
    return {index % 1000, index / 1000};
}

void addMoves(
    int x1, int y1,
    int x2, int y2,
    int sizeX, int sizeY,
    int holeX, int holeY,
    std::string s, std::vector<std::string>& moves)
{
    if(x1 < 0 || x1 >= sizeX || y1 >= sizeY)
    {
        return;
    }
    if(x1 == holeX && y1 == holeY)
    {
        return;
    }
    if(x1 == x2 && y1 == y2)
    {
        s += 'A';
        moves.push_back(s);
        return;
    }

    int xDiff = x2 - x1;
    int yDiff = y2 - y1;

    if(xDiff != 0)
    {
        std::string s1 = s;
        s1.append(std::abs(xDiff), xDiff > 0 ? '>' : '<');
        addMoves(x1 + xDiff, y1, x2, y2, sizeX, sizeY, holeX, holeY, s1, moves);
    }
    if(yDiff != 0)
    {
        std::string s1 = s;
        s1.append(std::abs(yDiff), yDiff > 0 ? 'v' : '^');
        addMoves(x1, y1 + yDiff, x2, y2, sizeX, sizeY, holeX, holeY, s1, moves);
    }
}

void parseRobotMoveMap(const std::vector<char>& buttons, int sizeX, int sizeY, int holeX, int holeY, bool isDoorRobot)
{
    for(int i = 0; i < buttons.size(); ++i)
    {
        for(int j = 0; j < buttons.size(); ++j)
        {
            char iChar = buttons[i];
            char jChar = buttons[j];
            auto& moveMap = s_robotMoveMap[getMapIndex(iChar, jChar)];

            int colFrom = isDoorRobot ? getDoorCol(iChar) : getRobotCol(iChar);
            int rowFrom = isDoorRobot ? getDoorRow(iChar) : getRobotRow(iChar);

            int colTo = isDoorRobot ? getDoorCol(jChar) : getRobotCol(jChar);
            int rowTo = isDoorRobot ? getDoorRow(jChar) : getRobotRow(jChar);

            int colDiff = colTo - colFrom;
            int rowDiff = rowTo - rowFrom;

            addMoves(colFrom, rowFrom,
                colTo, rowTo,
                sizeX, sizeY,
                holeX, holeY,
                "",
                moveMap);
        }
    }
}

std::unordered_map<int, int64_t> s_memoize;

int64_t getButtonPresses(const std::string& code, int depth)
{
    if(depth == 0)
    {
        return code.length();
    }
    int64_t result = 0;
    char currState = 'A';

    for(int index = 0; index < code.length(); ++index)
    {
        char c = code[index];
        int64_t mapIndex = getMapIndex(currState, c);
        const auto& v = s_robotMoveMap[mapIndex];
        int64_t result2 = 0x7fff'ffff'ffff'ffff;

        int memoizeIndex = (depth - 1) * 1'000'000 + mapIndex;
        auto f = s_memoize.find(memoizeIndex);

        if(f == s_memoize.end())
        {
            for(const auto& str : v)
            {
                result2 = std::min(getButtonPresses(str, depth - 1), result2);
            }
            s_memoize[memoizeIndex] = result2;
        }
        else
        {
            result2 = f->second;
        }
        assert(result2 < 0x7fff'ffff'ffff'ffff);
        result += result2;
        currState = c;
    }
    return result;
}

static void a()
{
    int64_t result = 0;
    parseRobotMoveMap(
        { '0', '1', '2','3', '4', '5', '6', '7', '8', '9', 'A' },
        3, 4,
        0, 3,
        true
    );
    parseRobotMoveMap(
        { 'A', '<', 'v', '>', '^' },
        3, 2,
        0, 0,
        false
    );
    for(int i = 0; i < s_codes.size(); ++i)
    {
        int64_t score = getButtonPresses(s_codes[i], 3) * s_numbers[i];
        result += score;
    }

    printf("21-a Button presses %" SDL_PRIs64 "\n", result);
}

static void b()
{
    int64_t result = 0;
    s_memoize.clear();
    for(int i = 0; i < s_codes.size(); ++i)
    {
        int64_t score = getButtonPresses(s_codes[i], 26) * s_numbers[i];
        result += score;
    }

    printf("21-b Button presses %" SDL_PRIs64 "\n", result);
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
