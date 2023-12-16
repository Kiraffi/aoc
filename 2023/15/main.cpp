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

#include <vector>

#include <unordered_map>
#include <unordered_set>


#include "input.cpp"

#define PROFILE 0
#include "../profile.h"

alignas(32) static constexpr char test15A[] =
    R"(rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7)";

static int64_t sParserNumber(int64_t startNumber, const char** data)
{
    int64_t number = startNumber;
    while(**data == ' ') ++*data;
    bool neg = false;
    if(**data == '-')
    {
        neg = true;
        ++*data;
    }
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return neg ? -number : number;
}

/*
static void sBitShiftRightOne(__m128i* value)
{
    __m128i movedTop = _mm_bsrli_si128(*value, 8);
    movedTop = _mm_slli_epi64(movedTop, 63);
    *value = _mm_srli_epi64(*value, 1);
    *value = _mm_or_si128(*value, movedTop);
}

static void sBitShiftLeftOne(__m128i* value)
{
    __m128i movedBot = _mm_bslli_si128(*value, 8);
    movedBot = _mm_srli_epi64(movedBot, 63);
    *value = _mm_slli_epi64(*value, 1);
    *value = _mm_or_si128(*value, movedBot);
}


static void sBitShift(__m128i* value, int dir)
{

    if(dir == -1)
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        bitShiftRightOne(value);
    }
    else if(dir == 1)
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        bitShiftLeftOne(value);
    }
}
 */

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("15A Total");
    int64_t sum = 0;
    int64_t curr = 0;
    while(*data)
    {
        if(*data == ',')
        {
            sum += curr;
            curr = 0;
        }
        else
        {
            curr += *data;
            curr *= 17;
            curr %= 256;
        }
        ++data;
    }
    return sum + curr;
}
struct StrNum
{
    const char* str;
    int strLen;
    int number;
};

static int findObject(const std::vector<StrNum> &hashMap, const char* str, int strLen)
{
    for(int index = 0; index < (int)hashMap.size(); ++index)
    {
        const StrNum& item = hashMap[index];
        if(item.strLen != strLen)
        {
            continue;
        }
        if(memcmp(str, item.str, strLen) == 0)
        {
            return index;
        }
    }
    return -1;
}


static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("15B Total");
    static const int HASHMAP_SIZE = 256;
    std::vector<StrNum> hashMaps[HASHMAP_SIZE];
    int64_t currHash = 0;

    const char* start = data;
    int strLen = 0;

    while(*data)
    {
        if(*data == '=')
        {
            ++data;
            int64_t number = sParserNumber(0, &data);
            --data;

            int index = findObject(hashMaps[currHash], start, strLen);
            if(index >= 0)
            {
                hashMaps[currHash][index].str = start;
                hashMaps[currHash][index].number = int(number);

            }
            else
            {
                hashMaps[currHash].push_back({.str = start, .strLen = strLen, .number = int(number) });
            }

        }
        else if(*data == '-')
        {
            int index = findObject(hashMaps[currHash], start, strLen);
            if(index >= 0)
            {
                hashMaps[currHash].erase(hashMaps[currHash].begin() + index);

            }
        }
        else if(*data == ',')
        {
            start = data + 1;
            currHash = 0;
            strLen = 0;
        }

        else
        {
            currHash += *data;
            currHash *= 17;
            currHash %= 256;
            strLen++;
        }
        ++data;
    }
    int64_t sum = 0;

    for(int i = 0; i < HASHMAP_SIZE; ++i)
    {
        for(int j = 0; j < (int)hashMaps[i].size(); ++j)
        {
            sum += (i + 1) * (j + 1) * hashMaps[i][j].number;
        }
    }
    
    return sum;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "15A: Hash: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "15B: Focusing power: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data15A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data15A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run15A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data15A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run15B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data15A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

