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

alignas(16) static constexpr char test10A[] =
    R"(.....
.S-7.
.|.|.
.L-J.
.....
)";

alignas(16) static constexpr char test10B[] =
    R"(..F7.
.FJ|.
SJ.L7
|F--J
LJ...
)";

alignas(16) static constexpr char test10C[] =
    R"(...........
.S-------7.
.|F-----7|.
.||.....||.
.||.....||.
.|L-7.F-J|.
.|..|.|..|.
.L--J.L--J.
...........
)";

alignas(16) static constexpr char test10D[] =
    R"(.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...
)";

alignas(16) static constexpr char test10E[] =
    R"(FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L
)";

struct Dir
{
    const char* pos;
    char canGoDir;
};


struct Map
{
    char flags;
};


static const uint8_t HasPipe = 0xff;

static const uint8_t LeftDir = 1;
static const uint8_t RightDir = 2;
static const uint8_t UpDir = 4;
static const uint8_t DownDir = 8;
static const uint8_t AllDirs = LeftDir | RightDir | UpDir | DownDir;

static int sCalculatePosition(const char* pos, const char* start)
{
    intptr_t diff = pos - start;
    return int(diff);
}

static void sGetMapSize(const char* data, int& width, int& height)
{
    width = 1;
    height = 1;
    while(*data++ != '\n') width++;
    while(*data)
    {
        data += width;
        ++height;
    }
}


/*
static void sDrawMap(const uint8_t* map, int width, int height)
{
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            uint8_t c = map[j * width + i];
            for (int k = 0; k < 8; ++k)
            {
                if (c & 1)
                {
                    printf("*");
                }
                else
                {
                    printf(".");

                }
                c >>= 1;
            }
        }
        printf("\n");

    }
}
*/


template<typename T>
static int64_t sMarkEdge(const char* data, int width, T&& lambdaFn)
{
    const char* begin = data;
    const char* startChar = data;

    int64_t steps = 0;
    while(*data != 'S')data++;
    startChar = data;
    while(*data) data++;

    std::vector<Dir> positions;

    uint8_t canGoDir = 0;
    {
        if((startChar - width) >= begin)
        {
            switch (*(startChar - width))
            {
                case 'F':
                case '|':
                case '7':
                    canGoDir |= UpDir;
                    positions.push_back(Dir{.pos = startChar, .canGoDir = UpDir});
                    break;
            }
        }
        if((startChar + width) < data)
        {
            switch (*(startChar + width))
            {
                case 'J':
                case '|':
                case 'L':
                    canGoDir |= DownDir;
                    positions.push_back(Dir{.pos = startChar, .canGoDir = DownDir});
                    break;
            }
        }
        if((startChar - 1) >= begin)
        {
            switch (*(startChar - 1))
            {
                case 'F':
                case '-':
                case 'L':
                    canGoDir |= LeftDir;
                    positions.push_back(Dir{.pos = startChar, .canGoDir = LeftDir});
                    break;
            }
        }
        if((startChar + 1) < data)
        {
            switch (*(startChar + 1))
            {
                case '7':
                case '-':
                case 'J':
                    canGoDir |= RightDir;
                    positions.push_back(Dir{.pos = startChar, .canGoDir = RightDir});
                    break;
            }
        }

    }

    if(canGoDir == (LeftDir | UpDir))
        lambdaFn(startChar, 'J');
    else if(canGoDir == (LeftDir | DownDir))
        lambdaFn(startChar, '7');
    else if(canGoDir == (RightDir | UpDir))
        lambdaFn(startChar, 'L');
    else if(canGoDir == (RightDir | DownDir))
        lambdaFn(startChar, 'F');
    else if(canGoDir == (LeftDir | RightDir))
        lambdaFn(startChar, '-');
    else if(canGoDir == (UpDir | DownDir))
        lambdaFn(startChar, '|');

    bool found = false;
    while(!found)
    {
        for(Dir& pos : positions)
        {
            const char *right = (pos.pos + 1);
            const char *left = (pos.pos - 1);
            const char *down = pos.pos + width;
            const char *up = pos.pos - width;

            switch(pos.canGoDir)
            {
                case RightDir:
                {
                    if (*right == 'J')
                    {
                        pos.pos = right;
                        pos.canGoDir = UpDir;
                        lambdaFn(right, *right);
                    }
                    else if (*right == '7')
                    {
                        pos.pos = right;
                        pos.canGoDir = DownDir;
                        lambdaFn(right, *right);
                    }
                    else if (*right == '-')
                    {
                        pos.pos = right;
                        pos.canGoDir = RightDir;
                        lambdaFn(right, *right);
                    }
                    break;
                }

                case LeftDir:
                {
                    if (*left == 'L')
                    {
                        pos.pos = left;
                        pos.canGoDir = UpDir;
                        lambdaFn(left, *left);
                    }
                    else if (*left == 'F')
                    {
                        pos.pos = left;
                        pos.canGoDir = DownDir;
                        lambdaFn(left, *left);
                    }
                    else if (*left == '-')
                    {
                        pos.pos = left;
                        pos.canGoDir = LeftDir;
                        lambdaFn(left, *left);
                    }
                    break;
                }

                case UpDir:
                {
                    if (*up == 'F')
                    {
                        pos.pos = up;
                        pos.canGoDir = RightDir;
                        lambdaFn(up, *up);
                    }
                    else if (*up == '7')
                    {
                        pos.pos = up;
                        pos.canGoDir = LeftDir;
                        lambdaFn(up, *up);
                    }
                    else if (*up == '|')
                    {
                        pos.pos = up;
                        pos.canGoDir = UpDir;
                        lambdaFn(up, *up);
                    }
                    break;
                }

                case DownDir:
                {
                    if (*down == 'L')
                    {
                        pos.pos = down;
                        pos.canGoDir = RightDir;
                        lambdaFn(down, *down);
                    }
                    else if (*down == 'J')
                    {
                        pos.pos = down;
                        pos.canGoDir = LeftDir;
                        lambdaFn(down, *down);
                    }
                    else if (*down == '|')
                    {
                        pos.pos = down;
                        pos.canGoDir = DownDir;
                        lambdaFn(down, *down);
                    }
                    break;
                }
            }
        }
        found = positions[0].pos == positions[1].pos;
        ++steps;
    }
    return steps;
}

static void sWiggleBits(uint64_t* currentMask, uint64_t wallMask)
{
    {
        uint64_t miniMask = 0;
        do
        {
            miniMask = 0;
            uint64_t startValue = *currentMask;

            *currentMask |= (*currentMask) >> 1;
            *currentMask |= (*currentMask) << 1;

            *currentMask &= wallMask;
            miniMask = startValue ^ *currentMask;
        } while (miniMask);
    }
}

static uint64_t sFillMap(uint8_t* __restrict__ map, const uint8_t* __restrict__ wallMap, int row, int mapWidth)
{
    uint64_t mask = 0;

    uint64_t prevOne = 0;
    int64_t mapWidthOffset = mapWidth / 8;

    const uint64_t* __restrict__ walls = (const uint64_t * __restrict__) (wallMap + row * mapWidth);
    uint64_t* __restrict__ currentMask = (uint64_t* __restrict__ )(map + row * mapWidth);

    for (int i = 0; i < mapWidthOffset - 1; ++i)
    {
        uint64_t old = *currentMask;
        uint64_t wallMask = ~(*walls);

        *currentMask |= *(currentMask - mapWidthOffset);
        *currentMask |= *(currentMask + mapWidthOffset);

        *currentMask |= prevOne >> 63;

        // reading next byte.
        uint64_t nextRead = *(currentMask + 1);
        *currentMask |= nextRead << 63;

        *currentMask &= wallMask;

        sWiggleBits(currentMask, wallMask);
        // Do not set the walls
        //*nextMask &= wallMask;
        // Check the mask
        mask |= old ^ *currentMask;

        prevOne = *currentMask;

        ++currentMask;
        ++walls;
    }

    {
        uint64_t old = *currentMask;
        uint64_t wallMask = ~(*walls);

        *currentMask |= *(currentMask - mapWidthOffset);
        *currentMask |= *(currentMask + mapWidthOffset);

        *currentMask |= prevOne << 63;
        // cannot read next on last 8 bytes.

        *currentMask &= wallMask;

        sWiggleBits(currentMask, wallMask);
        // Do not set the walls
        //*nextMask &= wallMask;
        // Check the mask
        mask |= old ^ *currentMask;
    }
    return mask;
}





static int64_t sParse10A(const char* data)
{
    int width = 0;
    int height = 0;

    sGetMapSize(data, width, height);
    int64_t steps = sMarkEdge(data, width, [&](const char*, char){});

    return steps;
}








static int64_t sParse10BLine(const char* data)
{
    int width = 0;
    int height = 0;
    int mapWidth = 0;
    std::vector<uint8_t> map;
    std::vector<uint8_t> pipeMap;

    std::vector<uint8_t> verticalMap;
    std::vector<uint8_t> leftMap;
    std::vector<uint8_t> rightMap;
    //std::vector<uint8_t> foundMap;

    sGetMapSize(data, width, height);

    mapWidth = ((width + 127) / 128) * 128 / 8;

    pipeMap.resize(mapWidth * height, 0);
    verticalMap.resize(mapWidth * height, 0);
    leftMap.resize(mapWidth * height, 0);
    rightMap.resize(mapWidth * height, 0);

    //foundMap.resize(mapWidth * height, 0);
    {
        TIMEDSCOPE("A part");
        sMarkEdge(data, width, [&](const char* pos, char c) {
            int mapPos = sCalculatePosition(pos, data);
            int x = mapPos % width;
            int y = mapPos / width;

            int mapX = x / 8;
            int bit = x % 8;
            uint8_t addBit = 1 << bit;

            int index = y * mapWidth + mapX;
            if(c == '-')
                verticalMap[index] |= addBit;
            switch(c)
            {
                case 'F':
                case 'L':
                    rightMap[index] |= addBit;
                    break;
            }
            switch(c)
            {
                case '7':
                case 'J':
                    leftMap[index] |= addBit;
                    break;
            }

            pipeMap[index] |= addBit;
        });
    }

    //sDrawMap(pipeMap.data(), mapWidth, height);
    //printf("\n\n");
    //sDrawMap(map.data(), mapWidth, height);
    //printf("\n\n");

    int64_t area = 0;

    for(int i = 0; i < mapWidth; i += 8)
    {
        uint64_t inside = 0;
        uint64_t goingRights = 0;
        uint64_t goingLefts = 0;
        for(int j = 0; j < height; ++j)
        {
            uint64_t pipes = *(uint64_t*)(&pipeMap[i + j * mapWidth]);

            uint64_t lefts = *(uint64_t*)(&leftMap[i + j * mapWidth]);
            uint64_t rights = *(uint64_t*)(&rightMap[i + j * mapWidth]);


            inside = inside ^ (rights & goingLefts);
            inside = inside ^ (lefts & goingRights);

            goingLefts = (lefts ^ goingLefts);
            goingRights = (rights ^ goingRights);


            uint64_t verticals = *(uint64_t*)(&verticalMap[i + j * mapWidth]);
            inside = inside ^ verticals;

            uint64_t insidePoints = (~pipes) & inside;

            area += std::popcount(insidePoints);
        }
    }
    //printf("\n");
    //sDrawMap(foundMap.data(), mapWidth, height);

    return area;
}







static int64_t sParse10B(const char* data)
{
    int width = 0;
    int height = 0;
    std::vector<Map> map;
    {
        TIMEDSCOPE("A part");
        sGetMapSize(data, width, height);

        map.resize(width * height, Map{});
        sMarkEdge(data, width, [&](const char* pos, char) {
            int mapPos = sCalculatePosition(pos, data);
            map[mapPos].flags |= HasPipe;
        });
    }
    std::vector<uint8_t> map4x3;


    int mapWidth = width * 4;
    mapWidth = ((mapWidth + 127) / 128) * 128 / 8;
    int mapHeight = height * 3;
    map4x3.resize(mapWidth * mapHeight);

    std::vector<uint8_t> map4x3Mask;
    map4x3Mask.resize(mapWidth * mapHeight);
    for(int i = 0; i < mapWidth; ++i)
    {
        map4x3Mask[i] = 0xff;
        map4x3Mask[i + (mapHeight - 1) * mapWidth] = 0xff;
    }
    // Zoom map by 4x3.

    {
        TIMEDSCOPE("Zoom map");
        for (int j = 0; j < height; j++)
        {
            uint8_t *line08 = map4x3.data() + (j * 3 + 0) * mapWidth;
            uint8_t *line18 = map4x3.data() + (j * 3 + 1) * mapWidth;
            uint8_t *line28 = map4x3.data() + (j * 3 + 2) * mapWidth;

            uint32_t *line0 = (uint32_t *) line08;
            uint32_t *line1 = (uint32_t *) line18;
            uint32_t *line2 = (uint32_t *) line28;

            for (int i = 0; i < width; ++i)
            {
                int index = i + j * width;

                uint32_t offset = 1 << ((i % 8) * 4);
                if (data[index] == 'S')
                {
                    *line0 |= 0b0100 * offset;
                    *line1 |= 0b1111 * offset;
                    *line2 |= 0b0100 * offset;
                }
                else if (map[index].flags & HasPipe)
                {
                    switch (data[index])
                    {
                        case 'F':
                            *line0 |= uint32_t(0b0000) * offset;
                            *line1 |= uint32_t(0b1100) * offset;
                            *line2 |= uint32_t(0b0100) * offset;
                            break;
                        case 'L':
                            *line0 |= uint32_t(0b0100) * offset;
                            *line1 |= uint32_t(0b1100) * offset;
                            *line2 |= uint32_t(0b0000) * offset;
                            break;
                        case '7':
                            *line0 |= uint32_t(0b0000) * offset;
                            *line1 |= uint32_t(0b0111) * offset;
                            *line2 |= uint32_t(0b0100) * offset;
                            break;
                        case 'J':
                            *line0 |= uint32_t(0b0100) * offset;
                            *line1 |= uint32_t(0b0111) * offset;
                            *line2 |= uint32_t(0b0000) * offset;
                            break;
                        case '|':
                            *line0 |= uint32_t(0b0100) * offset;
                            *line1 |= uint32_t(0b0100) * offset;
                            *line2 |= uint32_t(0b0100) * offset;
                            break;
                        case '-':
                            *line0 |= uint32_t(0b0000) * offset;
                            *line1 |= uint32_t(0b1111) * offset;
                            *line2 |= uint32_t(0b0000) * offset;
                            break;
                    }
                }
                else
                {
                    *line0 |= uint32_t(0b0000) * offset;
                    *line1 |= uint32_t(0b0000) * offset;
                    *line2 |= uint32_t(0b0000) * offset;
                }

                if ((i % 8) == 7)
                {
                    line0++;
                    line1++;
                    line2++;
                }
            }

            //sDrawMap(map4x3.data(), mapWidth, mapHeight);
            //printf("\n");
        }
    }

    //sDrawMap(map4x4MaskCurrent, mapWidth, mapHeight);
    //printf("\n\n");


    {
        TIMEDSCOPE("Fill map");
        uint64_t mask = 0;
        do
        {
            mask = 0;
            for (int j = 1; j < mapHeight - 1; ++j)
            {
                mask |= sFillMap(map4x3Mask.data(), map4x3.data(), j, mapWidth);
            }
            for (int j = mapHeight - 2; j > 0; --j)
            {
                mask |= sFillMap(map4x3Mask.data(), map4x3.data(), j, mapWidth);
            }
            //sDrawMap(map4x3MaskCurrent, mapWidth, mapHeight);
            //printf("\n\n");
        }  while (mask);
    }
    //sDrawMap(map4x3.data(), mapWidth, mapHeight);
    //printf("\n\n");

    //sDrawMap(map4x3Mask.data(), mapWidth, mapHeight);
    //printf("\n\n");


    int emptySpots = 0;
    {
        TIMEDSCOPE("Calculate empties");
        //printf("\n");
        //printf("\n");

        for (int j = 0; j < height; ++j)
        {
            for (int i = 0; i < mapWidth; ++i)
            {
                uint8_t mapMask = 0;

                mapMask |= map4x3Mask[i + (j * 3 + 0) * mapWidth];
                mapMask |= map4x3Mask[i + (j * 3 + 1) * mapWidth];
                mapMask |= map4x3Mask[i + (j * 3 + 2) * mapWidth];

                if ((mapMask & 0xf) == 0)
                {
                    emptySpots++;
                    //printf("O");
                }
                else
                {
                    //printf("*");
                }
                if ((mapMask & 0xf0) == 0)
                {
                    emptySpots++;
                    //printf("O");
                }
                else
                {
                    //printf("*");
                }
            }
            //printf("\n");
        }
    }
    return emptySpots;
}

#ifndef RUNNER
int main()
{
    printf("10A: Steps: %" PRIi64 "\n", sParse10A(data10A));
    printf("10B: Area: %" PRIi64 "\n", sParse10B(data10A));
    printf("10B: Area with in-out check: %" PRIi64 "\n", sParse10BLine(data10A));
    return 0;
}
#endif

int run10A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse10A(data10A);

    if(printOut)
        charsAdded = sprintf(buffer, "10A: Steps: %" PRIi64, aResult);
    return charsAdded;
}

int run10B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse10B(data10A);

    if(printOut)
        charsAdded = sprintf(buffer, "10B: Area: %" PRIi64, resultB);

    return charsAdded;
}

int run10C(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse10BLine(data10A);

    if(printOut)
        charsAdded = sprintf(buffer, "10B: Area: %" PRIi64, resultB);

    return charsAdded;
}

