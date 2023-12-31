#include <algorithm> // std::max
#include <assert.h> // assert
#include <bit> //std::popcount
#include <ctype.h> //isdigit
#include <immintrin.h> // SIMD
#include <inttypes.h> // PRI64
#include <math.h> // sqrt
#include <stdint.h> // intptr
#include <stdio.h> // printf
#include <stdlib.h> //strtol
#include <string.h> //strlen

#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>


#include "input.cpp"

#define PROFILE 0
#include "../profile.h"

alignas(32) static constexpr char test20A[] =
    R"(broadcaster -> a, b, c
%a -> b
%b -> c
%c -> inv
&inv -> a
)";

alignas(32) static constexpr char test20B[] =
    R"(broadcaster -> a
%a -> inv, con
&inv -> b
%b -> con
&con -> output
)";

static int sGetOrAdd(std::unordered_map<std::string, int>& stringMap, const char** data)
{
    const char* tmp = *data;
    while(isalpha(*tmp)) tmp++;
    std::string s(*data, tmp);
    auto iter = stringMap.find(s);
    int index = -1;
    if(iter == stringMap.end())
    {
        index = (int)stringMap.size();
        stringMap.insert({s, index});
    }
    else
    {
        index = iter->second;
    }
    *data = tmp;
    return index;

}

enum Op : uint8_t
{
    None = 0,
    FlipFlop = 1,
    Conj = 2,
    BroadCast = 4,
};

struct Device
{
    uint16_t states;
    uint8_t targetIndices[14];
    // which bit does this device control for the target for conj
    uint8_t targetStateIndices[14];
    uint8_t targetCount : 4;
    uint8_t sourceCount : 4;
    Op op;
};
// 4 * 8 bytes. Has at least 8 bytes extra for targets.
static_assert(sizeof(Device) == 32);

struct Pulse
{
    uint8_t index;
    uint8_t toSlotIndex : 4;
    uint8_t enable : 1;
};




static void sBroadcast(
    const Device& device,
    Pulse* otherP,
    int* otherPC,
    uint8_t state,
    int64_t& lows,
    int64_t& highs)
{
    for(int j = 0; j < device.targetCount; ++j)
    {
        otherP[*otherPC].index = device.targetIndices[j];
        otherP[*otherPC].toSlotIndex = device.targetStateIndices[j];
        otherP[*otherPC].enable = state;
        (*otherPC)++;
        if(state)
            ++highs;
        else
            ++lows;
    }
}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("20A Total");
    std::unordered_map<std::string, int> stringMap;
    Device devices[64] = {};

    while(*data)
    {
        Op op = {};
        if(*data == '%')
        {
            op = FlipFlop;
            data++;
        }
        else if(*data == '&')
        {
            op = Conj;
            data++;
        }
        else
        {
            op = BroadCast;
        }
        uint8_t index = sGetOrAdd(stringMap, &data);
        Device& device = devices[index];
        device.op = op;
        data += 2;
        while(*data != '\n')
        {
            data += 2;
            uint8_t targetIndex = sGetOrAdd(stringMap, &data);
            assert(device.targetCount <= 15);
            assert(devices[targetIndex].sourceCount <= 15);
            device.targetStateIndices[device.targetCount] = devices[targetIndex].sourceCount++;
            device.targetIndices[device.targetCount++] = targetIndex;
        }
        data++;
    }
    int startIndex = stringMap["broadcaster"];

    Pulse pulses1[256] = {};
    Pulse pulses2[256] = {};
    Pulse* currP = pulses1;
    Pulse* otherP = pulses2;

    int pulseCount1 = 1;
    int pulseCount2 = 0;
    int* currPC = &pulseCount1;
    int* otherPC = &pulseCount2;

    int64_t highs = 0;
    int64_t lows = 0;

    for(int k = 0; k < 1000; ++k)
    {
        currP[0].index = startIndex;
        currP[0].enable = 0;
        *currPC = 1;
        lows++;
        while (*currPC)
        {
            *otherPC = 0;
            for (int i = 0; i < *currPC; ++i)
            {
                const Pulse &pulse = currP[i];

                Device &device = devices[pulse.index];
                switch (device.op)
                {
                    case None:
                        break;
                    case FlipFlop:
                        if (!pulse.enable)
                        {
                            uint8_t b = device.states == 0 ? 1 : 0;
                            device.states = b;
                            sBroadcast(device, otherP, otherPC, b, lows, highs);
                        }
                        break;
                    case Conj:
                    {
                        uint16_t state = (uint16_t(pulse.enable)) << (uint16_t(pulse.toSlotIndex));
                        uint16_t turnOff = 1 << (uint16_t(pulse.toSlotIndex));
                        device.states &= ~turnOff;
                        device.states |= state;
                        uint16_t setBits = ((1 << uint16_t(device.sourceCount)) - 1);
                        if ((device.states & setBits) == setBits)
                        {
                            sBroadcast(device, otherP, otherPC, 0, lows, highs);
                        }
                        else
                        {
                            sBroadcast(device, otherP, otherPC, 1, lows, highs);
                        }
                        break;
                    }
                    case BroadCast:
                        uint8_t b = pulse.enable;
                        device.states = b;
                        sBroadcast(device, otherP, otherPC, b, lows, highs);
                        break;
                }
            }
            std::swap(currP, otherP);
            std::swap(currPC, otherPC);
        }
    }
    return lows * highs;
}


static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("20B Total");
    std::unordered_map<std::string, int> stringMap;
    Device devices[64] = {};

    while(*data)
    {
        Op op = {};
        if(*data == '%')
        {
            op = FlipFlop;
            data++;
        }
        else if(*data == '&')
        {
            op = Conj;
            data++;
        }
        else
        {
            op = BroadCast;
        }
        uint8_t index = sGetOrAdd(stringMap, &data);
        Device& device = devices[index];
        device.op = op;
        data += 2;
        while(*data != '\n')
        {
            data += 2;
            uint8_t targetIndex = sGetOrAdd(stringMap, &data);
            assert(device.targetCount <= 15);
            assert(devices[targetIndex].sourceCount <= 15);
            device.targetStateIndices[device.targetCount] = devices[targetIndex].sourceCount++;
            device.targetIndices[device.targetCount++] = targetIndex;
        }
        data++;
    }
    int startIndex = stringMap["broadcaster"];
    int rx = stringMap["rx"];
    int rxSource = 0;
    for(int i = 0; i < (int)stringMap.size(); ++i)
    {
        if(devices[i].targetIndices[0] == rx)
        {
            rxSource = i;
            break;
        }
    }

    Pulse pulses1[256] = {};
    Pulse pulses2[256] = {};
    Pulse* currP = pulses1;
    Pulse* otherP = pulses2;

    int pulseCount1 = 1;
    int pulseCount2 = 0;
    int* currPC = &pulseCount1;
    int* otherPC = &pulseCount2;

    int64_t highs = 0;
    int64_t lows = 0;
    int64_t k = 0;

    int64_t loops[16] = {};

    while(true)
    {
        currP[0].index = startIndex;
        currP[0].enable = 0;
        *currPC = 1;
        ++k;
        lows++;
        while (*currPC)
        {
            *otherPC = 0;
            for (int i = 0; i < *currPC; ++i)
            {
                const Pulse &pulse = currP[i];
                if(pulse.index == rxSource && pulse.enable)
                {
                    loops[pulse.toSlotIndex] = k;
                    bool found = true;
                    for(int l = 0; l < devices[rxSource].sourceCount; ++l)
                    {
                        found &= loops[l] != 0;
                    }
                    if(found)
                    {
                        goto end;
                    }
                }
                Device &device = devices[pulse.index];
                switch (device.op)
                {
                    case None:
                        break;
                    case FlipFlop:
                        if (!pulse.enable)
                        {
                            uint8_t b = device.states == 0 ? 1 : 0;
                            device.states = b;
                            sBroadcast(device, otherP, otherPC, b, lows, highs);
                        }
                        break;
                    case Conj:
                    {
                        uint16_t state = (uint16_t(pulse.enable)) << (uint16_t(pulse.toSlotIndex));
                        uint16_t turnOff = 1 << (uint16_t(pulse.toSlotIndex));
                        device.states &= ~turnOff;
                        device.states |= state;
                        uint16_t setBits = ((1 << uint16_t(device.sourceCount)) - 1);
                        if ((device.states & setBits) == setBits)
                        {
                            sBroadcast(device, otherP, otherPC, 0, lows, highs);
                        }
                        else
                        {
                            sBroadcast(device, otherP, otherPC, 1, lows, highs);
                        }
                        break;
                    }
                    case BroadCast:
                        uint8_t b = pulse.enable;
                        device.states = b;
                        sBroadcast(device, otherP, otherPC, b, lows, highs);
                        break;
                }
            }
            std::swap(currP, otherP);
            std::swap(currPC, otherPC);
        }
    }
    end:
    int64_t multi = 1;
    for(int l = 0; l < devices[rxSource].sourceCount; ++l)
    {
        multi *= loops[l];
    }
    // probably should find lcm for the numbers.
    return multi;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "20A: Pulses: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "20B: Accepted: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data20A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data20A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run20A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data20A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run20B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data20A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

