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

static const int Amount = 2048;

alignas(32) static constexpr char test25A[] =
    R"(jqt: rhn xhk nvd
rsh: frs pzl lsr
xhk: hfx
cmg: qnr nvd lhk bvb
rhn: xhk bvb hfx
bvb: xhk hfx
pzl: lsr hfx nvd
qnr: nvd
ntq: jqt hfx bvb xhk
nvd: lhk
lsr: lhk
rzs: qnr cmg lsr rsh
frs: qnr lhk lsr
)";

std::string sParseString(const char** ptr)
{
    const char* data = *ptr;
    while(isalpha(*data))data++;
    std::string s(*ptr, data - *ptr);
    *ptr = data;
    return s;
}

int16_t sGetIndexFromString(std::unordered_map<std::string, int16_t>& strings, const std::string& findString)
{
    auto iter =  strings.find(findString);
    int16_t result = 0;
    if(iter == strings.end())
    {
        result = int16_t(strings.size());
        strings[findString] = result;
    }
    else
    {
        result = iter->second;
    }
    return result;
}

int sFillMap(
    int16_t startIndex,
    const int16_t* connections,
    const uint8_t* connectionCount,
    uint8_t* visited,
    int16_t* connectionVisit)
{
    int lastIndex = -1;
    struct Day25State
    {
        int16_t currIndex;
        int16_t oldIndex;
    };

    std::vector<Day25State> states1;
    std::vector<Day25State> states2;

    states1.push_back({.currIndex = startIndex, .oldIndex = -1});
    while(!states1.empty())
    {
        states2.clear();
        for(const Day25State& s : states1)
        {
            if (visited[s.currIndex])
                continue;
            visited[s.currIndex] = 1;
            if (s.oldIndex != -1)
            {
                auto fn = [&](int index1, int index2) {
                    for (int i = 0; i < connectionCount[index1]; ++i)
                    {
                        if (connections[index1 * 8 + i] == index2)
                        {
                            connectionVisit[index1 * 8 + i] += 1;
                            break;
                        }
                    }
                };
                fn(s.currIndex, s.oldIndex);
                fn(s.oldIndex, s.currIndex);
            }

            for (uint16_t i = 0; i < connectionCount[s.currIndex]; ++i)
            {
                lastIndex = connections[s.currIndex * 8 + i];
                states2.push_back(Day25State{.currIndex = int16_t(lastIndex), .oldIndex = s.currIndex});
            }
        }
        std::swap(states1, states2);
    }
    return lastIndex;
}


static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("25A Total");
    std::unordered_map<std::string, int16_t> stringMap;
    std::string strings[Amount];

    int16_t connections[8 * Amount] = {};
    uint8_t connectionAmounts[Amount] = {};

    while(*data)
    {
        std::string s1 = sParseString(&data);
        int oldSize1 = int(stringMap.size());
        int32_t index1 = sGetIndexFromString(stringMap, s1);
        if(oldSize1 == index1)
            strings[index1] = s1;
        uint8_t& indexCount1 = connectionAmounts[index1];
        data++;
        while(*data != '\n')
        {
            data++;
            std::string s2 = sParseString(&data);
            int oldSize2 = int(stringMap.size());
            int32_t index2 = sGetIndexFromString(stringMap, s2);
            if(oldSize2 == index2)
                strings[index2] = s2;

            uint8_t& indexCount2 = connectionAmounts[index2];
            connections[index1 * 8 + indexCount1] = int16_t(index2);
            connections[index2 * 8 + indexCount2] = int16_t(index1);
            ++indexCount1;
            ++indexCount2;
        }
        data++;
    }
    srand(0x01234567);

    int16_t connectionVisit[Amount * 8] = {};
    for(int j = 0; j < 3; ++j)
    {
        int amount = j == 0 ? 5 : 3;
        for (int k = 0; k < amount; ++k)
        {
            int16_t r = rand() % (stringMap.size());
            for (int i = 0; i < 3; ++i)
            {
                uint8_t visited[Amount] = {};
                r = sFillMap(r, connections, connectionAmounts, visited, connectionVisit);
            }
        }

        int topIndex = 0;
        int topCount = connectionAmounts[0];

        struct Connections
        {
            int index;
            int amount;
        };

        //Connections conns[Amount * 8] = {};
        for(int i = 8; i < (int)stringMap.size() * 8; ++i)
        {
            //conns[i] = {.index = i, .amount = connectionVisit[i]};

            if(connectionVisit[i] > topCount)
            {
                topIndex = i;
                topCount = connectionVisit[i];
            }

        }
/*
        std::sort(conns, conns + Amount * 8, [](const Connections& a, const Connections& b){
            return a.amount > b.amount;
        });
*/
        auto remove = [&](int topIndex) {

            int index1 = topIndex / 8;
            int index2 = connections[topIndex];

            connectionAmounts[index1]--;
            std::swap(connections[topIndex], connections[index1 * 8 + connectionAmounts[index1]]);

            for (int i = 0; i < connectionAmounts[index2]; ++i)
            {
                if (connections[index2 * 8 + i] == index1)
                {
                    connectionAmounts[index2]--;
                    std::swap(connections[index2 * 8 + i], connections[index2 * 8 + connectionAmounts[index2]]);
                    break;
                }
            }
        };
        //printf("conns0: %i\n", conns[0].index);
        //printf("conns1: %i\n", conns[1].index);
        //printf("conns2: %i\n", conns[2].index);
        remove(topIndex);
        //remove(conns[0].index);
        //remove(conns[1].index);
        //remove(conns[2].index);
    }
    int64_t result = 0;
    {
        int r = 0;

        int16_t connectionVisit[Amount * 8] = {};
        uint8_t visited[Amount] = {};
        sFillMap(r, connections, connectionAmounts, visited, connectionVisit);

        for(int i = 0; i < int(stringMap.size()); ++i)
        {
            if(visited[i])
                result++;
        }

        int strSize = int(stringMap.size());
        result *= strSize - result;
    }
    return result;

}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "25A: Result: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data25A));
    printf("%s\n", printBuffer);
}
#endif

int run25A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data25A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}
