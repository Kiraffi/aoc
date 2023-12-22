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

template <typename T>
static void sMemset(T* arr, T value, int amount)
{
    const T* end = arr + amount;
    while(arr < end)
    {
        *arr++ = value;
    }

}

static __m128i sWriteMin128(__m128i newValue, uint16_t* map, int offset)
{
    __m128i_u* address = (__m128i*) (map + offset);
    __m128i out = _mm_loadu_si128(address);
    __m128i prev = out;
    out = _mm_min_epu16(newValue, out);
    _mm_storeu_si128(address, out);
    return prev = _mm_xor_si128(prev, out);
}


static __m256i sWriteMin256(__m256i newValue, uint16_t* map, int offset)
{
    __m256i_u* address = (__m256i*) (map + offset);
    __m256i out = _mm256_loadu_si256(address);
    __m256i prev = out;
    out = _mm256_min_epu16(newValue, out);
    _mm256_storeu_si256(address, out);
    return prev = _mm256_xor_si256(prev, out);
}

static bool sUpdateDir(const uint16_t* numberMap,
    const uint16_t* sourceMap, // upDown map
    uint8_t* changedRowsSource,
    uint16_t* destMap1, // left right map
    uint8_t* changedRowsDst1,
    int width,
    int height,
    int xDirection,
    int yDirection,
    int minimumSameDir,
    int maximumSameDir)
{
    //TIMEDSCOPE("17 Timed scope up down");

    __m256i changed = _mm256_setzero_si256();
    for(int y = 0; y < height; ++y)
    {
        if((changedRowsSource[y / 8] >> (y % 8)) == 0)
        {
            continue;
        }
        int x = Padding;
        __m256i changed1[16] = {};

        while(x < width + Padding)
        {
            int offset = y * MaxWidthU16 + x;
            __m256i values = _mm256_loadu_si256((const __m256i *) (sourceMap + offset));
            int moves = 1;
            while (moves <= maximumSameDir)
            {
                if((y + moves * yDirection < 0) | (y + moves * yDirection >= height))
                {
                    break;
                }
                int loopOffset = offset + moves * (xDirection + yDirection * MaxWidthU16);
                __m256i numbers = _mm256_loadu_si256((const __m256i*) (numberMap + loopOffset));
                values = _mm256_adds_epu16(values, numbers);
                if (moves >= minimumSameDir)
                {
                    changed1[moves] = _mm256_or_si256(sWriteMin256(values, destMap1, loopOffset),
                        changed1[moves]);
                }
                moves++;
            }
            x += 16;
        }
        for (int i = 0; i < maximumSameDir; ++i)
        {
            int index = y + (i + 1) * yDirection;
            if (!(_mm256_testz_si256(changed1[i], changed1[i])))
            {
                changedRowsDst1[(index) / 8] |= 1 << ((index) % 8);
            }
        }
        for (int i = 0; i < 8; ++i)
        {
            changed1[i] = _mm256_or_si256(changed1[2 * i], changed1[2 * i + 1]);
        }

        for (int i = 0; i < 4; ++i)
        {
            changed1[i] = _mm256_or_si256(changed1[2 * i], changed1[2 * i + 1]);
        }

        for (int i = 0; i < 2; ++i)
        {
            changed1[i] = _mm256_or_si256(changed1[2 * i], changed1[2 * i + 1]);
        }

        changed1[0] = _mm256_or_si256(changed1[0], changed1[1]);
        changed = _mm256_or_si256(changed, changed1[0]);
    }
    return !_mm256_testz_si256(changed, changed);
}


 bool sUpdateDir2(const uint16_t* numberMap,
    const uint16_t* sourceMap, // left right map
    uint8_t* changedRowsSource,
    uint16_t* destMap1, // up down map
    uint8_t* changedRowsDst1,
    int width,
    int height,
    int xDirection,
    int minimumSameDir,
    int maximumSameDir)
{
    //TIMEDSCOPE("17 Timed scope left right");

    __m128i changed = _mm_setzero_si128();
    __m128i HighBits = _mm_set_epi32(0xffff0000, 0, 0, 0);
    __m128i LowBits = _mm_set_epi32(0, 0, 0, 0x0000ffff);

    for(int y = 0; y < height; ++y)
    {
        if((changedRowsSource[y / 8] >> (y % 8)) == 0)
        {
            continue;
        }
        int x = 0;
        __m128i changed1 = _mm_setzero_si128();

        __m128i writeValue1 = ~_mm_setzero_si128();
        __m128i writeValue2 = ~_mm_setzero_si128();
        __m128i writeValue3 = ~_mm_setzero_si128();
        __m128i writeValue4 = ~_mm_setzero_si128();
        __m128i writeValue5 = ~_mm_setzero_si128();
        __m128i writeValue6 = ~_mm_setzero_si128();

        while(x < width)
        {

            int offset = y * MaxWidthU16;
            if(xDirection > 0)
            {
                offset += x + Padding;
            }
            else
            {
                offset += (Padding + width - x - 1) / 32 * 32;
                //offset += Padding + width - x;
            }
            __m128i v1 = _mm_loadu_si128((const __m128i *) (sourceMap + offset + 0 * xDirection * 8));
            __m128i v2 = _mm_loadu_si128((const __m128i *) (sourceMap + offset + 1 * xDirection * 8));
            __m128i v3 = _mm_loadu_si128((const __m128i *) (sourceMap + offset + 2 * xDirection * 8));
            __m128i v4 = _mm_loadu_si128((const __m128i *) (sourceMap + offset + 3 * xDirection * 8));
            __m128i v5 = ~_mm_setzero_si128();
            __m128i v6 = ~_mm_setzero_si128();

            __m128i numbers1 = _mm_loadu_si128((const __m128i *) (numberMap + offset + 0 * xDirection * 8));
            __m128i numbers2 = _mm_loadu_si128((const __m128i *) (numberMap + offset + 1 * xDirection * 8));
            __m128i numbers3 = _mm_loadu_si128((const __m128i *) (numberMap + offset + 2 * xDirection * 8));
            __m128i numbers4 = _mm_loadu_si128((const __m128i *) (numberMap + offset + 3 * xDirection * 8));
            __m128i numbers5 = _mm_loadu_si128((const __m128i *) (numberMap + offset + 4 * xDirection * 8));
            __m128i numbers6 = _mm_loadu_si128((const __m128i *) (numberMap + offset + 5 * xDirection * 8));

            if(xDirection > 0 )
            {
                for (int i = 1; i <= maximumSameDir; ++i)
                {
                    v6 = _mm_or_si128(_mm_slli_si128(v6, 2), _mm_srli_si128(v5, 14));
                    v5 = _mm_or_si128(_mm_slli_si128(v5, 2), _mm_srli_si128(v4, 14));
                    v4 = _mm_or_si128(_mm_slli_si128(v4, 2), _mm_srli_si128(v3, 14));
                    v3 = _mm_or_si128(_mm_slli_si128(v3, 2), _mm_srli_si128(v2, 14));
                    v2 = _mm_or_si128(_mm_slli_si128(v2, 2), _mm_srli_si128(v1, 14));
                    v1 = _mm_slli_si128(v1, 2);

                    v1 = _mm_adds_epu16(numbers1, v1);
                    v2 = _mm_adds_epu16(numbers2, v2);
                    v3 = _mm_adds_epu16(numbers3, v3);
                    v4 = _mm_adds_epu16(numbers4, v4);
                    v5 = _mm_adds_epu16(numbers5, v5);
                    v6 = _mm_adds_epu16(numbers6, v6);

                    v1 = _mm_or_si128(LowBits, v1);

                    if (i >= minimumSameDir)
                    {
                        writeValue1 = _mm_min_epu16(writeValue1, v1);
                        writeValue2 = _mm_min_epu16(writeValue2, v2);
                        writeValue3 = _mm_min_epu16(writeValue3, v3);
                        writeValue4 = _mm_min_epu16(writeValue4, v4);
                        writeValue5 = _mm_min_epu16(writeValue5, v5);
                        writeValue6 = _mm_min_epu16(writeValue6, v6);
                    }
                }
            }
            else
            {
                for (int i = 1; i <= maximumSameDir; ++i)
                {
                    v6 = _mm_or_si128(_mm_srli_si128(v6, 2), _mm_slli_si128(v5, 14));
                    v5 = _mm_or_si128(_mm_srli_si128(v5, 2), _mm_slli_si128(v4, 14));
                    v4 = _mm_or_si128(_mm_srli_si128(v4, 2), _mm_slli_si128(v3, 14));
                    v3 = _mm_or_si128(_mm_srli_si128(v3, 2), _mm_slli_si128(v2, 14));
                    v2 = _mm_or_si128(_mm_srli_si128(v2, 2), _mm_slli_si128(v1, 14));
                    v1 = _mm_srli_si128(v1, 2);

                    v1 = _mm_adds_epu16(numbers1, v1);
                    v2 = _mm_adds_epu16(numbers2, v2);
                    v3 = _mm_adds_epu16(numbers3, v3);
                    v4 = _mm_adds_epu16(numbers4, v4);
                    v5 = _mm_adds_epu16(numbers5, v5);
                    v6 = _mm_adds_epu16(numbers6, v6);

                    v1 = _mm_or_si128(HighBits, v1);

                    if (i >= minimumSameDir)
                    {
                        writeValue1 = _mm_min_epu16(writeValue1, v1);
                        writeValue2 = _mm_min_epu16(writeValue2, v2);
                        writeValue3 = _mm_min_epu16(writeValue3, v3);
                        writeValue4 = _mm_min_epu16(writeValue4, v4);
                        writeValue5 = _mm_min_epu16(writeValue5, v5);
                        writeValue6 = _mm_min_epu16(writeValue6, v6);
                    }
                }
            }

            changed1 = _mm_or_si128(sWriteMin128(writeValue1, destMap1, offset + 8 * 0 * xDirection), changed1);
            changed1 = _mm_or_si128(sWriteMin128(writeValue2, destMap1, offset + 8 * 1 * xDirection), changed1);
            changed1 = _mm_or_si128(sWriteMin128(writeValue3, destMap1, offset + 8 * 2 * xDirection), changed1);
            changed1 = _mm_or_si128(sWriteMin128(writeValue4, destMap1, offset + 8 * 3 * xDirection), changed1);

            writeValue1 = writeValue5;
            writeValue2 = writeValue6;
            writeValue3 = ~_mm_setzero_si128();
            writeValue4 = ~_mm_setzero_si128();
            writeValue5 = ~_mm_setzero_si128();
            writeValue6 = ~_mm_setzero_si128();
            x += 32;
        }

        {
            if ((!_mm_test_all_ones(~changed1)))
            {
                changedRowsDst1[(y) / 8] |= 1 << ((y) % 8);
            }
        }
        changed = _mm_or_si128(changed, changed1);
    }
    return !_mm_test_all_ones(~changed);
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

    alignas(32) uint16_t leftRightMap[MaxWidthU16 * MaxHeight] = {};
    alignas(32) uint16_t upDownMap[MaxWidthU16 * MaxHeight] = {};

    alignas(32) uint8_t changedRowsLeftRight[32] = {};
    alignas(32) uint8_t changedRowsUpDown[32] = {};

    sMemset(leftRightMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);
    sMemset(upDownMap, uint16_t(1 << 14), MaxWidthU16 * MaxHeight);
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
    leftRightMap[Padding] = 0;
    upDownMap[Padding] = 0;

    changedRowsLeftRight[0] = 1;
    changedRowsUpDown[0] = 1;

    uint16_t lowest = ~0;
    {
        TIMEDSCOPE("17 Update maps");
        bool changed = true;

        while (changed)
        {
            changed = false;
            changed |= sUpdateDir2(numberMap, leftRightMap, changedRowsLeftRight,
                upDownMap, changedRowsUpDown,
                width, height, 1, minimumSameDir,
                maximumSameDir);

            changed |= sUpdateDir2(numberMap, leftRightMap, changedRowsLeftRight,
                upDownMap,  changedRowsUpDown,
                width, height, -1, minimumSameDir,
                maximumSameDir);
            memset(changedRowsLeftRight, 0, 32);

            changed |= sUpdateDir(numberMap, upDownMap, changedRowsUpDown,
                leftRightMap, changedRowsLeftRight,
                width, height, 0, -1, minimumSameDir,
                maximumSameDir);

            changed |= sUpdateDir(numberMap, upDownMap, changedRowsUpDown,
                leftRightMap, changedRowsLeftRight,
                width, height, 0, 1, minimumSameDir,
                maximumSameDir);
            memset(changedRowsUpDown, 0, 32);

        }
        lowest = leftRightMap[Padding + width - 1 + (height - 1) * MaxWidthU16];
        lowest = sMin(lowest, upDownMap[Padding + width - 1 + (height - 1) * MaxWidthU16]);
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

