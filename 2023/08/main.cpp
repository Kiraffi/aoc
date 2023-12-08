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

alignas(16) static constexpr char test08A[] =
    R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ)
)";

alignas(16) static constexpr char test08B[] =
    R"(LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ)
)";

alignas(16) static constexpr char test08C[] =
    R"(LR

AAA = (AAB, XXX)
AAB = (XXX, AAZ)
AAZ = (AAB, XXX)
BBA = (BBB, XXX)
BBB = (BBC, BBC)
BBC = (BBZ, BBZ)
BBZ = (BBB, BBB)
XXX = (XXX, XXX)
)";


static int sGetChar(char c)
{
    return c - 'A';
}

/*
static int64_t sParserNumber(int64_t startNumber, const char** data)
{
    int64_t number = startNumber;
    while(**data == ' ') ++*data;
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return number;
}
*/

int sFindPrimes(int64_t value, int64_t* primeOut)
{
    int primeCount = 0;
    while((value & 1) == 0)
    {
        primeOut[primeCount++] = 2;
        value >>= 1;
    }

    for(int64_t i = 3; i < 65536; i += 2)
    {
        while((value / i) * i == value)
        {
            value /= i;
            primeOut[primeCount++] = i;
            if(value < i)
                return primeCount;
        }
    }
    return primeCount;
}

static uint32_t table[65536] = {};

static int64_t sParse08A(const char* data)
{
    const char* start = data;
    const char* end = data;
    while(*end != '\n') end++;
    data = end + 2;

    while(*data)
    {
        uint16_t index = sGetChar(*data++);
        index += sGetChar(*data++) << 5;
        index += sGetChar(*data++) << 10;

        data += 4;
        uint32_t left = sGetChar(*data++);
        left += sGetChar(*data++) << 5;
        left += sGetChar(*data++) << 10;

        data += 2;
        uint32_t right = sGetChar(*data++);
        right += sGetChar(*data++) << 5;
        right += sGetChar(*data++) << 10;

        right <<= 16;

        table[index] = left | right;
        data += 2;
    }
    static const int endPoint = 25 + (25 << 5) + (25 << 10);
    int index = 0;
    int64_t steps = 0;
    data = start;
    while(index != endPoint)
    {
        if(*data == 'L')
        {
            index = table[index] & 0xffff;
        }
        else
        {
            index = table[index] >> 16;
        }
        steps++;
        data++;
        if(data == end)
            data = start;
    }


    return steps;
}

static int64_t sParse08B(const char* data)
{
    const char* start = data;
    const char* end = data;
    while(*end != '\n') end++;
    data = end + 2;

    int nodesEndingWithA = 0;

    int starts[32] = {};
    static constexpr int Z = 0b11001 << 10;

    while(*data)
    {
        bool isA = false;
        uint16_t index = sGetChar(*data++);
        index += sGetChar(*data++) << 5;
        if(*data == 'A')
            isA = true;
        index += sGetChar(*data++) << 10;
        if(isA)
            starts[nodesEndingWithA++] = index;

        data += 4;
        uint32_t left = sGetChar(*data++);
        left += sGetChar(*data++) << 5;
        left += sGetChar(*data++) << 10;

        data += 2;
        uint32_t right = sGetChar(*data++);
        right += sGetChar(*data++) << 5;
        right += sGetChar(*data++) << 10;

        right <<= 16;

        table[index] = left | right;
        data += 2;
    }


    int64_t steps = 0;
    data = start;
    // Looks like the first index is same as loop index for my input.
    int64_t loops[8] = {};
    int indicesFound = 0;
    while(true)
    {
        steps++;
        if(*data == 'L')
        {
            for(int i = 0; i < nodesEndingWithA; ++i)
            {
                starts[i] = table[starts[i]] & 0xffff;
                if((starts[i] & Z ) == Z)
                {
                    loops[i] = steps;
                    indicesFound |= 1 << i;
                }
            }
        }
        else
        {
            for(int i = 0; i < nodesEndingWithA; ++i)
            {
                starts[i] = table[starts[i]] >> 16;
                if((starts[i] & Z ) == Z)
                {
                    loops[i] = steps;
                    indicesFound |= 1 << i;
                }
            }
        }
        data++;
        if(data == end)
            data = start;
        if(indicesFound == ((1 << nodesEndingWithA) - 1))
            break;
    }

    int primeCounts[8] = {};
    int64_t primes[8][8] = {};

    int mergedPrimeCounts = 0;
    int64_t mergedPrimes[32] = {};


    for(int i = 0; i < nodesEndingWithA; ++i)
    {
        int64_t *currentPrimes = primes[i];
        int& currentPrimeCount = primeCounts[i];
        currentPrimeCount = sFindPrimes(loops[i], currentPrimes);
        assert(currentPrimeCount < 8);

        int primeMask = 0;

        for(int j = 0; j < currentPrimeCount; ++j)
        {
            bool found = false;
            for(int i2 = 0; i2 < mergedPrimeCounts; i2++)
            {
                if(((primeMask >> i2) & 1) == 1)
                    continue;
                if(mergedPrimes[i2] == currentPrimes[j])
                {
                    found = true;
                    primeMask |= 1 << i2;
                    break;
                }
            }
            if(!found)
            {
                mergedPrimes[mergedPrimeCounts++] = currentPrimes[j];
            }
        }
    }


    int64_t leastCommonMultiple = 1;
    for(int i = 0; i < mergedPrimeCounts; ++i)
    {
        leastCommonMultiple *= mergedPrimes[i];
    }

    return leastCommonMultiple;
}

#ifndef RUNNER
int main()
{
    printf("8A: Steps: %" PRIi64 "\n", sParse08A(data08A));
    printf("8B: Steps: %" PRIi64 "\n", sParse08B(data08A));
    return 0;
}
#endif

int run08A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse08A(data08A);

    if(printOut)
        charsAdded = sprintf(buffer, "8A: Steps: %" PRIi64, aResult);
    return charsAdded;
}

int run08B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse08B(data08A);

    if(printOut)
        charsAdded = sprintf(buffer, "8B: Steps: %" PRIi64, resultB);

    return charsAdded;
}

