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

alignas(16) static constexpr char test11A[] =
    R"(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....
)";

struct Point
{
    int32_t x;
    int32_t y;
};
static const int Width = 256 / 64;


int32_t sAddPoints(const uint64_t* bits, int32_t pos, int32_t gapMultiplier)
{
    int index = 0;

    int32_t returnValue = pos;
    while(pos >= 64)
    {
        returnValue += ((int32_t)std::popcount(bits[index])) * gapMultiplier;
        pos -= 64;
        index++;
    }
    if(pos)
    {
        returnValue += ((int32_t) std::popcount(bits[index] & ((uint64_t(1) << (pos)) - uint64_t(1)))) * gapMultiplier;
    }
    return returnValue;
}


static int sParseData(const char* data, Point* points, uint64_t* colsBits, uint64_t* rowsBits)
{
    TIMEDSCOPE("Parsing");
    int32_t x = 0;
    int32_t y = 0;

    int pointCount = 0;

    while(*data)
    {
        char c = *data;
        if(c == '\n')
        {
            x = -1;
            y++;
        }
        else if(c == '#')
        {
            colsBits[x / 64] |= (uint64_t(1) << (uint64_t(x) % uint64_t(64)));
            rowsBits[y / 64] |= (uint64_t(1) << (uint64_t(y) % uint64_t(64)));
            points[pointCount++] = {.x = x, .y = y};
        }
        x++;
        data++;
    }

    for (int i = 0; i < Width; ++i)
    {
        colsBits[i] = ~colsBits[i];
        rowsBits[i] = ~rowsBits[i];
    }
    return pointCount;
}

static int64_t sGetDistanceSum(const Point* points, int pointCount)
{
    TIMEDSCOPE("Get distance sum");

    int64_t sumOfPaths = 0;

    for(int j = 0; j < pointCount - 1; ++j)
    {
        for (int i = j + 1; i < pointCount; ++i)
        {
            int32_t diffX = int32_t(points[i].x) - int32_t(points[j].x);
            int32_t diffY = int32_t(points[i].y) - int32_t(points[j].y);
            diffX = diffX > 0 ? diffX : -diffX;
            diffY = diffY > 0 ? diffY : -diffY;
            sumOfPaths += diffX + diffY;
        }
    }

    return sumOfPaths;
}


static int64_t sParse11A(const char* data)
{
    TIMEDSCOPE("11A Total");

    Point points[512];

    uint64_t colBits[Width] = {};
    uint64_t rowBits[Width] = {};

    int pointCount = sParseData(data, points, colBits, rowBits);

    for(int i = 0; i < pointCount; ++i)
    {
        points[i].x = sAddPoints(colBits, points[i].x, 1);
        points[i].y = sAddPoints(rowBits, points[i].y, 1);
    }

    return sGetDistanceSum(points, pointCount);
}



static int64_t sParse11B(const char* data)
{
    TIMEDSCOPE("11B Total");

    Point points[512];

    uint64_t colBits[Width] = {};
    uint64_t rowBits[Width] = {};

    int pointCount = sParseData(data, points, colBits, rowBits);

    for(int i = 0; i < pointCount; ++i)
    {
        points[i].x = sAddPoints(colBits, points[i].x, 99999);
        points[i].y = sAddPoints(rowBits, points[i].y, 99999);
    }

    return sGetDistanceSum(points, pointCount);
}

#ifndef RUNNER
int main()
{
    printf("11A: Distances: %" PRIi64 "\n", sParse11A(data11A));
    printf("11B: Distances: %" PRIi64 "\n", sParse11B(data11A));
    return 0;
}
#endif

int run11A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse11A(data11A);

    if(printOut)
        charsAdded = sprintf(buffer, "11A: Distances: %" PRIi64, aResult);
    return charsAdded;
}

int run11B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse11B(data11A);

    if(printOut)
        charsAdded = sprintf(buffer, "11B: Distances: %" PRIi64, resultB);

    return charsAdded;
}

