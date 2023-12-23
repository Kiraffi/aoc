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
static constexpr int XSize = 160;
static constexpr int YSize = 160;
static char sGetTile(const char* data, int x, int y, int width)
{
    assert(x >= 0 && x < XSize && y >= 0 && y < YSize);
    return data[x + y * (width + 1)];
}
/*
uint8_t visitedVisited[XSize * YSize * 128] = {};
uint8_t visitedVisitedCount = 0;
*/
static int64_t sGetLongestPath(const char* data, int x, int y, int width, int height, uint8_t* visited, int64_t steps)
{
    if(x < 0 || y < 0 || x >= width || y >= width)
        return 0;
    if(visited[x + y * width])
        return 0;
    visited[x + y * width] = 1;
    if(x == width - 2 && y == height - 1)
    {
        //memcpy(visitedVisited + visitedVisitedCount * XSize * YSize, visited, XSize * YSize);
        //visitedVisitedCount++;
        //printf("steps: %" PRIi64 "\n", steps);
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
/*
    for(int i = 0; i < visitedVisitedCount; ++i)
    {
        for(int y = 0; y < width; ++y)
        {
            for (int x = 0; x < height; ++x)
            {
                if(visitedVisited[(x + y * width) + i * XSize * YSize])
                {
                    printf("O");
                }
                else
                {
                    printf("%c", data[x + y * (width + 1)]);
                }
            }
            printf("\n");
        }
        printf("\n\n");
    }
*/

    return steps;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("23B Total");
    return *data;
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

