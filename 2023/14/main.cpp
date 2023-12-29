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

alignas(32) static constexpr char test14A[] =
    R"(O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....
)";

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("14A Total");
    int64_t sum = 0;

    uint8_t freeSlots[128] = {};
    int rockCount = 0;
    int8_t x = 0;
    int8_t y = 0;
    while(*data)
    {
        switch(*data)
        {
            case '\n':
                x = -1;
                ++y;
                break;
            case 'O':
                rockCount++;
                sum += freeSlots[x];
                freeSlots[x]++;
                break;
            case '#':
                freeSlots[x] = y + 1;
        }
        ++x;
        ++data;
    }
    sum = y * rockCount - sum;
    return sum;
}

// Note we split 2 lines into 128 bits. No need to deal with crossing lanes
void bitShiftRightOne(__m256i* value)
{
    __m256i movedTop = _mm256_bsrli_epi128(*value, 8);
    movedTop = _mm256_slli_epi64(movedTop, 63);
    *value = _mm256_srli_epi64(*value, 1);
    *value = _mm256_or_si256(*value, movedTop);
}

// Note we split 2 lines into 128 bits. No need to deal with crossing lanes
void bitShiftLeftOne(__m256i* value)
{
    __m256i movedBot = _mm256_bslli_epi128(*value, 8);
    movedBot = _mm256_srli_epi64(movedBot, 63);
    *value = _mm256_slli_epi64(*value, 1);
    *value = _mm256_or_si256(*value, movedBot);
}

void bitShiftRightOne(__m128i* value)
{
    __m128i movedTop = _mm_bsrli_si128(*value, 8);
    movedTop = _mm_slli_epi64(movedTop, 63);
    *value = _mm_srli_epi64(*value, 1);
    *value = _mm_or_si128(*value, movedTop);
}

void bitShiftLeftOne(__m128i* value)
{
    __m128i movedBot = _mm_bslli_si128(*value, 8);
    movedBot = _mm_srli_epi64(movedBot, 63);
    *value = _mm_slli_epi64(*value, 1);
    *value = _mm_or_si128(*value, movedBot);
}

static bool sMoveRocksUpDown(const __m128i* __restrict__ wallMap,
    __m128i* rockMap,
    int startIndex,
    int moveDir)
{

    __m128i changed = _mm_setzero_si128();
    __m128i row = rockMap[startIndex];
    rockMap[startIndex] = _mm_setzero_si128();
    int j = startIndex + moveDir;
    while(!_mm_testz_si128(row, row))
    {
        __m128i rocks = rockMap[j];
        __m128i colliders = _mm_or_si128(rocks, wallMap[j]);
        __m128i collided = _mm_and_si128(row, colliders);
        rockMap[j - moveDir] = _mm_or_si128(collided, rockMap[j - moveDir]);
        row = _mm_xor_si128(row, collided);
        changed = _mm_or_si128(changed, row);
        j += moveDir;
    }
    return !_mm_test_all_ones(~changed);
}
template<int moveDir>
constexpr static void sBitShift(__m128i* value) //, int dir)
{

    if(moveDir == -1)
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        //*value >>= 1;
        bitShiftRightOne(value);
    }
//    else if(dir == 1)
    else
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        //*value <<= 1;
        bitShiftLeftOne(value);
    }
}


template<int moveDir>
constexpr static void sBitShift(__m256i* value) //, int dir)
{

    if(moveDir == -1)
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        //*value >>= 1;
        bitShiftRightOne(value);
    }
//    else if(dir == 1)
    else
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        //*value <<= 1;
        bitShiftLeftOne(value);
    }
}


template<int moveDir>
static bool sMoveRocksLeftRight(const __m128i* __restrict__ wallMap,
    __m128i* __restrict__ rockMap,
    int rowIndex)
{
    __m256i row1 = _mm256_loadu_si256((const __m256i*)(rockMap + rowIndex));
    __m256i collisions1 = _mm256_loadu_si256((const __m256i*)(wallMap + rowIndex));
    __m256i origColl1 = collisions1;

    sBitShift<-moveDir>(&collisions1);
    __m256i newRow1 = _mm256_setzero_si256();

    while(!_mm256_testz_si256(row1, row1))
    {
        __m256i collided1 = _mm256_and_si256(collisions1, row1);

        while (!_mm256_testz_si256(collided1, row1))
        {

            for (int i = 0; i < 2; ++i)
            {
                collided1 = _mm256_and_si256(collided1, row1);
                sBitShift<-moveDir>(&collided1);
                collisions1 = _mm256_or_si256(collided1, collisions1);
            }
        }
        newRow1 = _mm256_or_si256(newRow1, _mm256_and_si256(row1, collisions1));
        row1 = _mm256_andnot_si256(collisions1, row1);
        sBitShift<moveDir>(&row1);
    }
    _mm256_storeu_si256((__m256i*)(rockMap + rowIndex), newRow1);

    return !_mm256_testc_si256(collisions1, origColl1);
}

static int64_t sCountScore(const __m128i* __restrict__ rockMap, int height)
{
    int64_t sum = 0;
    const uint64_t* values = (const uint64_t*)(rockMap + 1);
    for(int i = 1; i < height + 2; ++i)
    {
        int multiplier = height - i + 1;
        sum += multiplier * std::popcount((uint64_t)values[0]);
        sum += multiplier * std::popcount((uint64_t)values[1]);
        values += 2;
    }
    return sum;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("14B Total");
    int64_t sum = 0;

    __m128i rockMap[128] = {};
    __m128i wallMap[128] = {};
    __m128i one = _mm_set_epi32(0, 0, 0, 1);;
    __m128i minusOne = _mm_set1_epi32(-1);

    int8_t width = 0;
    int8_t height = 0;

    int8_t x = 1;
    int8_t y = 0;
    __m128i bitShift = one << 2;
    {
        TIMEDSCOPE("14B Generate rock and wall maps");
        while (*data)
        {
            [[__maybe_unused__]] __m128i old = rockMap[y + 1];
            switch (*data)
            {
                case '\n':
                    width = x;
                    y += 1;
                    x = 0;
                    bitShift = one << 1;
                    break;
                case 'O':
                    rockMap[y + 1] = _mm_or_si128(rockMap[y + 1], bitShift);
                    assert(!_mm_test_all_ones(~(old ^ rockMap[y + 1])));
                    break;
                case '#':
                    wallMap[y + 1] = _mm_or_si128(wallMap[y + 1], bitShift);
                    assert(!_mm_test_all_ones(~(old ^ wallMap[y + 1])));
                    break;

            }
            x += 1;
            bitShift += bitShift;
            if (_mm_test_all_ones(~bitShift))
            {
                bitShift = _mm_set_epi32(0, 1, 0, 0);
            }
            assert(!_mm_test_all_ones(~bitShift));
            ++data;
        }
    }
    height = y;
    wallMap[0] = minusOne;
    wallMap[height + 1] = minusOne;

    int moveAmount = width + 1;
    __m128i rightWall;
    if(moveAmount < 64)
    {
        rightWall = one << moveAmount;
    }
    else
    {
        rightWall = _mm_set_epi32(0, 1, 0, 0);
        rightWall <<= moveAmount - 64;
    }
    __m128i leftRightWall = rightWall | _mm_set_epi32(0, 0, 0, 3);
    for(__m128i& i : wallMap)
    {
        i |= leftRightWall;
    }

    std::unordered_map<int64_t, int64_t> diffMap;
    diffMap.reserve(65536);
    static const int64_t LoopCounts = 1000000000;
    for(int64_t j = 0; j < LoopCounts; ++j)
    {
        //TIMEDSCOPE("14 Loop");
        // Move up
        for (int i = 2; i <= height; ++i)
        {
            sMoveRocksUpDown(wallMap, rockMap, i, -1);
        }

        __m128i rowDiffs = _mm_setzero_si128();

        for(int i = 1; i <= height; ++i)
        {
            rowDiffs = _mm_xor_si128(rowDiffs,rockMap[i]);
        }


        int64_t* values = (int64_t *) &rowDiffs;
        int64_t hashValue = values[0] ^ values[1];

        auto iter = diffMap.find(hashValue);
        if(iter != diffMap.end())
        {
            int64_t loop = j - iter->second;
            loop = loop != 0 ? loop : j;
            int64_t tmp = LoopCounts - j;
            j += (tmp / loop) * loop;
            j -= loop;
            if(j + loop < LoopCounts)
                j += loop;
        }
        diffMap[hashValue] = j;

        if(_mm_test_all_ones(~rowDiffs))
        {
            break;
        }

        int end = (height - 1) / 8 * 8;

        // Move left
        for(int i = 1; i <= end + 1; i += 8)
        {
            sMoveRocksLeftRight<-1>(wallMap, rockMap, i + 0);
            sMoveRocksLeftRight<-1>(wallMap, rockMap, i + 2);
            sMoveRocksLeftRight<-1>(wallMap, rockMap, i + 4);
            sMoveRocksLeftRight<-1>(wallMap, rockMap, i + 6);
        }
        for(int i = end + 1; i <= height + 1; i += 2)
        {
            sMoveRocksLeftRight<-1>(wallMap, rockMap, i + 0);
        }

        // Move down
        for (int i = height - 1; i > 0; --i)
        {
            sMoveRocksUpDown(wallMap, rockMap, i, 1);
        }

        // Move left
        for(int i = 1; i <= end + 1; i += 8)
        {
            sMoveRocksLeftRight<1>(wallMap, rockMap, i + 0);
            sMoveRocksLeftRight<1>(wallMap, rockMap, i + 2);
            sMoveRocksLeftRight<1>(wallMap, rockMap, i + 4);
            sMoveRocksLeftRight<1>(wallMap, rockMap, i + 6);
        }
        for(int i = end + 1; i <= height; i += 2)
        {
            sMoveRocksLeftRight<1>(wallMap, rockMap, i + 0);
        }
    }
    sum = sCountScore(rockMap, height);

    return sum;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "14A: Sum of pressure: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "14B: Sum of pressure: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data14A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data14A));
    printf("%s\n", printBuffer);

    return 0;
}
#endif

int run14A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data14A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run14B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data14A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

