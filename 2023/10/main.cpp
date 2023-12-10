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
    std::vector<Dir> positions2;

    positions.reserve(4);
    positions2.reserve(4);

    positions.push_back(Dir{.pos = startChar, .canGoDir = AllDirs});

    std::vector<Dir>* current = &positions;
    std::vector<Dir>* other = &positions2;

    bool found = false;
    while(!found)
    {
        assert(!current->empty());
        other->clear();
        for(Dir& pos : *current)
        {
            if(pos.canGoDir & RightDir)
            {
                const char *right = (pos.pos + 1);
                int mapPos = sCalculatePosition(right, begin);
                if (*right == 'J')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = UpDir});
                    lambdaFn(mapPos);
                }
                else if (*right == '7')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = DownDir});
                    lambdaFn(mapPos);
                }
                else if (*right == '-')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = RightDir});
                    lambdaFn(mapPos);
                }
            }

            if(pos.canGoDir & LeftDir)
            {
                const char *left = (pos.pos - 1);
                int mapPos = sCalculatePosition(left, begin);
                if (*left == 'L')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = UpDir});
                    lambdaFn(mapPos);
                }
                else if (*left == 'F')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = DownDir});
                    lambdaFn(mapPos);
                }
                else if (*left == '-')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = LeftDir});
                    lambdaFn(mapPos);
                }
            }

            if((pos.pos - width) >= begin && (pos.canGoDir & UpDir))
            {
                const char *up = pos.pos - width;
                int mapPos = sCalculatePosition(up, begin);
                if (*up == 'F')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = RightDir});
                    lambdaFn(mapPos);
                }
                else if (*up == '7')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = LeftDir});
                    lambdaFn(mapPos);
                }
                else if (*up == '|')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = UpDir});
                    lambdaFn(mapPos);
                }
            }

            if((pos.pos + width) < data && (pos.canGoDir & DownDir))
            {
                const char *down = pos.pos + width;
                int mapPos = sCalculatePosition(down, begin);
                if (*down == 'L')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = RightDir});
                    lambdaFn(mapPos);
                }
                else if (*down == 'J')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = LeftDir});
                    lambdaFn(mapPos);
                }
                else if (*down == '|')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = DownDir});
                    lambdaFn(mapPos);
                }
            }
        }

        ++steps;
        for(size_t i = 0; i < other->size() && !found; ++i)
        {
            const char* search = other->at(i).pos;
            for(size_t j = i + 1; j < other->size(); ++j)
            {
                if(other->at(j).pos == search)
                {
                    found = true;
                    break;
                }
            }
        }

        std::swap(current, other);
    }
    return steps;
}

static int64_t sParse10A(const char* data)
{
    int width = 0;
    int height = 0;

    sGetMapSize(data, width, height);
    int64_t steps = sMarkEdge(data, width, [&](int){});

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

static int64_t sParse10B(const char* data)
{
    int width = 0;
    int height = 0;
    std::vector<Map> map;
    {
        TIMEDSCOPE("A part");
        sGetMapSize(data, width, height);

        map.resize(width * height, Map{});
        sMarkEdge(data, width, [&](int mapPos) {
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

