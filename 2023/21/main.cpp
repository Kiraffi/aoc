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

alignas(32) static constexpr char test21A[] =
    R"(...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........
)";

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

void sGetStart(const char* data, int& x, int& y)
{
    int width = 0;
    int height = 0;

    sGetSize(data, width, height);

    const char* dataStart = data;
    while(*data != 'S') data++;
    x = intptr_t(data - dataStart) / (width + 1);
    y = intptr_t(data - dataStart) / (width + 1);
}

int64_t sGetFillCount(const uint8_t* map2, int x, int y, int width, int height, int steps)
{
    alignas(32) uint8_t map[32 * 140] = {};
    map[x / 8 + y * 32] |= 1 << (x % 8);

    for(int i = 0; i < steps; ++i)
    {
        __m128i curr1 = _mm_setzero_si128();
        __m128i curr2 = _mm_setzero_si128();

        __m128i next1 = _mm_load_si128((const __m128i*)(map + 00));
        __m128i next2 = _mm_load_si128((const __m128i*)(map + 16));

        for(y = 0; y < height; ++y)
        {
            __m128i prev1 = curr1;
            __m128i prev2 = curr2;

            curr1 = next1;
            curr2 = next2;
            int offset = y * 32;
            // y + 1
            next1 = _mm_load_si128((const __m128i*)(map + offset + 00 + 32));
            next2 = _mm_load_si128((const __m128i*)(map + offset + 16 + 32));

            __m128i p1 = curr1;
            sBitShiftLeftOne(&p1);
            __m128i p11 = curr1;
            sBitShiftRightOne(&p11);
            p1 = _mm_or_si128(p1, p11);

            // Move that one bit from the later top one to bot first
            // move 14 * 8 + 15 bits.
            __m128i p2 = _mm_bslli_si128(curr2, 14);
            p2 = _mm_slli_epi16(p2, 15);
            p2 = _mm_or_si128(p2, prev1);
            p1 = _mm_or_si128(p1, next1);
            p1 = _mm_or_si128(p1, p2);


            __m128i p3 = curr2;
            sBitShiftLeftOne(&p3);
            __m128i p31 = curr2;
            sBitShiftRightOne(&p31);
            p3 = _mm_or_si128(p3, p31);

            __m128i p4 = _mm_bsrli_si128(curr1, 14);
            p4 = _mm_srli_epi16(p4, 15);
            p4 = _mm_or_si128(p4, prev2);
            p3 = _mm_or_si128(p3, next2);
            p3 = _mm_or_si128(p4, p3);

            __m128i noWall1 = _mm_load_si128((const __m128i*)(map2 + offset + 00));
            __m128i noWall2 = _mm_load_si128((const __m128i*)(map2 + offset + 16));

            p1 = _mm_and_si128(p1, noWall1);
            p3 = _mm_and_si128(p3, noWall2);

            _mm_storeu_si128( (__m128i_u*)(map + offset + 00), p1);
            _mm_storeu_si128( (__m128i_u*)(map + offset + 16), p3);
        }
    }

    // Draw map
    if(false)
    {
        for (int aa = 0; aa < height; ++aa)
        {
            for (int bb = 0; bb < width; ++bb)
            {
                char c = (map2[bb / 8 + aa * 32] >> (bb % 8)) & 1;
                c = c == 1 ? '.' : '#';
                if (((map[bb / 8 + aa * 32] >> (bb % 8)) & 1) != 0)
                    printf("O");
                else
                    printf("%c", c);
            }
            printf("\n");
        }
        printf("\n");
        printf("\n");
    }

    int64_t result = 0;
    const uint64_t* ptr = (const uint64_t*) map;
    for(int i = 0; i < height * 32; i += 8)
    {
        result += std::popcount( *ptr );
        ptr++;
    }
    return result;
}


static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("21A Total");
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);
    int x = 0;
    int y = 0;
    sGetStart(data, x, y);

    alignas(32) uint8_t map2[32 * 140] = {};
    for(int yy = 0; yy < height; ++yy)
    {
        for(int xx = 0; xx < width; ++xx)
        {
            if(data[xx + yy * (width + 1)] != '#')
                map2[xx / 8 + yy * 32] |= (1 << (xx % 8));
        }
    }

    return sGetFillCount(map2, x, y, width, height, 64);
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("21B Total");
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);

    assert(width == height);

    int x = 0;
    int y = 0;
    sGetStart(data, x, y);

    alignas(32) uint8_t map2[32 * 140] = {};
    for(int yy = 0; yy < height; ++yy)
    {
        for(int xx = 0; xx < width; ++xx)
        {
            if(data[xx + yy * (width + 1)] != '#')
                map2[xx / 8 + yy * 32] |= (1 << (xx % 8));
        }
    }


    static constexpr int Steps = 26501365;
    assert(Steps % width == x);

    int64_t grids = Steps / width - 1;
    int64_t oddGrids = (grids / 2 * 2 + 1);
    oddGrids *= oddGrids;
    int64_t evenGrids = ((grids + 1) / 2 * 2);
    evenGrids *= evenGrids;

    int64_t oddCount = sGetFillCount(map2,  x, y, width, height, width * 3 / 2 + 1);
    int64_t evenCount = sGetFillCount(map2, x, y, width, height, width * 3 / 2);

    int64_t uCount = sGetFillCount(map2, x,         height - 1, width, height, height - 1);
    int64_t dCount = sGetFillCount(map2, x,         0,          width, height, height - 1);
    int64_t lCount = sGetFillCount(map2, width - 1, y,          width, height, width - 1);
    int64_t rCount = sGetFillCount(map2, 0,         y,          width, height, width - 1);

    int64_t trCount = sGetFillCount(map2, 0,         height - 1, width, height, width / 2 - 1);
    int64_t tlCount = sGetFillCount(map2, width - 1, height - 1, width, height, width / 2 - 1);
    int64_t brCount = sGetFillCount(map2, 0,         0,          width, height, width / 2 - 1);
    int64_t blCount = sGetFillCount(map2, width - 1, 0,          width, height, width / 2 - 1);

    int64_t trbCount = sGetFillCount(map2, 0,         height - 1, width, height, width * 3 / 2 - 1);
    int64_t tlbCount = sGetFillCount(map2, width - 1, height - 1, width, height, width * 3 / 2 - 1);
    int64_t brbCount = sGetFillCount(map2, 0,         0,          width, height, width * 3 / 2 - 1);
    int64_t blbCount = sGetFillCount(map2, width - 1, 0,          width, height, width * 3 / 2 - 1);

    return oddGrids * oddCount + evenGrids * evenCount
        + uCount + dCount + lCount + rCount
        + (grids + 1) * (trCount + tlCount + brCount + blCount)
        + (grids + 0) * (trbCount + tlbCount + brbCount + blbCount);
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "21A: Spots: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "21B: Spots: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data21A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data21A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run21A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data21A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run21B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data21A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

