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
static constexpr int Padding = 0; //16;
static constexpr int RowOffset = 0;
static_assert((Padding % 16) == 0);
static constexpr int RoundUpWidth = 32;
static constexpr int MaxWidthBytes = (((141 + Padding) * 2) + RoundUpWidth - 1) / RoundUpWidth * RoundUpWidth;
static constexpr int MaxWidthU16 = MaxWidthBytes / 2;
static constexpr int MaxHeight = 144;

#define LoadU2(Addr1, Addr2) _mm256_loadu2_m128i((const __m128i *)(Addr1), (const __m128i *)(Addr2))
#define StoreU2(Addr1, Addr2, ValueOut) _mm256_storeu2_m128i((__m128i *)(Addr1), (__m128i *)(Addr2), ValueOut)

#define PROFILE 1
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

//template <typename T>
static void sMemset(uint16_t* arr, uint16_t value, int amount)
{
    __m256i setValue = _mm256_set1_epi16(value);
    __m256i* start = (__m256i*)(arr);
    const __m256i* end = (const __m256i*)(arr + amount);
    while(start < end)
    {
        _mm256_storeu_si256(start, setValue);
        start++;
    }

}

template<int Amount>
static __m128i sByteShiftRight128(__m128i value)
{
    static_assert(Amount > 0 && Amount < 16);
    return _mm_bsrli_si128(value, Amount);
}

template<int Amount>
static __m128i sByteShiftLeft128(__m128i value)
{
    static_assert(Amount > 0 && Amount < 16);
    return _mm_bslli_si128(value, Amount);
}



[[maybe_unused]]
static __m128i sWriteMin(__m128i newValue, uint16_t* map, int offset)
{
    __m128i_u* address = (__m128i*) (map + offset);
    __m128i out = _mm_loadu_si128(address);
    __m128i prev = out;
    out = _mm_min_epu16(newValue, out);
    _mm_storeu_si128(address, out);
    return _mm_xor_si128(prev, out);
}

static __m256i sWriteMin(__m256i newValue, uint16_t* __restrict__ map, int offset)
{
    __m256i_u* address = (__m256i*) (map + offset);
    __m256i out = _mm256_loadu_si256(address);
    __m256i prev = out;
    out = _mm256_min_epu16(newValue, out);
    _mm256_storeu_si256(address, out);
    return _mm256_xor_si256(prev, out);
}


static __m256i sWriteMin2(__m256i newValue, uint16_t* __restrict__ map1, uint16_t* __restrict__ map2)
{
    __m256i out = LoadU2(map1, map2);
    __m256i prev = out;
    out = _mm256_min_epu16(newValue, out);
    StoreU2(map1, map2, out);
    return _mm256_xor_si256(prev, out);
}

template<int YDirection>
static bool sUpdateUpDownDir(const uint16_t* __restrict__ numberMap,
    const uint16_t* __restrict__ sourceMap, // upDown map
    const uint8_t* __restrict__ changedRowsSource,
    uint16_t* __restrict__ destMap1, // left right map
    uint8_t* __restrict__ changedRowsDst1,
    int width,
    int height,
    int minimumSameDir,
    int maximumSameDir)
{
    //TIMEDSCOPE("17 Timed scope up down");
    bool changed = false;
    for(int y = RowOffset; y < height + RowOffset; ++y)
    {
        if(changedRowsSource[y] == 0)
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
                if((y + moves * YDirection < 0) | (y + moves * YDirection >= height))
                {
                    break;
                }
                int loopOffset = offset + moves * (YDirection * MaxWidthU16);
                __m256i numbers = _mm256_loadu_si256((const __m256i*) (numberMap + loopOffset));
                values = _mm256_adds_epu16(values, numbers);
                if (moves >= minimumSameDir)
                {
                    changed1[moves] = _mm256_or_si256(sWriteMin(values, destMap1, loopOffset),
                        changed1[moves]);
                }
                moves++;
            }
            x += 16;
        }
        for (int i = 0; i < maximumSameDir; ++i)
        {
            int index = y + i * YDirection;
            if (!(_mm256_testz_si256(changed1[i], changed1[i])))
            {
                changedRowsDst1[index] = 1;
                changed = true;
            }
        }
    }
    return changed;
}

template<int XDirection>
 bool sUpdateSidewayDir(const uint16_t* __restrict__ numberMap,
    const uint16_t* __restrict__ sourceMap, // left right map
    const uint8_t* __restrict__ changedRowsSource,
    uint16_t* __restrict__ destMap1, // up down map
    uint8_t* __restrict__ changedRowsDst1,
    int width,
    int height,
    int minimumSameDir,
    int maximumSameDir)
{

    static constexpr int DirMoveSize = 8;


    #define SetHighest(SetValue) _mm256_set_epi32(SetValue, 0, 0, 0, SetValue, 0, 0, 0)
    #define SetLowest(SetValue) _mm256_set_epi32(0, 0, 0, SetValue, 0, 0, 0, SetValue)

     static __m256i HighBits = SetHighest(0xffff'0000);
     static __m256i LowBits = SetLowest(0x0000'ffff);


     //TIMEDSCOPE("17 Timed scope left right");
    bool changed = false;

    static constexpr int ValueCount = 8;
    static constexpr int WriteCount = ValueCount - 2;
    static constexpr int ValueSize = sizeof(__m128i) / 2;
    static constexpr int MoveAmount = (ValueCount - 2) * ValueSize;

    int rows[2] = {};
    int rowCount = 0;

    for(int y1 = RowOffset; y1 < height + RowOffset; ++y1)
    {
        if(changedRowsSource[y1] == 0)
        {
            if(y1 != height + RowOffset - 1 || rowCount == 0)
                continue;
        }
        rows[rowCount++] = y1;
        if(rowCount < 2)
        {
            continue;
        }
        if(rows[0] == rows[1])
        {
            rows[0] = y1 - 1;
        }
        __m256i changed1 = {};
        __m256i writeValue[ValueCount] = {};
        for(auto & i : writeValue)
        {
            i = ~_mm256_setzero_si256();
        }

        int x = 0;
        if(XDirection < 0)
        {
            // Division round down
            x = width / MoveAmount * MoveAmount;
        }

        while(x >= 0 && x < width)
        {
            int offset1 = rows[0] * MaxWidthU16 + Padding + x;
            int offset2 = rows[1] * MaxWidthU16 + Padding + x;

            __m256i v[ValueCount] = {};
            for(int i = 0; i < WriteCount; ++i)
            {
                int add = i * XDirection * DirMoveSize;
                v[i] = LoadU2(sourceMap + offset1 + add, sourceMap + offset2 + add);
            }
            for(int i = WriteCount; i < ValueCount; ++i)
            {
                v[i] = ~_mm256_setzero_si256();
            }

            __m256i numbers[ValueCount] = {};
            for(int i = 0; i < ValueCount; ++i)
            {
                int add = i * XDirection * DirMoveSize;
                numbers[i] = LoadU2(numberMap + offset1 + add, numberMap + offset2 + add);
            }

            if(XDirection > 0 )
            {
                for (int i = 1; i <= maximumSameDir; ++i)
                {
                    for(int j = ValueCount - 1; j > 0; --j)
                    {
                        // Move from highest to lowest the first 2 bits.
                        __m256i old1 = v[j];
                        __m256i old2 = v[j - 1];

                        old1 = _mm256_slli_si256(old1, 2);
                        old2 = _mm256_srli_si256(old2, 14);
                        old1 = _mm256_or_si256(old1, old2);
                        v[j] = _mm256_adds_epu16(old1, numbers[j]);
                        if (i >= minimumSameDir)
                            writeValue[j] = _mm256_min_epu16(writeValue[j], v[j]);
                    }

                    // Make first 16 bits a high value so it wont be written with min
                    v[0] = _mm256_slli_si256(v[0], 2);
                    v[0] = _mm256_adds_epu16(v[0], numbers[0]);
                    v[0] = _mm256_or_si256(v[0], LowBits);

                    if (i >= minimumSameDir)
                    {
                        writeValue[0] = _mm256_min_epu16(writeValue[0], v[0]);
                    }
                }
            }
            else
            {
                for (int i = 1; i <= maximumSameDir; ++i)
                {
                    for(int j = ValueCount - 1; j > 0; --j)
                    {
                        __m256i old1 = v[j];
                        __m256i old2 = v[j - 1];

                        old1 = _mm256_srli_si256(old1, 2);
                        old2 = _mm256_slli_si256(old2, 14);

                        old1 = _mm256_or_si256(old1, old2);

                        v[j] = _mm256_adds_epu16(old1, numbers[j]);
                        if (i >= minimumSameDir)
                            writeValue[j] = _mm256_min_epu16(writeValue[j], v[j]);
                    }
                    v[0] = _mm256_srli_si256(v[0], 2);
                    v[0] = _mm256_adds_epu16(v[0], numbers[0]);

                    v[0] = _mm256_or_si256(v[0], HighBits);

                    if (i >= minimumSameDir)
                    {
                        writeValue[0] = _mm256_min_epu16(writeValue[0], v[0]);
                    }
                }
            }

            for(int i = 0; i < WriteCount; ++i)
            {
                int off = DirMoveSize * i * XDirection;
                int currOffset = x + off;
                if(currOffset < width && currOffset > -MoveAmount)
                    changed1 = _mm256_or_si256(sWriteMin2(
                        writeValue[i], destMap1 + offset1 + off, destMap1 + offset2 + off), changed1);
            }
            for(int i = 0; i < ValueCount; ++i)
            {
                if(i >= WriteCount)
                {
                    writeValue[i - WriteCount] = writeValue[i];
                }
                writeValue[i] = ~_mm256_setzero_si256();
            }
            x += MoveAmount * XDirection;
        }

        {
            //int64_t z = _mm256_movemask_epi8(changed1);
            if(!_mm256_testz_si256(changed1, _mm256_set_epi32(0, 0, 0, 0, -1, -1, -1, -1)))
            {
                changedRowsDst1[rows[0]] = 1;
                changed = true;
            }
            if(!_mm256_testz_si256(changed1, _mm256_set_epi32(-1, -1, -1, -1, 0, 0, 0, 0)))
            {
                changedRowsDst1[rows[1]] = 1;
                changed = true;
            }
        }
        rowCount = 0;
    }

    return changed;
}


static int64_t sGetMinimumEnergy(const char* data, int minimumSameDir, int maximumSameDir)
{
    int width = 0;
    int height = 0;

    sGetSize(data, width, height);

    assert(minimumSameDir <= maximumSameDir);
    //assert(maximumSameDir < Padding);
    assert(width <= MaxWidthU16);
    assert(height <= MaxHeight);

    alignas(32) uint16_t numberMap[MaxWidthU16 * MaxHeight] = {};

    alignas(32) uint16_t leftRightMap[MaxWidthU16 * MaxHeight] = {};
    alignas(32) uint16_t upDownMap[MaxWidthU16 * MaxHeight] = {};

    alignas(32) uint8_t changedRowsLeftRight[32 * 8] = {};
    alignas(32) uint8_t changedRowsUpDown[32 * 8] = {};

    sMemset(leftRightMap, uint16_t(1 << 15), MaxWidthU16 * MaxHeight);
    sMemset(upDownMap, uint16_t(1 << 15), MaxWidthU16 * MaxHeight);
    sMemset(numberMap, uint16_t(1 << 15), MaxWidthU16 * MaxHeight);

    {
        const char* tmp = data;
        int index = Padding + MaxWidthU16 * RowOffset;
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
    leftRightMap[Padding + RowOffset * MaxWidthU16] = 0;
    upDownMap[Padding + RowOffset * MaxWidthU16] = 0;

    changedRowsLeftRight[0 + RowOffset] = 1;
    changedRowsUpDown[0 + RowOffset] = 1;

    uint16_t lowest = 0;
    {
        //TIMEDSCOPE("17 Update maps");
        bool changed = true;
        while (changed)
        {
            changed = false;
            changed |= sUpdateSidewayDir<1>(numberMap, leftRightMap, changedRowsLeftRight,
                upDownMap, changedRowsUpDown,
                width, height, minimumSameDir,
                maximumSameDir);

            changed |= sUpdateSidewayDir<-1>(numberMap, leftRightMap, changedRowsLeftRight,
                upDownMap, changedRowsUpDown,
                width, height, minimumSameDir,
                maximumSameDir);
            memset(changedRowsLeftRight, 0, 32 * 8);

            changed |= sUpdateUpDownDir<-1>(numberMap, upDownMap, changedRowsUpDown,
                leftRightMap, changedRowsLeftRight,
                width, height, minimumSameDir,
                maximumSameDir);

            changed |= sUpdateUpDownDir<1>(numberMap, upDownMap, changedRowsUpDown,
                leftRightMap, changedRowsLeftRight,
                width, height, minimumSameDir,
                maximumSameDir);

            memset(changedRowsUpDown, 0, 32 * 8);

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

