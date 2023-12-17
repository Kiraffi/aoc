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

// 141 + 16 + 16 u16 rounded up by 32 = 320 bytes
static constexpr int Padding = 16;
static_assert((Padding % 16) == 0);
static constexpr int MaxWidthBytes = (((141 + Padding) * 2) + 31) / 32 * 32;
static constexpr int MaxWidthU16 = MaxWidthBytes / 2;
static constexpr int MaxHeight = 160;

#define PROFILE 0
#include "../profile.h"

alignas(32) static constexpr char test17A[] =
    R"(2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533
)";

alignas(32) static constexpr char test17B[] =
    R"(111111111111
999999999991
999999999991
999999999991
999999999991
)";
template <typename T>
static T sMax(T a, T b)
{
    return a < b ? b : a;
}

template <typename T>
static T sMin(T a, T b)
{
    return a < b ? a : b;
}


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

static void sGetSize(const char* data, int& width, int& height)
{
    width = 0;
    height = 0;
    while(*data++ != '\n') width++;

    ++height;
    while(*data != '\0')
    {
        ++height;
        data += width + 1;
    }
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

template <typename T>
static void sMemset(T* arr, T value, int amount)
{
    const T* end = arr + amount;
    while(arr < end)
    {
        *arr++ = value;
    }
}

static __m128i sWriteMin(__m128i newValue, uint16_t* map, int offset)
{
    __m128i_u* address = (__m128i*) (map + offset);
    __m128i out = _mm_loadu_si128(address);
    __m128i prev = out;
    out = _mm_min_epu16(newValue, out);
    _mm_storeu_si128(address, out);
    return prev = _mm_xor_si128(prev, out);
}

__m128i sUpdateDir(const uint16_t* numberMap,
    const uint16_t* sourceMap, // can be left, right, up or down. The dest are always the others
    uint16_t* destMap1, // basically left map if sourceMap is up or down
    uint16_t* destMap2, // basically right map if sourceMap is up or down
    int width,
    int height,
    int xDirection,
    int yDirection,
    int minimumSameDir,
    int maximumSameDir)
{
    __m128i changed = _mm_setzero_si128();
    for(int y = 0; y < height; ++y)
    {
        int x = Padding;
        while(x < width + Padding)
        {
            int offset = y * MaxWidthU16 + x;
            __m128i values = _mm_loadu_si128((const __m128i *) (sourceMap + offset));
            int moves = 1;
            while (moves <= maximumSameDir)
            {
                if((y + moves * yDirection < 0) | (y + moves * yDirection >= height))
                {
                    break;
                }
                int loopOffset = offset + moves * (xDirection + yDirection * MaxWidthU16);
                __m128i numbers = _mm_loadu_si128((const __m128i*) (numberMap + loopOffset));
                values = _mm_adds_epu16(values, numbers);
                if (moves >= minimumSameDir)
                {
                    changed = _mm_or_si128(sWriteMin(values, destMap2, loopOffset), changed);
                    changed = _mm_or_si128(sWriteMin(values, destMap1, loopOffset), changed);
                }
                moves++;
            }
            x += 8;
        }
    }
    return changed;
}

static int64_t sGetMinimumEnergy(const char* data, int minimumSameDir, int maximumSameDir)
{
    int width = 0;
    int height = 0;

    sGetSize(data, width, height);

    assert(minimumSameDir <= maximumSameDir);
    assert(maximumSameDir < Padding);
    assert(width <= MaxWidthU16);
    assert(height <= MaxHeight);

    alignas(32) uint16_t numberMap[MaxWidthU16 * MaxHeight] = {};

    alignas(32) uint16_t leftMap[MaxWidthU16 * MaxHeight] = {};
    alignas(32) uint16_t rightMap[MaxWidthU16 * MaxHeight] = {};
    alignas(32) uint16_t upMap[MaxWidthU16 * MaxHeight] = {};
    alignas(32) uint16_t downMap[MaxWidthU16 * MaxHeight] = {};

    sMemset(leftMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);
    sMemset(rightMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);
    sMemset(upMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);
    sMemset(downMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);
    sMemset(numberMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);

    {
        const char* tmp = data;
        int index = Padding;
        while(*tmp)
        {
            if(*tmp != '\n')
            {
                numberMap[index++] = *tmp - '0';
            }
            else
            {
                index /= MaxWidthU16;
                index = (index + 1) * MaxWidthU16 + Padding;
            }
            tmp++;
        }
    }
    leftMap[Padding] = 0;
    rightMap[Padding] = 0;
    upMap[Padding] = 0;
    downMap[Padding] = 0;

    uint16_t lowest = ~0;
    {
        TIMEDSCOPE("17 Update maps");
        __m128i changed = _mm_set1_epi32(1);

        while (!(_mm_test_all_ones(~changed)))
        {
            changed = _mm_setzero_si128();
            changed = _mm_or_si128(sUpdateDir(numberMap, rightMap, upMap, downMap, width, height, 1, 0, minimumSameDir,
                maximumSameDir), changed);
            changed = _mm_or_si128(sUpdateDir(numberMap, leftMap, upMap, downMap, width, height, -1, 0, minimumSameDir,
                maximumSameDir), changed);

            changed = _mm_or_si128(sUpdateDir(numberMap, upMap, leftMap, rightMap, width, height, 0, -1, minimumSameDir,
                maximumSameDir), changed);
            changed = _mm_or_si128(sUpdateDir(numberMap, downMap, leftMap, rightMap, width, height, 0, 1, minimumSameDir,
                    maximumSameDir), changed);

        }
        lowest = sMin(lowest, leftMap[Padding + width - 1 + (height - 1) * MaxWidthU16]);
        lowest = sMin(lowest, rightMap[Padding + width - 1 + (height - 1) * MaxWidthU16]);
        lowest = sMin(lowest, upMap[Padding + width - 1 + (height - 1) * MaxWidthU16]);
        lowest = sMin(lowest, downMap[Padding + width - 1 + (height - 1) * MaxWidthU16]);
    }

    return lowest;
}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("17A Total");
    return sGetMinimumEnergy(data, 1, 3);
}
static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("17B Total");
    return sGetMinimumEnergy(data, 4, 10);
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "17A: Minimum energy: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "17B: Minimum energy: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data17A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data17A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run17A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data17A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run17B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data17A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

