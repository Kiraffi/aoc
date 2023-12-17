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

static uint32_t sGlobalLowest = ~0u;

alignas(32) static constexpr char test17A[] =
    R"(2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533
)";

alignas(32) static constexpr char test17B[] =
    R"(111111111111
999999999991
999999999991
999999999991
999999999991
)";
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
    while(**data >= '0')
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
enum Dir : uint8_t
{
    None = 0,
    Right = 1,
    Left = 2,
    Up = 4,
    Down = 8,
};

union Day17State
{
    struct Values
    {
        uint32_t energy;
        uint8_t x;
        uint8_t y;
        Dir dir;
        uint8_t howManyInRow;
    } values;
    struct Hash
    {
        uint32_t energy;
        uint32_t hash;
    } hash;
};
static_assert(sizeof(Day17State) == 8);
static_assert(sizeof(Day17State::Values) == 8);
static_assert(sizeof(Day17State::Hash) == 8);

static bool sCanMove(const Day17State& state,
    const char* mapData,
    Dir dir,
    std::unordered_map<uint32_t, Day17State>& visited,
    int width,
    int height,
    int minimumDir,
    Day17State& newState)
{
    int xOffset = 0;
    int yOffset = 0;
    switch(dir)
    {
        case None: return false;
        case Up: yOffset = -1; break;
        case Down: yOffset = 1; break;
        case Left: xOffset = -1; break;
        case Right: xOffset = 1; break;
    }

    int x = state.values.x + xOffset;
    int y = state.values.y + yOffset;

    if(x < 0 || x >= width || y < 0 || y >= height)
    {
        return false;
    }
    uint32_t lowest = ~0u;

    uint32_t energy = (mapData[x + y * (width + 1)] - '0') + state.values.energy;
    if(energy >= sGlobalLowest)
        return false;
    newState.values.x = x;
    newState.values.y = y;
    newState.values.dir = dir;
    newState.values.howManyInRow = uint8_t(state.values.dir == dir ? state.values.howManyInRow + 1u : 1u);
    if(x == width - 1 && y == height - 1 && newState.values.howManyInRow >= minimumDir && newState.values.howManyInRow)
    {
        sGlobalLowest = energy;
        return false;
    }

    auto found = visited.find(newState.hash.hash);
    if(found != visited.end())
    {
        lowest = found->second.values.energy;
    }

    if(energy < lowest)
    {
        newState.values.energy = energy;
        visited[newState.hash.hash] = newState;
        return true;
    }
    return false;
}

static int64_t sGetMinimumEnergy(const char* data, int minimumSameDir, int maximumSameDir)
{
    sGlobalLowest = ~0u;

    int width = 0;
    int height = 0;
    sGetSize(data, width, height);

    std::vector<Day17State> states1;
    std::vector<Day17State> states2;

    std::unordered_map<uint32_t, Day17State> visited;
    //uint32_t mapLowest[256 * 256] = {};
    //mapLowest[0] = 1;
    states1.push_back(Day17State{.values{.energy = 1, .x = 0, .y = 0, .dir = Right, .howManyInRow = 0}});
    states1.push_back(Day17State{.values{.energy = 1, .x = 0, .y = 0, .dir = Down, .howManyInRow = 0}});

    std::vector<Day17State>* states = &states1;
    std::vector<Day17State>* newStates = &states2;

    while(!states->empty())
    {
        newStates->clear();
        Day17State newState = {};

        for (const Day17State& state : *states)
        {
            uint8_t dir = 0xf;
            if (state.values.howManyInRow == maximumSameDir)
                dir &= ~state.values.dir;

            if (state.values.howManyInRow < minimumSameDir)
                dir &= state.values.dir;

            switch (state.values.dir)
            {
                case None:
                    break;
                case Up:
                    dir &= ~Down;
                    break;
                case Down:
                    dir &= ~Up;
                    break;
                case Left:
                    dir &= ~Right;
                    break;
                case Right:
                    dir &= ~Left;
                    break;
            }

            if (sCanMove(state, data, Dir(dir & Left), visited, width, height, minimumSameDir, newState))
            {
                newStates->push_back(newState);
            }
            if (sCanMove(state, data, Dir(dir & Right), visited, width, height, minimumSameDir, newState))
            {
                newStates->push_back(newState);
            }
            if (sCanMove(state, data, Dir(dir & Up), visited, width, height, minimumSameDir, newState))
            {
                newStates->push_back(newState);
            }
            if (sCanMove(state, data, Dir(dir & Down), visited, width, height, minimumSameDir, newState))
            {
                newStates->push_back(newState);
            }

        }
        std::swap(states, newStates);
    }
    return sGlobalLowest - 1;
}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("17A Total");
    return sGetMinimumEnergy(data, 1, 3);
}
static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("17B Total");
    return sGetMinimumEnergy(data, 4, 10);
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "17A: Minimum energy: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "17B: Minimum energy: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data17A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data17A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run17A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data17A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run17B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data17A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

