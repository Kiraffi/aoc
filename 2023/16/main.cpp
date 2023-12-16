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

#define PROFILE 1
#include "../profile.h"

alignas(32) static constexpr char test16A[] =
    R"(>|<<<\....
|v-.\^....
.v...|->>>
.v...v^.|.
.v...v^...
.v...v^..\
.v../2\\..
<->-/vv|..
.|<<<2-|.\
.v//.|.v..
)";

enum Dir
{
    None = 0,
    Right = 1,
    Left = 2,
    Up = 4,
    Down = 8,
};

/*
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
*/

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

static int sGetX(int fromHash)
{
    return fromHash & 255;
}
static int sGetY(int fromHash)
{
    return (fromHash >> 8) & 255;
}
static int sGetDir(int fromHash)
{
    return (fromHash >> 16) & 255;
}

/*
static int sSetX(int toHash, int x)
{
    toHash &= ~255;
    toHash |= x;
    return toHash;
}

static int sSetY(int toHash, int x)
{
    toHash &= ~(255 << 8);
    toHash |= x << 8;
    return toHash;
}
*/
static int sSetDir(int toHash, int x)
{
    toHash &= ~(255 << 16);
    toHash |= x << 16;
    return toHash;
}

static int sSetPosDir(int x, int y, int dir)
{
    return x | (y << 8) | (dir << 16);
}

static void sGetMapSize(const char* data, int& width, int& height)
{
    width = 0;
    height = 0;

    while(*data != '\n')
    {
        ++data;
        ++width;
    }
    data++;
    while(*data)
    {
        data += width + 1;
        ++height;
    }
    ++height;

}

static int64_t sCalculateEnergy(const char* data, int hashStart)
{
    //TIMEDSCOPE("sCalculateEnergy");

    static const int MAX_POSITIONS = 128;
    // Using visitedMap array vs hashset seems to be over 20x faster. 200ms -> 7ms
    alignas (16)uint8_t visitedMap[128 * 128]= {};
    // Using constant size stack allocated array saves also a bit of time compared to std::vector, although
    // if it was static it probably would not make a difference.
    int32_t positions[MAX_POSITIONS] = {};

    int64_t energy = 0;
    int positionCount = 1;
    int width = 0;
    int height = 0;

    sGetMapSize(data, width, height);
    positions[0] = hashStart;

    while(positionCount > 0)
    {
        int pos = positions[0];
        int x = sGetX(pos);
        int y = sGetY(pos);
        int dir = sGetDir(pos);

        while(true)
        {
            int index = y * 128 + x;
            if(visitedMap[index] & dir)
            {
                break;
            }
            visitedMap[index] |= dir;
            char c = data[y * (width + 1) + x];
            switch(c)
            {
                case '/':
                    switch(dir)
                    {
                        case Right: dir = Up; break;
                        case Left: dir = Down; break;
                        case Up: dir = Right; break;
                        case Down: dir = Left; break;
                    }
                    break;
                case '\\':
                    switch(dir)
                    {
                        case Right: dir = Down; break;
                        case Left: dir = Up; break;
                        case Up: dir = Left; break;
                        case Down: dir = Right; break;
                    }
                    break;
                case '-':
                    if(dir & (Up | Down))
                    {
                        positions[positionCount++] = sSetDir(pos, Left);
                        assert(positionCount < MAX_POSITIONS);
                        dir = Right;
                    }
                    break;
                case '|':
                    if(dir & (Left | Right))
                    {
                        positions[positionCount++] = sSetDir(pos, Up);
                        assert(positionCount < MAX_POSITIONS);
                        dir = Down;
                    }
                    break;
            }

            switch(dir)
            {
                case Right: x++; break;
                case Left: x--; break;
                case Up: y--; break;
                case Down: y++; break;
            }
            if(x < 0 || y < 0 || x >= width || y >= height)
            {
                break;
            }
            pos = sSetPosDir(x, y, dir);

        }
        positions[0] = positions[positionCount - 1];
        positionCount--;
    }
    {
        //TIMEDSCOPE("Calculate sum");
#if 1 // Use simd for summing ~6ms -> ~4.8ms. Each iteration goes from ~4us to 2us
        __m128i ones8 = _mm_set1_epi8(1);
        __m128i ones16 = _mm_set1_epi16(3);

        const __m128i* ptr = (const __m128i*) visitedMap;

        __m128i sumValues = _mm_setzero_si128();
        for(int i = 0; i < 128 * 128 / 16; i += 128)
        {
            for (int j = 0; j < 128; ++j)
            {
                // Since we have based on direction bits set, can be more than 1,
                // we need to reduct the bits, doing bitshift right by 2 and or them,
                // then bitshift right by 1 and or them.
                // Then to reduce storing the values, we sum them up to make them 16bit values.
                __m128i values = _mm_loadu_si128(ptr);
                __m128i values1 = _mm_srli_epi16(values, 2);
                values = _mm_or_si128(values, values1);
                __m128i values2 = _mm_srli_epi16(values, 1);
                values = _mm_or_si128(values, values2);
                values = _mm_and_si128(values, ones8);

                __m128i values3 = _mm_srli_epi16(values, 8);
                values = _mm_add_epi16(values, values3);
                values = _mm_and_si128(values, ones16);

                sumValues = _mm_add_epi16(values, sumValues);
                ++ptr;
            }
        }
        alignas(16) uint16_t storeValues[8] = {};
        _mm_storeu_si128((__m128i*)storeValues, sumValues);
        for(uint16_t v : storeValues)
            energy += v;
#else
        for (uint8_t v: visitedMap)
            energy += v ? 1 : 0;
#endif
    }

    return energy;

}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("16A Total");
    return sCalculateEnergy(data, sSetDir(0, Right));
}

template <typename T>
static T sGetMax(T a, T b)
{
    return a < b ? b : a;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("16B Total");
    int64_t maxEnergy = 0;
    int width = 0;
    int height = 0;
    sGetMapSize(data, width, height);

    for(int x = 0; x < width; ++x)
    {
        maxEnergy = sGetMax(maxEnergy, sCalculateEnergy(data, sSetPosDir(x, 0, Down)));
        maxEnergy = sGetMax(maxEnergy, sCalculateEnergy(data, sSetPosDir(x, height - 1, Up)));
    }
    for(int y = 0; y < height; ++y)
    {
        maxEnergy = sGetMax(maxEnergy, sCalculateEnergy(data, sSetPosDir(0, y, Right)));
        maxEnergy = sGetMax(maxEnergy, sCalculateEnergy(data, sSetPosDir(width - 1, y, Left)));
    }


    return maxEnergy;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "16A: Energy: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "16B: Energy: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data16A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data16A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run16A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data16A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run16B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data16A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

