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

static void sGetMapSize(const char* data, int& width, int& height)
{
    width = 0;
    height = 1;
    while(*data++ != '\n') width++;
    while(*data)
    {
        data += width;
        ++height;
    }
}

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

    positions.reserve(64);
    positions2.reserve(64);

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
#if 0
    const char* start = data;
    int tmpWidth = 0;
    while(*data != 'S')
    {
        tmpWidth++;
        if(*data == '\n')
        {
            width = width == 0 ? tmpWidth : width;
            tmpWidth = 0;
            ++height;
        }
        ++data;
    }
    std::vector<Dir> positions;
    std::vector<Dir> positions2;
    positions.push_back(Dir{.pos = data, .prev = data, .canGoDir = AllDirs});

    while(*data)
    {
        if(*data == '\n')
        {
            ++height;
        }
        data++;
    }

    std::vector<Dir>* current = &positions;
    std::vector<Dir>* other = &positions2;

    int64_t moves = 0;
    bool found = false;
    while(!found)
    {
        other->clear();
        for(Dir& pos : *current)
        {
            //bool wasLeft = pos.prev == (pos.pos - 1);
            //bool wasRight = pos.prev == (pos.pos + 1);
            //bool wasUp = pos.prev == (pos.pos + width);
            //bool wasDown = pos.prev == (pos.pos + width);
            if(pos.canGoDir & RightDir)
            {
                const char *right = (pos.pos + 1);

                if (*right == 'J')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = UpDir});
                }
                else if (*right == '7')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = DownDir});
                }
                else if (*right == '-')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = RightDir});
                }
            }

            if(pos.canGoDir & LeftDir)
            {
                const char *left = (pos.pos - 1);
                if (*left == 'L')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = UpDir});
                }
                else if (*left == 'F')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = DownDir});
                }
                else if (*left == '-')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = LeftDir});
                }
            }

            if((pos.pos - width) >= start && (pos.canGoDir & UpDir))
            {
                const char *up = (pos.pos - width - 0);
                if (*up == 'F')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = RightDir});
                }
                else if (*up == '7')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = LeftDir});
                }
                else if (*up == '|')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = UpDir});
                }
            }

            if((pos.pos + width) < data && (pos.canGoDir & DownDir))
            {
                const char *down = (pos.pos + width + 0);
                if (*down == 'L')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = RightDir});
                }
                else if (*down == 'J')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = LeftDir});
                }
                else if (*down == '|')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = DownDir});
                }
            }
        }

        ++moves;
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
#endif
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
#if 0
    const char* start = data;
    const char* startPos = data;
    int width = 0;
    int height = 0;
    int tmpWidth = 0;
    while(*data)
    {
        tmpWidth++;
        if(*data == 'S')
            startPos = data;
        if(*data == '\n')
        {
            width = width == 0 ? tmpWidth : width;
            tmpWidth = 0;
            ++height;
        }
        ++data;
    }
    std::vector<Dir> positions;
    std::vector<Dir> positions2;
    positions.push_back(Dir{.pos = startPos, .prev = data, .canGoDir = AllDirs});

    std::vector<Map> map(width * height, Map{});

    std::vector<Dir>* current = &positions;
    std::vector<Dir>* other = &positions2;

    bool found = false;
    while(!found)
    {
        other->clear();
        for(Dir& pos : *current)
        {
            if(pos.canGoDir & RightDir)
            {
                const char *right = (pos.pos + 1);
                int mapPos = sCalculatePosition(right, start);
                if (*right == 'J')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = UpDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*right == '7')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = DownDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*right == '-')
                {
                    other->push_back(Dir{.pos = right, .canGoDir = RightDir});
                    map[mapPos].flags |= HasPipe;
                }
            }

            if(pos.canGoDir & LeftDir)
            {
                const char *left = (pos.pos - 1);
                int mapPos = sCalculatePosition(left, start);
                if (*left == 'L')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = UpDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*left == 'F')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = DownDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*left == '-')
                {
                    other->push_back(Dir{.pos = left, .canGoDir = LeftDir});
                    map[mapPos].flags |= HasPipe;
                }
            }

            if((pos.pos - width) >= start && (pos.canGoDir & UpDir))
            {
                const char *up = (pos.pos - width - 0);
                int mapPos = sCalculatePosition(up, start);
                if (*up == 'F')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = RightDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*up == '7')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = LeftDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*up == '|')
                {
                    other->push_back(Dir{.pos = up, .canGoDir = UpDir});
                    map[mapPos].flags |= HasPipe;
                }
            }

            if((pos.pos + width) < data && (pos.canGoDir & DownDir))
            {
                const char *down = (pos.pos + width + 0);
                int mapPos = sCalculatePosition(down, start);
                if (*down == 'L')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = RightDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*down == 'J')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = LeftDir});
                    map[mapPos].flags |= HasPipe;
                }
                else if (*down == '|')
                {
                    other->push_back(Dir{.pos = down, .canGoDir = DownDir});
                    map[mapPos].flags |= HasPipe;
                }
            }
        }

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
#endif
    std::vector<char> map3x3;
    map3x3.resize(map.size() * 9);
    // Zoom map by 3x3.
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
    std::vector<int> tiles;
    std::vector<int> newTiles;

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
        /* print map
        for(int tmp = 0; tmp < (int)map3x3.size(); ++tmp)
        {
            printf("%c", map3x3[tmp]);
            if((tmp % (width * 3)) >= width * 3 - 1)
            {
                printf("\n");
            }
        }
        printf("\n\n");
        */
    }

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
    return emptySpots;
}

#ifndef RUNNER
int main()
{
    printf("10A: Value: %" PRIi64 "\n", sParse10A(data10A));
    printf("10B: Value: %" PRIi64 "\n", sParse10B(data10A));
    return 0;
}
#endif

int run10A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse10A(data10A);

    if(printOut)
        charsAdded = sprintf(buffer, "10A: Value: %" PRIi64, aResult);
    return charsAdded;
}

int run10B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse10B(data10A);

    if(printOut)
        charsAdded = sprintf(buffer, "10B: Value: %" PRIi64, resultB);

    return charsAdded;
}

