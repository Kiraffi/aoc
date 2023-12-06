#include <algorithm> // std::max
#include <assert.h> // assert
#include <bit> // std::countr_zero
#include <ctype.h> //isdigit
#include <immintrin.h> // SIMD
#include <stdint.h> // intptr
#include <stdio.h> // printf
#include <stdlib.h> //strtol
#include <string.h> //strlen

#include "input.cpp"

using CallFn = void (*)();

alignas(16) static constexpr char test03A[] =
    R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)";

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

template<typename T>
static void sFindSurroundingNumbers(int x, int y, int width, int height, const char* start,
                                    T&& lambdaFn)
{
    for(int j = y - 1; j <= y + 1; ++j)
    {
        if(j < 0 || j >= height)
            continue;

        for(int i = x - 1; i <= x + 1; ++i)
        {
            if(i < 0 || i >= width)
            {
                continue;
            }
            const char* pStart = start + j * (width + 1) + i;
            if(!isdigit(*pStart))
                continue;
            const char* p = pStart;
            int digit = 0;
            while(isdigit(*pStart)) pStart--;
            pStart++;
            while(isdigit(*p))
            {
                ++i;
                p++;
            }
            while(pStart < p)
            {
                digit = digit * 10 + (*pStart) - '0';
                ++pStart;
            }
            lambdaFn(digit);
        }
    }


}

static void sParse03A(const char* data, bool printOut)
{
    const char* start = data;

    int sum = 0;
    int width = 0;
    int height = 0;

    sGetSize(data, width, height);

    int x = 0;
    int y = 0;

    const __m128i* ptr = (const __m128i*)data;

    const __m128i zeroMinusOneChar = _mm_set1_epi8('0' - 1);
    const __m128i tenChar = _mm_set1_epi8('9' + 1);
    const __m128i floorChar = _mm_set1_epi8('.');
    const __m128i endRowChar = _mm_set1_epi8('\n');

    while(y < height)
    {
        __m128i value = _mm_loadu_si128(ptr);

        __m128i endRow = _mm_cmpeq_epi8(value, endRowChar);
        __m128i floor = _mm_cmpeq_epi8(value, floorChar);

        // 0 < x < 10
        __m128i lt10 = _mm_cmplt_epi8(value, tenChar);
        __m128i gtMinusOne = _mm_cmpgt_epi8(value, zeroMinusOneChar);

        //__m128i number = _mm_cmpge_epi8 _cmpeq_epi8(readPtr, floorChar);

        __m128i tile1 = _mm_or_si128(endRow, floor);
        __m128i tile2 = _mm_and_si128(lt10, gtMinusOne);
        __m128i tile = _mm_or_si128(tile1, tile2);

        tile = ~tile;
        uint32_t mask = _mm_movemask_epi8(tile);

        int tmpX = x;
        int tmpY = y;
        while(mask != 0)
        {
            int zeros = std::countr_zero(mask);
            mask = mask >> zeros;
            tmpX += zeros;
            if(tmpX >= width)
            {
                tmpX = tmpX - width - 1;
                tmpY++;

            }
            sFindSurroundingNumbers(tmpX, tmpY, width, height, start, [&](int digit) {
                sum += digit;
            });
            mask = mask >> 1;
            tmpX++;
        }

        x += 16;
        if(x >= width)
        {
            x = x - width - 1;
            y++;
        }
        ++ptr;
        ++data;
    };

    if(printOut)
        printf("3A: Sum of valid digits: %i\n", sum);
}


static void sParse03B(const char* data, bool printOut)
{
    const char* start = data;
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);
    int x = 0;
    int y = 0;
    int sum = 0;

    const __m128i* ptr = (const __m128i*)data;

    const __m128i starChar = _mm_set1_epi8('*');

    while(y < height)
    {
        __m128i value = _mm_loadu_si128(ptr);

        __m128i tile = _mm_cmpeq_epi8(value, starChar);
        uint32_t mask = _mm_movemask_epi8(tile);

        int tmpX = x;
        int tmpY = y;
        while(mask != 0)
        {
            int zeros = std::countr_zero(mask);
            mask = mask >> zeros;
            tmpX += zeros;
            if(tmpX >= width)
            {
                tmpX = tmpX - width - 1;
                tmpY++;

            }
            int digitCount = 0;
            int ratio = 1;

            sFindSurroundingNumbers(tmpX, tmpY, width, height, start, [&](int digit) {
                digitCount++;
                ratio *= digit;
            });

            if(digitCount == 2)
            {
                sum += ratio;
            }

            mask = mask >> 1;
            tmpX++;
        }

        x += 16;
        if(x >= width)
        {
            x = x - width - 1;
            y++;
        }
        ++ptr;
        ++data;
    };
    if(printOut)
        printf("3B: Sum of gear ratios: %i\n", sum);

}


#ifndef RUNNER
int main()
{
    sParse03A(data03A, true);
    sParse03B(data03A, true);
    return 0;
}
#endif


void run03A(bool printOut)
{
    sParse03A(data03A, printOut);
}
void run03B(bool printOut)
{
    sParse03B(data03A, printOut);
}
