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

enum Op
{
    OpNone,
    OpAND,
    OpOR,
    OpXOR,

    OpCount,
};

struct Wire
{
    uint32_t left;
    uint32_t right;
    uint32_t assign;
    Op op;
};

static const std::string s_Filename = "input/24.input";


static SDL_GPUComputePipeline* s_pipelines[PipelineCount] = {};

static SDL_GPUBuffer* s_buffers[BufferCount] = {};

static std::string s_input = readInputFile(s_Filename);

using ValueMap = std::unordered_map<uint32_t, uint8_t>;
using WireMap = std::vector<Wire>;
static ValueMap s_gateValueMap;
static WireMap s_wires;

static int s_dataBuffer[ValuesBufferSize] = {};

const char* getTitle()
{
    return "AOC 2024 day 24";
}

static uint32_t parseValue(const std::string& s)
{
    uint32_t value = uint32_t(s[0]) | (uint32_t(s[1]) << 8) | (uint32_t(s[2]) << 16);
    return value;
}

static std::string getStringFromIndex(uint32_t index)
{
    std::string s;
    s += char((index >> 0) & 0xff);
    s += char((index >> 8) & 0xff);
    s += char((index >> 16) & 0xff);

    return s;
}

static Op parseOp(const std::string &s)
{
    static const std::string OpNames[] = {
        "AND",
        "OR",
        "XOR",
    };

    for(int i = 0; i < SDL_arraysize(OpNames); ++i)
    {
        if(OpNames[i] == s)
        {
            return Op(i + 1);
        }
    }
    assert(false);
    return Op::OpNone;
}

static void parse()
{
    int row = 0;
    std::string line;
    std::istringstream iss(s_input);
    while(std::getline(iss, line))
    {
        if(line.empty())
        {
            break;
        }
        uint32_t value = parseValue(line);
        s_gateValueMap.insert({value, line[5] == '1' ? 1 : 0});
    }

    while(std::getline(iss, line))
    {
        std::istringstream iss2(line);
        std::string s;

        Wire wire = {};

        iss2 >> s;
        wire.left = parseValue(s);

        iss2 >> s;
        wire.op = parseOp(s);

        iss2 >> s;
        wire.right = parseValue(s);

        iss2 >> s;
        iss2 >> s;
        wire.assign = parseValue(s);

        s_wires.push_back(wire);
    }
}

static int8_t doOp(Op op, int8_t l, int8_t r)
{
    switch(op)
    {
        case Op::OpAND: return l && r;
        case Op::OpOR: return l || r;
        case Op::OpXOR: return l ^ r;

        case Op::OpNone:
        case Op::OpCount:
            assert(false);
    }
    return Op::OpNone;
}

static std::string createStrIndex(char c, int number)
{
    std::string s;
    s += c;
    s += '0' + (number / 10);
    s += '0' + (number % 10);
    return s;
}

static uint32_t createIndex(char c, int number)
{
    return parseValue(createStrIndex(c, number));
}

static void finishValues(const WireMap& wires, ValueMap& gateValueMap)
{
    bool allDone = false;
    while(!allDone)
    {
        allDone = true;
        for(const auto& wire : wires)
        {
            auto f1 = gateValueMap.find(wire.left);
            auto f2 = gateValueMap.find(wire.right);

            if(f1 != gateValueMap.end() && f2 != gateValueMap.end())
            {
                gateValueMap[wire.assign] = doOp(wire.op, f1->second, f2->second);
            }
            else
            {
                allDone = false;
            }
        }
    }
}

static uint64_t getValue(char c, const ValueMap& valueMap)
{
    uint64_t result = 0;
    int64_t index = 0;
    auto findValue = [&result, &valueMap, c](int index) -> bool
    {
        uint32_t value = createIndex(c, index);
        auto f = valueMap.find(value);
        if(f != valueMap.end())
        {
            result |= (int64_t(f->second)) << index;
            return true;
        }
        return false;
    };

    while(findValue(index))
    {
        ++index;
    }
    return result;
}



static void a()
{
    int64_t result = 0;

    auto gateValueMap = s_gateValueMap;
    finishValues(s_wires, gateValueMap);
    result = getValue('z', gateValueMap);

    printf("24-a Value: %" SDL_PRIs64 "\n", result);
}


static void setValues(char c, int64_t index, ValueMap& gateValueMap)
{
    for(int i = 0; i < 45; ++i)
    {
        gateValueMap[createIndex(c, i)] = 0;
    }
    if(index >= 0)
    {
        gateValueMap[createIndex(c, index)] = 1;
    }
}

static void swapStrs(const std::string& s1, const std::string& s2, WireMap& wires)
{
    uint32_t vv1 = parseValue(s1);
    uint32_t vv2 = parseValue(s2);
    int v1 = -1;
    int v2 = -1;

    int index = 0;

    for(int index = 0; index < s_wires.size(); ++index)
    {
        const Wire& w = s_wires[index];
        if(w.assign == vv1)
        {
            v1 = index;
        }
        if(w.assign == vv2)
        {
            v2 = index;
        }
    }
    if(v1 >= 0 && v2 >= 0)
    {
        std::swap(wires[v1].assign, wires[v2].assign);
    }
}

static void printMap(char c, const ValueMap& valueMap)
{
    std::string s;
    int index = 0;
    while(true)
    {
        auto f = valueMap.find(createIndex(c, index));
        if(f == valueMap.end())
        {
            break;
        }
        ++index;
        s = std::to_string(f->second) + s;
    }
    s.insert(0, 64 - s.size(), ' ');
    printf("%s\n", s.c_str());
}

void printLR(uint32_t assign, const WireMap& wires)
{
    for(const auto& w : wires)
    {
        if(w.left == assign)
        {
            printf(", l: %s", getStringFromIndex(w.assign).c_str());
        }
        if(w.right == assign)
        {
            printf(", r: %s", getStringFromIndex(w.assign).c_str());
        }
    }
}

const Wire getWireAssign(uint32_t assign, const WireMap& wires)
{
    for(const auto& wire : wires)
    {
        if(wire.assign == assign)
        {
            return wire;
        }
    }
    assert(false);
    return Wire{};
}


const std::vector<Wire> getWireLR(uint32_t lr, const WireMap& wires)
{
    std::vector<Wire> result;
    for(const auto& wire : wires)
    {
        if(wire.left == lr || wire.right == lr)
        {
            result.push_back(wire);
        }
    }
    assert(result.size() > 0);
    return result;
}

static std::vector<std::string> findXOR(uint32_t value, const std::vector<Wire>& xyWires, WireMap& wires)
{
    std::vector<std::string> swaps;
    for(const auto& w : xyWires)
    {
        if(w.op == OpXOR)
        {
            std::vector<Wire> otherWires = getWireLR(w.assign, wires);
            for(const auto& ww : otherWires)
            {
                if(ww.op == OpXOR)
                {
                    swaps.push_back(getStringFromIndex(ww.assign));
                    swaps.push_back(createStrIndex('z', value));
                    swapStrs(swaps[0], swaps[1], wires);
                }
            }
        }
    }
    return swaps;
}

static std::unordered_set<int> findWrongValues(const WireMap& wires)
{
    std::unordered_set<int> wrongValues;

    for(int x = 0; x < 45; ++x)
    {
        ValueMap valueMap;
        setValues('x', x, valueMap);
        setValues('y', -1, valueMap);
        finishValues(wires, valueMap);

        uint64_t xValue = getValue('x', valueMap);
        uint64_t yValue = getValue('y', valueMap);
        uint64_t zValue = getValue('z', valueMap);
        if(xValue + yValue != zValue)
        {
            //printMap('x', valueMap);
            //printMap('y', valueMap);
            //printMap('z', valueMap);
            //printf("%i, %i\n\n", x, -1);
            wrongValues.insert(x);
        }

    }

    for(int x = 0; x < 45; ++x)
    {
        ValueMap valueMap;
        setValues('x', -1, valueMap);
        setValues('y', x, valueMap);
        finishValues(wires, valueMap);

        uint64_t xValue = getValue('x', valueMap);
        uint64_t yValue = getValue('y', valueMap);
        uint64_t zValue = getValue('z', valueMap);
        if(xValue + yValue != zValue)
        {
            //printMap('x', valueMap);
            //printMap('y', valueMap);
            //printMap('z', valueMap);
            //printf("%i, %i\n\n", -1, x);
            wrongValues.insert(x);
        }

    }

    for(int x = 0; x < 45; ++x)
    {
        ValueMap valueMap;
        setValues('x', x, valueMap);
        setValues('y', x, valueMap);
        finishValues(wires, valueMap);

        uint64_t xValue = getValue('x', valueMap);
        uint64_t yValue = getValue('y', valueMap);
        uint64_t zValue = getValue('z', valueMap);
        if(xValue + yValue != zValue)
        {
            //printMap('x', valueMap);
            //printMap('y', valueMap);
            //printMap('z', valueMap);
            //printf("%i, %i\n\n", x, x);
            wrongValues.insert(x);
        }
    }

    return wrongValues;
}


static void b()
{
    int64_t result = 0;

    auto wires = s_wires;

    if(true)
    {
        int v1 = 0;
        int v2 = 0;
/*
        swapStrs("z10", "vcf", wires);
        swapStrs("tnc", "z39", wires);
        swapStrs("fhg", "z17", wires);


        swapStrs("fsq", "dvb", wires);
*/
    }
    std::unordered_set<int> wrongValues = findWrongValues(wires);

    std::vector<std::string> strs;
    for(int wrongValue : wrongValues)
    {
        std::vector<Wire> xyWires = getWireLR(createIndex('x', wrongValue), wires);
        Wire zWire = getWireAssign(createIndex('z', wrongValue), wires);
        if(zWire.op != OpXOR)
        {
            const auto& v = findXOR(wrongValue, xyWires, wires);
            strs.insert(strs.end(), v.begin(), v.end());
        }
        else
        {
            int32_t swapAND = -1;
            int32_t swapXOR = -1;
            for(const Wire& ww : xyWires)
            {
                if(ww.op == OpAND)
                {
                    swapAND = ww.assign;
                }
                else if(ww.op == OpXOR)
                {
                    swapXOR = ww.assign;
                }
            }
            //getStringFromIndex(ww.assign);
            assert(swapAND >= 0 && swapXOR >= 0);
            if(zWire.left == swapAND || zWire.right == swapAND)
            {
                std::string s1 = getStringFromIndex(swapAND);
                std::string s2 = getStringFromIndex(swapXOR);
                strs.push_back(s1);
                strs.push_back(s2);

                swapStrs(s1, s2, wires);

            }
        }
    }
/*
    wrongValues = findWrongValues(wires);

    for(int x = 0; x < 45; ++x)
    {
        ValueMap valueMap;
        setValues('x', x, valueMap);
        setValues('y', x, valueMap);
        setValues('x', x - 1, valueMap);
        setValues('y', x - 1, valueMap);
        finishValues(wires, valueMap);

        uint64_t xValue = getValue('x', valueMap);
        uint64_t yValue = getValue('y', valueMap);
        uint64_t zValue = getValue('z', valueMap);
        if(xValue + yValue != zValue)
        {
            printMap('x', valueMap);
            printMap('y', valueMap);
            printMap('z', valueMap);
            printf("%i, %i\n\n", x, x);
            wrongValues.insert(x);
        }
    }


    for(int value : wrongValues)
    {
        printf("%i\n", value);
    }

    auto gateValueMap = s_gateValueMap;
    finishValues(wires, gateValueMap);

    printMap('x', gateValueMap);
    printMap('y', gateValueMap);
    printMap('z', gateValueMap);

    {
        uint64_t xValue = getValue('x', gateValueMap);
        uint64_t yValue = getValue('y', gateValueMap);
        uint64_t zValue = getValue('z', gateValueMap);
        if(xValue + yValue != zValue)
        {
            printMap('x', gateValueMap);
            printMap('y', gateValueMap);
            printMap('z', gateValueMap);
        }
    }
    */
    std::sort(strs.begin(), strs.end());
    std::string printStr;

    for(const std::string& str : strs)
    {
        printStr += str;
        printStr += ",";
    }
    printStr = printStr.substr(0, printStr.size() - 1);

    printf("24-b Swapped strings: %s\n", printStr.c_str());
    //printf("24-b  %" SDL_PRIs64 "\n", result);
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
