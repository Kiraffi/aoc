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

alignas(32) static constexpr char test23A[] =
    R"(#.#####################
#.......#########...###
#######.#########.#.###
###.....#.>.>.###.#.###
###v#####.#v#.###.#.###
###.>...#.#.#.....#...#
###v###.#.#.#########.#
###...#.#.#.......#...#
#####.#.#.#######.#.###
#.....#.#.#.......#...#
#.#####.#.#.#########v#
#.#...#...#...###...>.#
#.#.#v#######v###.###v#
#...#.>.#...>.>.#.###.#
#####v#.#.###v#.#.###.#
#.....#...#...#.#.#...#
#.#########.###.#.#.###
#...###...#...#...#.###
###.###.#.###v#####v###
#...#...#.#.>.>.#.>.###
#.###.###.#.###.#.#v###
#.....###...###...#...#
#####################.#
)";


template <typename T>
static T sMax(T a, T b)
{
    return a < b ? b : a;
}
/*
template <typename T>
static T sMin(T a, T b)
{
    return a < b ? a : b;
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

static constexpr int XSize = 160;
static constexpr int YSize = 160;
static char sGetTile(const char* data, int x, int y, int width)
{
    assert(x >= 0 && x < XSize && y >= 0 && y < YSize);
    return data[x + y * (width + 1)];
}

static int64_t sGetLongestPath(const char* data, int x, int y, int width, int height, uint8_t* visited, int64_t steps)
{
    if(x < 0 || y < 0 || x >= width || y >= width)
        return 0;
    if(visited[x + y * width])
        return 0;
    visited[x + y * width] = 1;
    if(x == width - 2 && y == height - 1)
    {
        visited[x + y * width] = 0;
        return steps;
    }
    char tile = sGetTile(data, x, y, width);
    int64_t result = 0;
    switch(tile)
    {
        case '#': break;
        case '>': result = sGetLongestPath(data, x + 1, y + 0, width, height, visited, steps + 1); break;
        case '<': result = sGetLongestPath(data, x - 1, y + 0, width, height, visited, steps + 1); break;
        case '^': result = sGetLongestPath(data, x + 0, y - 1, width, height, visited, steps + 1); break;
        case 'v': result = sGetLongestPath(data, x + 0, y + 1, width, height, visited, steps + 1); break;
        default:
            result = sMax(result, sGetLongestPath(data, x + 1, y + 0, width, height, visited, steps + 1));
            result = sMax(result, sGetLongestPath(data, x - 1, y + 0, width, height, visited, steps + 1));
            result = sMax(result, sGetLongestPath(data, x + 0, y + 1, width, height, visited, steps + 1));
            result = sMax(result, sGetLongestPath(data, x + 0, y - 1, width, height, visited, steps + 1));
            break;
    }
    // using bread crumbs, set it back to state it was before calling the function.
    visited[x + y * width] = 0;
    return result;
}


static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("23A Total");
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);

    uint8_t visited[XSize * YSize] = {};
    int64_t steps = sGetLongestPath(data, 1, 0, width, height, visited, 0);
    return steps;
}

enum Dir : uint8_t
{
    None = 0,
    Left = 1,
    Right = 2,
    Up = 8,
    Down = 16
};

struct CrossRoad
{
    uint16_t distances[4];
    uint8_t indices[4];
};

static void sCreateJumpMap(
    const char* data,
    int x,
    int y,
    int width,
    int height,
    Dir dir,
    CrossRoad* crossRoads,
    std::unordered_map<uint16_t, uint8_t>& crossRoadPositions)
{
    int distance = 1;
    int lastX = x;
    int lastY = y;
    int startX = x;
    int startY = y;
    switch(dir)
    {
        case Left: x--; dir = Right; break;
        case Right: x++; dir = Left; break;
        case Up: y--; dir = Down; break;
        case Down: y++; dir = Up; break;
        case None:
            assert(false);
    }
    while(true)
    {
        int validDirs = 0;
        int dirs = 0;
        char rTile = sGetTile(data, x + 1, y + 0, width);
        char lTile = sGetTile(data, x - 1, y + 0, width);
        char uTile = sGetTile(data, x + 0, y - 1, width);
        char dTile = sGetTile(data, x + 0, y + 1, width);
        if (rTile != '#')
        {
            if (!(lastX == x + 1 && lastY == y + 0))
                validDirs |= Right;
            dirs++;
        }
        if (lTile != '#')
        {
            if (!(lastX == x - 1 && lastY == y + 0))
                validDirs |= Left;
            dirs++;
        }
        if (dTile != '#')
        {
            if (!(lastX == x + 0 && lastY == y + 1))
                validDirs |= Down;
            dirs++;
        }
        if (uTile != '#')
        {
            if (!(lastX == x + 0 && lastY == y - 1))
                validDirs |= Up;
            dirs++;
        }
        assert(dirs > 1 || (x == width - 2 && y == height - 1));
        if(dirs > 2 || (x == width - 2 && y == height - 1))
        {
            int prevDirIndex = 0;
            if(lastX + 1 == x && lastY == y) { prevDirIndex = 0; }
            else if(lastX - 1 == x && lastY == y) { prevDirIndex = 1; }
            else if(lastY - 1 == y && lastX == x) { prevDirIndex = 2; }
            else if(lastY + 1 == y && lastX == x) { prevDirIndex = 3; }
            else assert(false);

            uint16_t pos = x + y * width;
            bool added = false;
            if(crossRoadPositions.find(pos) == crossRoadPositions.end())
            {
                size_t amount = crossRoadPositions.size();
                crossRoadPositions[pos] = amount;
                added = true;
            }
            uint16_t oldPos = startX + startY * width;
            assert(crossRoadPositions.find(oldPos) != crossRoadPositions.end());
            int oldIndex = crossRoadPositions[oldPos];
            int oldCrossRoadDirIndex = 0;
            switch(dir)
            {
                case Right: oldCrossRoadDirIndex = 0; break;
                case Left: oldCrossRoadDirIndex = 1; break;
                case Up: oldCrossRoadDirIndex = 2; break;
                case Down: oldCrossRoadDirIndex = 3; break;
                default:
                    assert(false);
                    break;
            }
            int index = crossRoadPositions[pos];
            assert(index);
            CrossRoad& oldCrossRoad = crossRoads[oldIndex];
            oldCrossRoad.indices[oldCrossRoadDirIndex] = index;
            oldCrossRoad.distances[oldCrossRoadDirIndex] = distance;

            crossRoads[index].indices[prevDirIndex] = oldIndex;
            crossRoads[index].distances[prevDirIndex] = distance;

            if(added)
            {
                assert(distance < 65536);
                if(validDirs & Right)
                {
                    sCreateJumpMap(data, x, y, width, height, Right, crossRoads, crossRoadPositions);
                }
                if(validDirs & Left)
                {
                    sCreateJumpMap(data, x, y, width, height, Left, crossRoads, crossRoadPositions);
                }
                if(validDirs & Up)
                {
                    sCreateJumpMap(data, x, y, width, height, Up, crossRoads, crossRoadPositions);
                }
                if(validDirs & Down)
                {
                    sCreateJumpMap(data, x, y, width, height, Down, crossRoads, crossRoadPositions);
                }
            }
            return;
        }
        else if (dirs == 2)
        {
            lastX = x;
            lastY = y;
            distance++;

            switch(validDirs)
            {
                case Right: x++; break;
                case Left: x--; break;
                case Up: y--; break;
                case Down: y++; break;
                default: assert(false); break;
            }
        }
    }
}


int64_t sFindLongest(int currIndex,
    int endIndex,
    int64_t startDistance,
    const CrossRoad* crossRoads,
    uint64_t visited,
    uint64_t secondLastCheck)
{
    if(currIndex == endIndex)
        return startDistance;
    if((visited >> currIndex) & 1)
        return 0;
    if((visited & secondLastCheck) == secondLastCheck)
        return 0;

    uint64_t newVisited = visited | (uint64_t(1) << uint64_t(currIndex));

    int64_t result = 0;

    const CrossRoad& crossRoad = crossRoads[currIndex];
    for(int i = 0; i < 4; ++i)
    {
        if((newVisited >> crossRoad.indices[i]) & 1)
            continue;
        if(crossRoad.distances[i])
        {
            result = sMax(result, sFindLongest(
                crossRoad.indices[i],
                endIndex,
                startDistance + crossRoad.distances[i],
                crossRoads,
                newVisited,
                secondLastCheck));
        }
    }
    return result;
}



static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("23B Total");
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);
    assert(width < 256 && height < 256);

    CrossRoad crossRoads[64] = {};
    std::unordered_map<uint16_t, uint8_t> crossRoadPositions;
    // width - 2, height - 1
    crossRoadPositions[width * height - 2] = 1;
    // 1, 0
    crossRoadPositions[1] = 0;
    {
        TIMEDSCOPE("23B create jump map");
        sCreateJumpMap(data, 1, 0, width, height, Down, crossRoads, crossRoadPositions);
    }
    assert(crossRoads.size() == crossRoadPositions.size());
    assert(crossRoads.size() < 64);

    int64_t result = 0;

    {
        uint64_t secondLastCheck = 0;
        for(int i = 0; i < 4; i++)
        {
            const CrossRoad& crossRoad = crossRoads[crossRoads[1].indices[3]];
            if(crossRoad.distances[i] && crossRoad.indices[i] != 1)
            {
                secondLastCheck |= uint64_t(1) << uint64_t(crossRoad.indices[i]);
            }
        }

        TIMEDSCOPE("23B find longest");
        // Find route to the cross road before the goal element
        result = sFindLongest(0, crossRoads[1].indices[3], 0, crossRoads, 0, secondLastCheck);
    }
    return result + crossRoads[1].distances[3];
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "23A: Result: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "23B: Result: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data23A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data23A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run23A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data23A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run23B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data23A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

