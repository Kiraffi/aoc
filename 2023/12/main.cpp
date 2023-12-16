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
        uint8_t questionMarkCount;
        uint8_t continuous;
        uint8_t number;
        uint8_t padding;
    } values;
    uint32_t hash;
};

static_assert(sizeof(Day12State) == 4);
static_assert(sizeof(Day12State::hash) == sizeof(Day12State::Values));

static int64_t visit(
    Day12State state,
    char* line,
    uint8_t lineLen,
    uint8_t linePos,
    uint8_t* numbers,
    int numberCount,
    std::unordered_map<uint32_t, int64_t>& visited
)
{
    uint32_t hash = state.hash;
    auto iter = visited.find(hash);
    if(iter != visited.end())
        return iter->second;

    int64_t visitNumbers = 0;

    for(uint8_t i = linePos; i < lineLen; ++i)
    {
        char c = line[i];
        if(c == '?')
        {
            ++state.values.questionMarkCount;
            Day12State newState = state;
            line[i] = '.';
            visitNumbers += visit(newState, line, lineLen, i, numbers, numberCount, visited);
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
    visited[hash] = visitNumbers;
    return visitNumbers;
}

static int64_t sGetArrangementsCount(const char* data, int multiplier)
{
    int64_t combos = 0;

    uint8_t numbers[32] = {};
    char line[256] = {};

    std::unordered_map<uint32_t, int64_t> visited;
    while(*data)
    {
        uint8_t numberCount = 0;

        uint8_t position = 0;

        visited.clear();
        {
            //TIMEDSCOPE("Parsing");

            while (*data != ' ')
            {
                line[position++] = *data++;
            }
            data++;
            while(true)
            {
                numbers[numberCount++] = sParserNumber(0, &data);
                if (*data == '\n')
                    break;
                data++;
            }

            line[position++] = '?';
            for (int i = 1; i < multiplier; ++i)
            {
                memcpy(line + position * i, line, position);
                memcpy(numbers + numberCount * i, numbers, numberCount);
            }
            position *= multiplier;
            numberCount *= multiplier;

            line[position - 1] = '\0';

        }
        {
            //TIMEDSCOPE("Counting");
            Day12State state = {};
            combos += visit(state, line, position, 0, numbers, numberCount, visited);
        }
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
    int64_t value = sGetArrangementsCount(data, 5);
    return value;
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

