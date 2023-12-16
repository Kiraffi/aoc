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

alignas(16) static constexpr char test12A[] =
    R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1
)";

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

union alignas(4) Day12State
{
    struct Values
    {
        uint8_t index;
        uint8_t continuous;
        uint8_t number;
        char choice;

    } values;
    uint32_t hash;
};

static_assert(sizeof(Day12State) == 4);
static_assert(sizeof(Day12State::hash) == sizeof(Day12State::Values));


static int64_t visit(
    Day12State stateStart,
    char* line,
    uint8_t lineLen,
    uint8_t* numbers,
    int numberCount,
    std::unordered_map<uint32_t, int64_t>& visited
)
{
    auto iter = visited.find(stateStart.hash);
    if(iter != visited.end())
        return iter->second;
    Day12State state = stateStart;

    int64_t visitNumbers = 0;
    visited[stateStart.hash] = visitNumbers;

    for(uint8_t i = state.values.index; i < lineLen; ++i)
    {
        char c = line[i];
        if(c == '?')
        {
            Day12State newState = state;
            newState.values.choice = '.';
            newState.values.index = i;
            line[i] = '.';
            visitNumbers += visit(newState, line, lineLen, numbers, numberCount, visited);
            line[i] = '?';

            c = '#';
        }

        if(c == '#')
        {
            ++state.values.continuous;
            if(state.values.continuous > numbers[state.values.number])
            {
                break;
            }
        }
        else if(state.values.continuous)
        {
            if(numbers[state.values.number] != state.values.continuous)
            {
                break;
            }
            ++state.values.number;
            state.values.continuous = 0;
            if(state.values.number == numberCount)
            {
                while(i < lineLen && line[i] != '#')
                {
                    ++i;
                }
                if(i == lineLen)
                {
                    visitNumbers++;
                }
                break;
            }
        }
    }
    visited[stateStart.hash] = visitNumbers;
    return visitNumbers;
}


static int64_t sGetArrangementsCount(const char* data, int multiplier)
{
    int64_t combos = 0;

    uint8_t numbers[32] = {};

    std::unordered_map<uint32_t, int64_t> visited;
    while(*data)
    {
        int numberCount = 0;

        int unknowns = 0;

        int position = 0;

        char line[256] = {};
        uint8_t lineLen = 0;

        visited.clear();
        const char* start = data;
        for(int i = 0; i < multiplier; ++i)
        {
            data = start;
            while (*data != '\n')
            {
                char c = *data;
                if (isdigit(c))
                {
                    numbers[numberCount++] = sParserNumber(0, &data);
                    if (*data == '\n')
                        break;
                }
                else if (c == '?')
                {
                    ++unknowns;
                    line[position] = c;
                }
                else if (c == ' ')
                {
                    lineLen = position + 1;
                }
                else if (c == '#' || c == '.')
                {
                    line[position] = c;
                }
                data++;
                position++;
            }
            position = lineLen - 1;
            line[position++] = '?';

        }
        Day12State state = { .values{ .index = 0, .continuous = 0, .number = 0, .choice = '#'}};
        combos += visit(state, line, lineLen, numbers, numberCount, visited);
        ++data;
    }
    return combos;

}

static int64_t sParse12A(const char* data)
{
    TIMEDSCOPE("12A Total");
    return sGetArrangementsCount(data, 1);
}



static int64_t sParse12B(const char* data)
{
    TIMEDSCOPE("12B Total");
    return sGetArrangementsCount(data, 5);
}


#ifndef RUNNER
int main()
{
    printf("12A: Counts: %" PRIi64 "\n", sParse12A(data12A));
    printf("12B: Counts: %" PRIi64 "\n", sParse12B(data12A));
    return 0;
}
#endif

int run12A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse12A(data12A);

    if(printOut)
        charsAdded = sprintf(buffer, "12A: Counts: %" PRIi64, aResult);
    return charsAdded;
}

int run12B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse12B(data12A);

    if(printOut)
        charsAdded = sprintf(buffer, "12B: Counts: %" PRIi64, resultB);

    return charsAdded;
}

