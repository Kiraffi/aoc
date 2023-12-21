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

#define PROFILE 1
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

/*
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
*/
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
    while(**data >= '0' && **data <= '9')
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
/*
template <typename T>
static void sMemset(T* arr, T value, int amount)
{
    const T* end = arr + amount;
    while(arr < end)
    {
        *arr++ = value;
    }

}
*/

struct Point
{
    int x;
    int y;
};

void sCheckMove(const char* data, int x, int y, int width, int height, std::unordered_set<int64_t>& points)
{
    //if(x < 0 || x >= width || y < 0 || y >= height)
    //    return;

    int64_t p = x + (int64_t(y) << int64_t(32));
    if(points.contains(p))
        return;
    //assert(pointCount + 1 < PointsMax);
    char tile = data[(x % width) + (y % height) * (width + 1)];
    if(tile == '.')
        points.insert(p);
}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("21A Total");
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);
    const char* dataStart = data;
    while(*data != 'S') data++;
    int x = intptr_t(data - dataStart) / (width + 1);
    int y = intptr_t(data - dataStart) / (width + 1);


    std::unordered_set<int64_t> points1 = {};
    points1.insert(x + (int64_t(y) << (int64_t(32))));
    std::unordered_set<int64_t> points2 = {};

    for(int i = 0; i < 64; ++i)
    {
        points2.clear();
        for(int64_t point : points1)
        {
            int x1 = point & (0xffff'ffff);
            int y1 = point >> 32;
            sCheckMove(dataStart, x1 + 1, y1, width, height, points2);
            sCheckMove(dataStart, x1 - 1, y1, width, height, points2);
            sCheckMove(dataStart, x1, y1 + 1, width, height, points2);
            sCheckMove(dataStart, x1, y1 - 1, width, height, points2);
        }
        std::swap(points1, points2);
    }
    char map[131 * 131 * 16] = {};
    for(int j = 0; j < height * 4; ++j)
    {
        for(int i = 0; i < width * 4; ++i)
        {
            map[i + j * width * 4] = dataStart[(i % width) + (j % height) * (width + 1)];
            if(points1.find(int64_t(i) + (int64_t(j) << (int64_t(32)))) != points1.end())
            {
                map[i + j * width] = 'O';
            }
        }
    }
    for(int j = 0; j < height * 4; ++j)
    {
        for(int i = 0; i < width * 4; ++i)
        {
            printf("%c", map[i + j * width * 4]);
        }
        printf("\n");
    }

    return (int64_t)points1.size();
}


static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("21B Total");

    return *data;
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

