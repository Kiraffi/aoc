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

struct Point
{
    int x;
    int y;
};

void sCheckMove(const char* data,
    int x,
    int y,
    int width,
    int height,
    std::unordered_set<int64_t>& points)
{
    if(x < 0 || x >= width || y < 0 || y >= height)
        return;

    int64_t p = x + (int64_t(y) << int64_t(32));
    if(points.contains(p))
        return;
    char tile = data[(x % width) + (y % height) * (width + 1)];
    if(tile != '#')
        points.insert(p);
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

int64_t sGetFillCount(const char* data, int x, int y, int width, int height, int steps)
{
    std::unordered_set<int64_t> points1 = {};
    points1.insert(x + (int64_t(y) << (int64_t(32))));
    std::unordered_set<int64_t> points2 = {};

    for(int i = 0; i < steps; ++i)
    {
        points2.clear();
        for(int64_t point : points1)
        {
            int x1 = point & (0xffff'ffff);
            int y1 = point >> 32;
            sCheckMove(data, x1 + 1, y1, width, height, points2);
            sCheckMove(data, x1 - 1, y1, width, height, points2);
            sCheckMove(data, x1, y1 + 1, width, height, points2);
            sCheckMove(data, x1, y1 - 1, width, height, points2);
        }
        std::swap(points1, points2);
    }
    return int64_t(points1.size());
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

    return sGetFillCount(data, x, y, width, height, 64);
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

    static constexpr int Steps = 26501365;
    assert(Steps % width == x);

    int64_t grids = Steps / width - 1;
    int64_t oddGrids = (grids / 2 * 2 + 1);
    oddGrids *= oddGrids;
    int64_t evenGrids = ((grids + 1) / 2 * 2);
    evenGrids *= evenGrids;

    int64_t oddCount = sGetFillCount(data,  x, y, width, height, width * 3 / 2 + 1);
    int64_t evenCount = sGetFillCount(data, x, y, width, height, width * 3 / 2);

    int64_t uCount = sGetFillCount(data, x,         height - 1, width, height, height - 1);
    int64_t dCount = sGetFillCount(data, x,         0,          width, height, height - 1);
    int64_t lCount = sGetFillCount(data, width - 1, y,          width, height, width - 1);
    int64_t rCount = sGetFillCount(data, 0,         y,          width, height, width - 1);

    int64_t trCount = sGetFillCount(data, 0,         height - 1, width, height, width / 2 - 1);
    int64_t tlCount = sGetFillCount(data, width - 1, height - 1, width, height, width / 2 - 1);
    int64_t brCount = sGetFillCount(data, 0,         0,          width, height, width / 2 - 1);
    int64_t blCount = sGetFillCount(data, width - 1, 0,          width, height, width / 2 - 1);

    int64_t trbCount = sGetFillCount(data, 0,         height - 1, width, height, width * 3 / 2 - 1);
    int64_t tlbCount = sGetFillCount(data, width - 1, height - 1, width, height, width * 3 / 2 - 1);
    int64_t brbCount = sGetFillCount(data, 0,         0,          width, height, width * 3 / 2 - 1);
    int64_t blbCount = sGetFillCount(data, width - 1, 0,          width, height, width * 3 / 2 - 1);

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

