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

static const std::string s_Filename = "input/17.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);



static int s_dataBuffer[ValuesBufferSize] = {};


struct Computer
{
    int64_t m_regA;
    int64_t m_regB;
    int64_t m_regC;
    int64_t m_ip;
    std::vector<int> m_outValues;
};
static Computer s_computer = {};
static std::vector<int> s_program;

const char* getTitle()
{
    return "AOC 2024 day 17";
}

static int parseRegister(const std::string& line)
{
    std::istringstream iss(line);
    std::string tmp;
    iss >> tmp;
    iss >> tmp;
    int result;
    iss >> result;

    return result;
}

static void parseProgram(const std::string& line)
{
    std::istringstream iss(line);
    std::string tmp;
    iss >> tmp;
    int value;
    while(iss >> value)
    {
        char c;
        iss >> c;
        s_program.push_back(value);
    }

}

static void parse()
{
    std::string line;
    std::istringstream iss(s_input);
    std::getline(iss, line);
    s_computer.m_regA = parseRegister(line);
    std::getline(iss, line);
    s_computer.m_regB = parseRegister(line);
    std::getline(iss, line);
    s_computer.m_regC = parseRegister(line);
    std::getline(iss, line);
    std::getline(iss, line);
    parseProgram(line);

/*
    while(std::getline(iss, line))
    {
    }
*/
}

bool readInst(Computer& cpu, const std::vector<int>& prog)
{
    if(cpu.m_ip >= prog.size())
    {
        return false;
    }
    int op = prog[cpu.m_ip];
    int64_t lit = prog[cpu.m_ip + 1];
    cpu.m_ip += 2;

    int64_t combo = lit;
    switch(combo)
    {
        case 4: combo = cpu.m_regA; break;
        case 5: combo = cpu.m_regB; break;
        case 6: combo = cpu.m_regC; break;
    }
    if(op != 1 && op != 3 && lit == 7)
    {
        return false;
    }


    switch(op)
    {
        case 0: cpu.m_regA = cpu.m_regA / (1 << combo); break;
        case 6: cpu.m_regB = cpu.m_regA / (1 << combo); break;
        case 7: cpu.m_regC = cpu.m_regA / (1 << combo); break;

        case 1: cpu.m_regB = cpu.m_regB ^ lit; break;

        case 2: cpu.m_regB = combo % 8; break;

        case 3: if(cpu.m_regA != 0) cpu.m_ip = lit; break;
        case 4: cpu.m_regB = cpu.m_regB ^ cpu.m_regC; break;

        case 5: cpu.m_outValues.push_back(combo % 8); break;
    }
    return true;

}

static void printNumbers(const Computer& cpu)
{
    for(int i = 0; i < cpu.m_outValues.size() - 1; ++i)
    {
        printf("%i,", cpu.m_outValues[i]);
    }
    printf("%i\n", cpu.m_outValues.back());
}

static void a()
{
    //470
    int64_t result = 0;

    printf("day 17-a: ");


    Computer cpu = s_computer;
    while(readInst(cpu, s_program))
    {

    }
    printNumbers(cpu);
    //printf("a: %i, b: %i, c: %i\n", cpu.m_regA, cpu.m_regB, cpu.m_regC);
    //printf("17-a Smallest score %" SDL_PRIs64 "\n", result);
}

bool findNumber(int64_t& index, int64_t number)
{
    int64_t prev = index / 8;
    Computer cpu = s_computer;
    while(index / 8 == prev)
    {
        cpu.m_regA = index;
        cpu.m_regB = s_computer.m_regB;
        cpu.m_regC = s_computer.m_regC;
        cpu.m_outValues.clear();

        while(cpu.m_outValues.size() == 0)
        {
            readInst(cpu, s_program);
        }
        if(number == cpu.m_outValues[0])
        {
            return true;
        }
        ++index;
    }
    index = prev;
    return false;
}

static void b()
{
    int64_t result = 1;
    for(int i = s_program.size() - 1; i >= 0; i--)
    {
        int64_t prev = result;
        if(!findNumber(result, s_program[i]))
        {
            while(result / 8 != (result + 1) / 8)
            {
                i++;
                result /= 8;
            }
            result++;
            i += 2;
            continue;
        }
        result *= 8;
    }
    result /= 8;

    printf("17-b Smallet value to print same output as input %" SDL_PRIs64 "\n", result);

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
