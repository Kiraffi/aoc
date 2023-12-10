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

struct MapTile
{
    //int position;
    char tile;
    //int fromPos;
    //char canSpreadDir;
};



static const char Visited = 1;
static const char IsNotValid = 2;
static const char IsValid = 4;
static const char HasPipe = 8;

static const char LeftDir = 1;
static const char RightDir = 2;
static const char UpDir = 4;
static const char DownDir = 8;
static const char AllDirs = LeftDir | RightDir | UpDir | DownDir;

static int sCalculatePosition(const char* pos, const char* start)
{
    intptr_t diff = pos - start;
    return int(diff);
}
/*
static void sSet3x3Pattern(
    std::vector<char>& map3x3,
    size_t i,
    const char* row1,
    const char* row2,
    const char* row3,
    int width)
{
    size_t x = i % width;
    size_t y = i / width;
    x *= 3;
    y *= 3;
    const char* rows[3] = {row1, row2, row3};
    for(size_t jj = 0; jj < 3; ++jj)
    {
        const char* row = rows[jj];
        for (size_t j = 0; j < 3; ++j)
        {
            size_t index = x + j + (y + jj) * width * 3;
            map3x3[index] = row[j];
        }
    }
}
*/
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

static int64_t sParse10B(const char* data)
{
    int width = 0;
    int height = 0;
    sGetMapSize(data, width, height);

    std::vector<Map> map;
    map.resize(width * height, Map{});
    sMarkEdge(data, width, [&](int mapPos){
        map[mapPos].flags |= HasPipe;
    });

    std::vector<uint8_t> map3x3;


    int mapWidth = width * 4;
    mapWidth = ((mapWidth + 31) / 32) * 32 / 8;
    int mapHeight = height * 3;
    map3x3.resize(mapWidth * mapHeight);

    std::vector<uint8_t> map4x4Mask;
    map4x4Mask.resize(mapWidth * mapHeight);
    for(int i = 0; i < mapWidth; ++i)
        map4x4Mask[i] = 0xff;

    std::vector<uint8_t> map4x4Mask2;
    map4x4Mask2.resize(mapWidth * mapHeight);
    for(int i = 0; i < mapWidth; ++i)
        map4x4Mask2[i] = 0xff;


    uint8_t* map4x4MaskCurrent = map4x4Mask.data();
    uint8_t* map4x4MaskOther = map4x4Mask2.data();

    // Zoom map by 4x4.


    for(int j = 0; j < height; j++)
    {
        uint32_t* line0 = (uint32_t*)(map3x3.data() + (j * 3 + 0) * mapWidth);
        uint32_t* line1 = (uint32_t*)(map3x3.data() + (j * 3 + 1) * mapWidth);
        uint32_t* line2 = (uint32_t*)(map3x3.data() + (j * 3 + 2) * mapWidth);

        //*line08 = 0;
        //*line18 = 0;
        //*line28 = 0;

        //*line0 = 0;
        //*line1 = 0;
        //*line2 = 0;

        for(int i = 0; i < width; ++i)
        {
            int index = i + j * width;

            int offset = 1 << ((i % 8) * 4);
            if(data[index] == 'S')
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
                        *line0 |= 0b0000 * offset;
                        *line1 |= 0b1100 * offset;
                        *line2 |= 0b0100 * offset;
                        break;
                    case 'L':
                        *line0 |= 0b0100 * offset;
                        *line1 |= 0b1100 * offset;
                        *line2 |= 0b0000 * offset;
                        break;
                    case '7':
                        *line0 |= 0b0000 * offset;
                        *line1 |= 0b0111 * offset;
                        *line2 |= 0b0100 * offset;
                        break;
                    case 'J':
                        *line0 |= 0b0100 * offset;
                        *line1 |= 0b0111 * offset;
                        *line2 |= 0b0000 * offset;
                        break;
                    case '|':
                        *line0 |= 0b0100 * offset;
                        *line1 |= 0b0100 * offset;
                        *line2 |= 0b0100 * offset;
                        break;
                    case '-':
                        *line0 |= 0b0000 * offset;
                        *line1 |= 0b1111 * offset;
                        *line2 |= 0b0000 * offset;
                        break;
                }
            }

            else
            {
                *line0 |= 0b0000 * offset;
                *line1 |= 0b0000 * offset;
                *line2 |= 0b0000 * offset;
            }

            if((i % 8) == 7)
            {
                line0++;
                line1++;
                line2++;

                *line0 = 0;
                *line1 = 0;
                *line2 = 0;
            }
        }
//        int shiftAmount = ((8 - (width % 8)) % 8) * 4;
//        *line0 <<= shiftAmount;
//        *line1 <<= shiftAmount;
//        *line2 <<= shiftAmount;


        //sDrawMap(map3x3.data(), mapWidth, mapHeight);
        //printf("\n");
    }
    /*
    for(size_t i = 0; i < map.size(); ++i)
    {
        if (map[i].flags & HasPipe)
        {
            switch (data[i])
            {
                case 'F': sSet3x3Pattern(map3x3, i, "...", ".**", ".*.", width); break;
                case 'L': sSet3x3Pattern(map3x3, i, ".*.", ".**", "...", width); break;
                case '7': sSet3x3Pattern(map3x3, i, "...", "**.", ".*.", width); break;
                case 'J': sSet3x3Pattern(map3x3, i, ".*.", "**.", "...", width); break;
                case '|': sSet3x3Pattern(map3x3, i, ".*.", ".*.", ".*.", width); break;
                case '-': sSet3x3Pattern(map3x3, i, "...", "***", "...", width); break;
            }
        }
        else if(data[i] == 'S')
        {
            sSet3x3Pattern(map3x3, i, ".*.", "***", ".*.", width);
        }
        else
        {
            sSet3x3Pattern(map3x3, i, "...", "...", "...", width);
        }
    }
     */

    //sDrawMap(map3x3.data(), mapWidth, mapHeight);

    //sDrawMap(map4x4MaskCurrent, mapWidth, mapHeight);
    //printf("\n\n");

    uint32_t mask = 1; // could be 0
    while(mask)
    {
        mask = 0;
        uint8_t* walls = map3x3.data();
        uint8_t* nextMask = map4x4MaskOther;

        uint8_t* currentMask = map4x4MaskCurrent;

        uint8_t* currentStart = map4x4MaskCurrent;
        uint8_t* currentEnd = map4x4MaskCurrent + mapWidth * mapHeight;

        for(int j = 0; j < mapHeight; ++j)
        {
            uint8_t prevOne = 0;

            for (int i = 0; i < mapWidth; ++i)
            {
                uint8_t old = *currentMask;

                *nextMask = *currentMask;
                *nextMask |= (*currentMask) >> 1;
                *nextMask |= (*currentMask) << 1;

                if (currentMask - mapWidth >= currentStart)
                {
                    *nextMask |= *(currentMask - mapWidth);
                }

                if (currentMask + mapWidth < currentEnd)
                {
                    *nextMask |= *(currentMask + mapWidth);
                }

                prevOne >>= 7;
                *nextMask |= prevOne;

                if (i < mapWidth - 1)
                {
                    uint8_t nextRead = *(currentMask + 1);
                    nextRead <<= 7;
                    *nextMask |= nextRead;
                }
                // Do not set the walls
                *nextMask &= ~(*walls);

                // Check the mask
                mask |= old ^ *nextMask;
                prevOne = *currentMask;

                ++nextMask;
                ++walls;
                ++currentMask;
            }
        }
        std::swap(map4x4MaskCurrent, map4x4MaskOther);

        //sDrawMap(map4x4MaskCurrent, mapWidth, mapHeight);
        //printf("\n\n");
    }




    int emptySpots = 0;
    for(int j = 0; j < height; ++j)
    {
        for(int i = 0; i < mapWidth; ++i)
        {
            uint8_t mapMask = 0;

            mapMask |= map4x4MaskCurrent[i + (j * 3 + 0) * mapWidth];
            mapMask |= map4x4MaskCurrent[i + (j * 3 + 1) * mapWidth];
            mapMask |= map4x4MaskCurrent[i + (j * 3 + 2) * mapWidth];

            if((mapMask & 0xf) == 0)
                emptySpots++;
            if((mapMask & 0xf0) == 0)
                emptySpots++;
        }
    }
    /*
    for(size_t i = 0; i < map3x3.size(); ++i)
    {
        uint8_t walls = map3x3[i] | map4x4Mask[i];
        walls = ~walls;
        emptySpots += std::popcount(walls);
    }
*/

/*
    std::vector<int> tiles;
    std::vector<int> newTiles;
    tiles.reserve(256);
    newTiles.reserve(256);

    std::vector<int>* currentTiles = &tiles;
    std::vector<int>* otherTiles = &newTiles;

    tiles.push_back(0);
    while(!(*currentTiles).empty())
    {
        otherTiles->clear();
        for (int tileIndex: *currentTiles)
        {
            if (tileIndex < 0 || tileIndex >= (int)map3x3.size())
                continue;
            char& tile = map3x3[tileIndex];
            if (tile == '.')
            {
                tile = '-';
                otherTiles->push_back(tileIndex - 1);
                otherTiles->push_back(tileIndex + 1);
                otherTiles->push_back(tileIndex - width * 3);
                otherTiles->push_back(tileIndex + width * 3);
            }
        }
        std::swap(currentTiles, otherTiles);
        // * print map
        for(int tmp = 0; tmp < (int)map3x3.size(); ++tmp)
        {
            printf("%c", map3x3[tmp]);
            if((tmp % (width * 3)) >= width * 3 - 1)
            {
                printf("\n");
            }
        }
        printf("\n\n");
    }
        */

    /*
    int emptySpots = 0;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width - 1; x++)
        {
            static constexpr int tripleDot = '.' * 0x10101;
            bool isEmpty = true;
            int index = y * 9 * width + x * 3;
            // Does some unaligned memory read.
            isEmpty &= ((*(int*)(&map3x3[index + 0 * width * 3])) & 0xffffff) == tripleDot;
            isEmpty &= ((*(int*)(&map3x3[index + 1 * width * 3])) & 0xffffff) == tripleDot;
            isEmpty &= ((*(int*)(&map3x3[index + 2 * width * 3])) & 0xffffff) == tripleDot;
            if(isEmpty)
            {
                emptySpots++;
            }
        }
    }
     */
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

