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
static int hittrees = 0;
void sCheckMove(const char* data,
    int x,
    int y,
    int width,
    int height,
    int areaWidth,
    int areaHeight,
    std::unordered_set<int64_t>& points)
{
    if(x < 0 || x >= areaWidth || y < 0 || y >= areaHeight)
        return;

    int64_t p = x + (int64_t(y) << int64_t(32));
    if(points.contains(p))
        return;
    //assert(pointCount + 1 < PointsMax);
    char tile = data[(x % width) + (y % height) * (width + 1)];
    if(tile != '#')
        points.insert(p);
    else
        hittrees++;
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
            sCheckMove(dataStart, x1 + 1, y1, width, height, width, height, points2);
            sCheckMove(dataStart, x1 - 1, y1, width, height, width, height, points2);
            sCheckMove(dataStart, x1, y1 + 1, width, height, width, height, points2);
            sCheckMove(dataStart, x1, y1 - 1, width, height, width, height, points2);
        }
        std::swap(points1, points2);
    }
    return (int64_t)points1.size();
}

static int sCountTrees(const std::unordered_set<int64_t>& points1, int x0, int y0, int x1, int y1)
{
    int trees = 0;
    int midX = x0 + (x1 - x0) / 2;
    int midY = y0 + (y1 - y0) / 2;
    int points = 0;
    int points2 = 0;
    printf("[x0:%i, y0:%i] - [x1:%i, y1:%i] mid[midX:%i, midY:%i]\n", x0, y0, x1, y1, midX, midY);

    int row = 0;
    int height = y1 - y0 + 1;
    for(int j = y0; j <= midY; ++j)
    {
        for(int i = midX - row; i <= midX + row; i += 2)
        {
            ++points;
            if(!points1.contains(int64_t(i) + (int64_t(j) << (int64_t(32)))))
            {
                trees++;
            }
            else
            {
                printf("p: %i, %i\n", i, j);
                points2++;
            }
        }
        ++row;
    }
    for(int j = midY + 1; j < y1; ++j)
    {
        int offset = height - 1 - row;
        for(int i = midX - offset; i <= midX + offset; i += 2)
        {
            ++points;
            if(!points1.contains(int64_t(i) + (int64_t(j) << (int64_t(32)))))
            {
                trees++;
            }
            else
            {
                printf("p2: %i, %i\n", i, j);
                points2++;
            }
        }
        row++;
    }
    printf("points: %i, points2: %i, trees: %i\n", points, points2, trees);
    return trees;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("21B Total");
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);
    const char* dataStart = data;
    while(*data != 'S') data++;
    static constexpr int64_t MulMul = 1;
    static constexpr int64_t Center = 1;
    int x = intptr_t(data - dataStart) / (width + 1) + width * MulMul * Center;
    int y = intptr_t(data - dataStart) / (width + 1) + height * MulMul * Center;



    std::unordered_set<int64_t> points1 = {};
    points1.insert(x + (int64_t(y) << (int64_t(32))));
    std::unordered_set<int64_t> points2 = {};

    static constexpr int64_t Multi = (Center + 1) * MulMul + 1; //MulMul * 2 + 1;

    for(int i = 0; i < 65 + width * MulMul; ++i)
    {
        hittrees = 0;
        points2.clear();
        for(int64_t point : points1)
        {
            int x1 = point & (0xffff'ffff);
            int y1 = point >> 32;
            sCheckMove(dataStart, x1 + 1, y1, width, height, width * Multi, height * Multi, points2);
            sCheckMove(dataStart, x1 - 1, y1, width, height, width * Multi, height * Multi, points2);
            sCheckMove(dataStart, x1, y1 + 1, width, height, width * Multi, height * Multi, points2);
            sCheckMove(dataStart, x1, y1 - 1, width, height, width * Multi, height * Multi, points2);
        }
        std::swap(points1, points2);
    }
    printf("hittrees: %i\n", hittrees);



    char map[131 * 131 * Multi * Multi];
    for(int64_t j = 0; j < height * Multi; ++j)
    {
        for(int64_t i = 0; i < width * Multi; ++i)
        {
            map[i + j * width * Multi] = dataStart[(i % width) + (j % height) * (width + 1)];
            if(points1.contains(int64_t(i) + (int64_t(j) << (int64_t(32)))))
            {
                map[i + j * width * Multi] = 'O';
            }
        }
    }
    int64_t numbers[64 * 64 *  Multi * Multi] = {};
    for(int64_t j = 0; j < height * Multi; ++j)
    {
        for(int64_t i = 0; i < width * Multi; ++i)
        {
            int64_t xInd = i / width;
            int64_t yInd = j / height;
            numbers[xInd + yInd * Multi] += map[i + j * width * Multi] == 'O' ? 1 : 0;
        }
    }

    for(int64_t j = 0; j < height * Multi; ++j)
    {
        for(int64_t i = 0; i < width * Multi; ++i)
        {
            printf("%c", map[i + j * width * Multi]);
        }
        printf("\n");
    }

    for(int i = 0; i < Multi * Multi; ++i)
    {
        printf("%i count: %" PRIi64 "\n", i, numbers[i]);
    }

    int trees1 = sCountTrees(points1, 1, 0, width + 1, height + 0);
    printf("trees1: %i\n", trees1);

    int trees2 = sCountTrees(points1, 65, 65, 131 + 65, 131 + 65);
    printf("trees2: %i\n", trees2);


    return (int64_t)points1.size();
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

