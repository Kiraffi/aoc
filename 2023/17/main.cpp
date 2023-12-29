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


static __m256i HighBits256 = _mm256_set_epi32(0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0, 0, 0, 0);
static __m256i LowBits256 = _mm256_set_epi32(0, 0, 0, 0, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff, 0xffff'ffff);

#if 0
using VType = __m128i;
#define I128 1
#define SetHighest(SetValue) _mm_set_epi32(SetValue, 0, 0, 0)
#define SetLowest(SetValue) _mm_set_epi32(0, 0, 0, SetValue)

#else
using VType = __m256i;
#define SetHighest(SetValue) _mm256_set_epi32(SetValue, 0, 0, 0, 0, 0, 0 ,0)
#define SetLowest(SetValue) _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, SetValue)
#endif

static VType HighBits = SetHighest(0xffff'0000);
static VType LowBits = SetLowest(0x0000'ffff);



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

template <typename T>
static void sMemset(T* arr, T value, int amount)
{
    const T* end = arr + amount;
    while(arr < end)
    {
        *arr++ = value;
    }

}

template<int Amount>
static __m256i sByteShiftRight256(__m256i value)
{
    static_assert(Amount > 0 && Amount < 16);
    static_assert(Amount == 2);
    __m256i movedTop = _mm256_bsrli_epi128(value, Amount);
    value = _mm256_permute4x64_epi64(value, _MM_SHUFFLE(1, 1, 2, 2));
    value = _mm256_bslli_epi128(value, 16 - Amount);
    value = _mm256_and_si256(value, LowBits256);
    return _mm256_or_si256(value, movedTop);
}

template<int Amount>
static __m256i sByteShiftLeft256(__m256i value)
{
    static_assert(Amount > 0 && Amount < 16);
    __m256i movedBot = _mm256_bslli_epi128(value, Amount);
    value = _mm256_permute4x64_epi64(value, _MM_SHUFFLE(1, 1, 2, 2));
    value = _mm256_bsrli_epi128(value, 16 - Amount);
    value = _mm256_and_si256(value, HighBits256);
    return _mm256_or_si256(value, movedBot);
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

static bool sUpdateDir(const uint16_t* __restrict__ numberMap,
    const uint16_t* __restrict__ sourceMap, // upDown map
    const uint8_t* __restrict__ changedRowsSource,
    uint16_t* __restrict__ destMap1, // left right map
    uint8_t* __restrict__ changedRowsDst1,
    int width,
    int height,
    int xDirection,
    int yDirection,
    int minimumSameDir,
    int maximumSameDir)
{
    //TIMEDSCOPE("17 Timed scope up down");
    bool changed = false;
    for(int y = RowOffset; y < height + RowOffset; ++y)
    {
        //if((changedRowsSource[y / 8] >> (y % 8)) == 0)
        if(changedRowsSource[y] == 0) // / 8] >> (y % 8)) == 0)
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
                    changed1[moves] = _mm256_or_si256(sWriteMin(values, destMap1, loopOffset),
                        changed1[moves]);
                }
                moves++;
            }
            x += 16;
        }
        for (int i = 0; i < maximumSameDir; ++i)
        {
            int index = y + i * yDirection;
            if (!(_mm256_testz_si256(changed1[i], changed1[i])))
            {
                //changedRowsDst1[(index) / 8] |= 1 << ((index) % 8);
                changedRowsDst1[index] = 1;
                changed = true;
            }
        }
    }
    return changed;
}


 bool sUpdateDir2(const uint16_t* __restrict__ numberMap,
    const uint16_t* __restrict__ sourceMap, // left right map
    const uint8_t* __restrict__ changedRowsSource,
    uint16_t* __restrict__ destMap1, // up down map
    uint8_t* __restrict__ changedRowsDst1,
    int width,
    int height,
    int xDirection,
    int minimumSameDir,
    int maximumSameDir)
{
#if I128
    static constexpr int DirMoveSize = 8;


    #define sByteShiftLeft sByteShiftLeft128
    #define sByteShiftRight sByteShiftRight128

    #define Shuffle(Vec, SHUFFLE) _mm_shuffle_epi32(Vec, SHUFFLE)
    #define LoadU(Addr) _mm_loadu_si128((const __m128i *)(Addr))
    #define VRotR16(Value, Amount) _mm_srli_epi16(Value, Amount)
    #define VRotL16(Value, Amount) _mm_slli_epi16(Value, Amount)

    #define VRotRVType(Value, Amount) _mm_srli_si128(Value, Amount)
    #define VRotLVType(Value, Amount) _mm_slli_si128(Value, Amount)

    #define VAnd(AValue, BValue) _mm_and_si128(AValue, BValue)
    #define VAndNot(AValue, BValue) _mm_andnot_si128(AValue, BValue)
    #define VOr(AValue, BValue) _mm_or_si128(AValue, BValue)
    #define VAddsU16(AValue, BValue) _mm_adds_epu16(AValue, BValue)
    #define VMinU16(AValue, BValue) _mm_min_epu16(AValue, BValue)
    #define VAndTestZero(AValue, BValue) _mm_testz_si128(AValue, BValue)
    #define VSetZero() _mm_setzero_si128()
    #define BroadcastI16 _mm_broadcastw_epi16
#else
     #define sByteShiftLeft sByteShiftLeft256
    #define sByteShiftRight sByteShiftRight256

     static constexpr int DirMoveSize = 16;
    #define Shuffle _mm256_permute4x64_epi64
    #define LoadU(Addr) _mm256_loadu_si256((const __m256i *)(Addr))

    #define VRotR16(Value, Amount) _mm256_srli_epi16(Value, Amount)
    #define VRotL16(Value, Amount) _mm256_slli_epi16(Value, Amount)

    #define VRotRVType(Value, Amount) _mm256_srli_si256(Value, Amount)
    #define VRotLVType(Value, Amount) _mm256_slli_si256(Value, Amount)

    #define VAnd(AValue, BValue) _mm256_and_si256(AValue, BValue)
    #define VAndNot(AValue, BValue) _mm256_andnot_si256(AValue, BValue)
    #define VOr(AValue, BValue) _mm256_or_si256(AValue, BValue)
    #define VAddsU16(AValue, BValue) _mm256_adds_epu16(AValue, BValue)
    #define VMinU16(AValue, BValue) _mm256_min_epu16(AValue, BValue)
    #define VAndTestZero(AValue, BValue) _mm256_testz_si256(AValue, BValue)
    #define VSetZero() _mm256_setzero_si256()
    #define BroadcastI16(Value) _mm256_broadcastw_epi16(_mm256_castsi256_si128(Value))
#endif
     //TIMEDSCOPE("17 Timed scope left right");
    bool changed = false;

     static constexpr int ValueCount = 6;
     static constexpr int WriteCount = sizeof(VType) == 16 ? ValueCount - 2 : ValueCount - 2;
     static constexpr int ValueSize = sizeof(VType) / 2;
     static constexpr int MoveAmount = (ValueCount - 2) * ValueSize;

    [[maybe_unused]] static VType High2Bits = SetHighest(0xc000'0000);
    [[maybe_unused]] static VType Low2Bits = SetLowest(0x0000'000c);

    for(int y = RowOffset; y < height + RowOffset; ++y)
    {
        //if((changedRowsSource[y / 8] >> (y % 8)) == 0)
        if(changedRowsSource[y] == 0)
        {
            continue;
        }

        VType changed1 = VSetZero();
        VType writeValue[ValueCount] = {};
        for(auto & i : writeValue)
        {
            i = ~VSetZero();
        }

        int x = 0;
        if(xDirection < 0)
        {
            // Division round down
            x = width / MoveAmount * MoveAmount;
        }

        while(x >= 0 && x < width)
        {
            int offset = y * MaxWidthU16 + Padding + x;

            VType v[ValueCount] = {};
            for(int i = 0; i < WriteCount; ++i)
            {
                v[i] = LoadU(sourceMap + offset + i * xDirection * DirMoveSize);
            }
            for(int i = WriteCount; i < ValueCount; ++i)
            {
                v[i] = ~VSetZero();
            }

            VType numbers[ValueCount] = {};
            for(int i = 0; i < ValueCount; ++i)
            {
                numbers[i] = LoadU(numberMap + offset + i * xDirection * DirMoveSize);
            }

            if(xDirection > 0 )
            {
                for (int i = 1; i <= maximumSameDir; ++i)
                {
                    VType prev = VSetZero();
                    for(int j = 0; j < ValueCount; ++j)
                    {
                        // Move from highest to lowest the first 2 bits.
                        VType old1 = v[j];
                        VType old2 = prev;

                        prev = old1;

                        // Deal with 64 and 128 bit cross lane border
                        //VType tmp1 = Shuffle(old1, _MM_SHUFFLE(2, 1, 0, 3));
                        //tmp1 = VRotR16(tmp1, 14);
                        //tmp1 = VAndNot(Low2Bits, tmp1);

                        // deal with moving last 2 bits from prev read to first 2 bits.
                        //__m256i tmp2 = _mm256_permute4x64_epi64(old2, _MM_SHUFFLE(3, 3, 3, 3));
#if 1
                        VType tmp1 = sByteShiftLeft<2>(old1);
                        VType tmp2 = Shuffle(old2, _MM_SHUFFLE(3,3,3,3));
                        tmp2 = VRotRVType(tmp2, 14);
                        tmp2 = VAnd(LowBits, tmp2);

/*
                        old1 = VRotLVType(old1, 2);
                        old2 = VRotRVType(old2, 14);
                        assert(VAndTestZero(~tmp, old1));
                        assert(VAndTestZero(~tmp2, old2));
*/
                        old1 = tmp1;
                        old2 = tmp2;
#else

                        old1 = VRotLVType(old1, 2);
                        old2 = VRotRVType(old2, 14);
#endif

                        old1 = VOr(old1, old2);


                        v[j] = VAddsU16(old1, numbers[j]);
                    }

                    // Make first 16 bits a high value so it wont be written with min
                    //v[0] = _mm256_or_si256(v[0], LowBits);
                    v[0] = VOr(v[0], LowBits);

                    if (i >= minimumSameDir)
                    {
                        for(int j = 0; j < ValueCount; ++j)
                        {
                            writeValue[j] = VMinU16(writeValue[j], v[j]);

                        }
                    }
                }
            }
            else
            {
                for (int i = 1; i <= maximumSameDir; ++i)
                {
                    VType prev = VSetZero();

                    for(int j = 0; j < ValueCount; ++j)
                    {
                        VType old1 = v[j];
                        VType old2 = prev;
                        prev = old1;

#if 1
                        VType tmp1 = sByteShiftRight<2>(old1);
                        //old1 = VRotRVType(old1, 2);
                        VType tmp2 = BroadcastI16(old2);
                        tmp2 = VRotLVType(tmp2, 14);
                        tmp2 = VAnd(HighBits, tmp2);
/*
                        old1 = VRotRVType(old1, 2);
                        old2 = VRotLVType(old2, 14);
                        assert(VAndTestZero(~tmp1, old1));
                        assert(VAndTestZero(~tmp2, old2));
*/
                        old1 = tmp1;
                        old2 = tmp2;
#else

                        old1 = VRotRVType(old1, 2);
                        old2 = VRotLVType(old2, 14);
#endif

                        old1 = VOr(old1, old2);

                        v[j] = VAddsU16(old1, numbers[j]);
                    }
                    v[0] = VOr(v[0], HighBits);

                    if (i >= minimumSameDir)
                    {
                        for(int j = 0; j < ValueCount; ++j)
                        {
                            writeValue[j] = VMinU16(writeValue[j], v[j]);
                        }
                    }
                }
            }

            for(int i = 0; i < WriteCount; ++i)
            {
                int off = DirMoveSize * i * xDirection;
                int currOffset = x + off;
                if(currOffset < width && currOffset > -MoveAmount)
                    changed1 = VOr(sWriteMin(writeValue[i], destMap1, offset + off), changed1);
            }
            for(int i = 0; i < ValueCount; ++i)
            {
                if(i >= WriteCount)
                {
                    writeValue[i - WriteCount] = writeValue[i];
                }
                writeValue[i] = ~VSetZero();
            }
            x += MoveAmount * xDirection;
        }

        {
            if (!VAndTestZero(changed1, changed1))
            {
                //changedRowsDst1[(y) / 8] |= 1 << ((y) % 8);
                changedRowsDst1[y] = 1;
                changed = true;
            }
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

    uint16_t lowest = ~0;
    {
        TIMEDSCOPE("17 Update maps");
        bool changed = true;
        int rounds = 0;
        while (changed)
        {
            ++rounds;
            changed = false;
            changed |= sUpdateDir2(numberMap, leftRightMap, changedRowsLeftRight,
                upDownMap, changedRowsUpDown,
                width, height, 1, minimumSameDir,
                maximumSameDir);

            changed |= sUpdateDir2(numberMap, leftRightMap, changedRowsLeftRight,
                upDownMap,  changedRowsUpDown,
                width, height, -1, minimumSameDir,
                maximumSameDir);
            memset(changedRowsLeftRight, 0, 32 * 8);

            changed |= sUpdateDir(numberMap, upDownMap, changedRowsUpDown,
                leftRightMap, changedRowsLeftRight,
                width, height, 0, -1, minimumSameDir,
                maximumSameDir);

            changed |= sUpdateDir(numberMap, upDownMap, changedRowsUpDown,
                leftRightMap, changedRowsLeftRight,
                width, height, 0, 1, minimumSameDir,
                maximumSameDir);

            memset(changedRowsUpDown, 0, 32 * 8);

        }
        printf("rounds: %i\n", rounds);
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

