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

#define PROFILE 1
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


static int64_t sParse12A(const char* data)
{
    TIMEDSCOPE("12A Total");
    int64_t combos = 0;
    int lineNumber = 1;
    struct State
    {
        uint8_t index;
        uint8_t continuous;
        uint8_t number;
        char choice;
    };
    std::vector<State> states;

    while(*data)
    {
        //TIMEDSCOPE("12A Loop");

    //printf("\n\n------\nLine: %i\n", lineNumber);
        //const char* lineStart = data;
        //const char* end = data;
        uint8_t numbers[32] = {};
        int numberCount = 0;

        //uint8_t unknownPositions[128] = {};
        int unknowns = 0;

        int64_t found = 0;
        int position = 0;

        char line[256] = {};
        uint8_t lineLen = 0;

        states.clear();

        const char* start = data;
        for(int i = 0; i < 1; ++i)
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
                    //unknownPositions[unknowns++] = position;
                    line[position] = c;
                }
                else if (c == ' ')
                {
                    lineLen = position + 1;
                    //end = data + 1;
                }
                else if (c == '#' || c == '.')
                {
                    line[position] = c;
                }
                data++;
                position++;
            }
            position = lineLen - 1;
            //unknownPositions[unknowns++] = position;
            line[position++] = '?';

        }
        //printf("unknowns: %i, line len: %i\n", unknowns, lineLen);

        if(line[0] == '?')
        {
            states.push_back({.index = 0, .continuous = 0,.number = 0,.choice = '#'});
            states.push_back({.index = 0, .continuous = 0,.number = 0,.choice = '.'});
        }
        else
        {
            states.push_back({.index = 0, .continuous = 0,.number = 0,.choice = '.'});
        }

        while(!states.empty())
        {

            const State& state = states.back();
            states.pop_back();
            //for(const State& state : *currentStates)
            {
                uint8_t continuous = state.continuous;
                uint8_t number = state.number;
                for(uint8_t i = state.index; i < lineLen; ++i)
                {
                    char c = line[i];
                    if(c == '?')
                    {
                        if(i == state.index)
                        {
                            c = state.choice;
                        }
                        else
                        {
                            states.push_back(State{.index = i,  .continuous = continuous, .number = number, .choice = '.' });
                            c = '#';
                        }
                    }

                    if(c == '#')
                    {
                        ++continuous;
                        if(continuous > numbers[number])
                        {
                            break;
                        }
                    }
                    else
                    {
                        if(continuous)
                        {
                            if(numbers[number] != continuous)
                            {
                                break;
                            }
                            ++number;
                            if(number == numberCount)
                            {
                                while(i < lineLen)
                                {
                                    if(line[i] == '#')
                                        break;
                                    ++i;
                                }
                                if(i != lineLen)
                                    break;
                                found++;
                                combos++;
                                break;
                            }
                        }
                        continuous = 0;
                    }

                }

            }

            //std::swap(currentStates, otherStates);
        }
#if 0

        for(int i = 0; i < (1 << unknowns); ++i)
        {
            //const char* ptr = lineStart;
            for(int j = 0; j < unknowns; ++j)
            {
                if(i & (1 << j))
                {
                    line[unknownPositions[j]] = '#';
                }
                else
                {
                    line[unknownPositions[j]] = '.';
                }
            }
            /*
            int foundUnknown = 0;
            int j = 0;
            while(ptr < end)
            {
                char c = *ptr;
                if(c == '?')
                {

                    line[j] = (i >> foundUnknown) & 1 ? '#' : '.';
                    ++foundUnknown;
                }
                else
                {
                    line[j] = c;
                }
                j++;
                ptr++;
            }
             */

            /*
            int number = 0;
            int continuous = 0;
            bool isRight = true;
            for(int k = 0; k < lineLen; k++)
            {
                if(line[k] == '#')
                {
                    ++continuous;
                    if(continuous > numbers[number])
                    {
                        isRight = false;
                        break;
                    }
                }
                else
                {
                    if(continuous)
                    {
                        if(numbers[number] != continuous)
                        {
                            isRight = false;
                            break;
                        }
                        ++number;
                    }
                    continuous = 0;
                }
            }
            if(isRight && number == numberCount)
            {
                combos++;
                found++;
            }
             */
        }
#endif
        //printf("Rec Line: %i, found: %" PRIu64 "\n", lineNumber, found);
        //assert(found);

        ++data;
        ++lineNumber;
    }
    return combos;
}



union alignas(4) NewState
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

static_assert(sizeof(NewState) == 4);
static_assert(sizeof(NewState::hash) == sizeof(NewState::Values));


static int64_t visit(
    NewState stateStart,
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
    NewState state = stateStart;

    int64_t visitNumbers = 0;
    visited[stateStart.hash] = visitNumbers;

    for(uint8_t i = state.values.index; i < lineLen; ++i)
    {
        char c = line[i];
        if(c == '?')
        {
            NewState newState = state;
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
        if(state.values.continuous && (c == '.'))// || i + 1 == lineLen))
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
    //visited.insert({state.hash, visitNumbers});
    //visited.insert({state.hash, visitNumbers});
    visited[stateStart.hash] = visitNumbers;
    return visitNumbers;
}

static int64_t sParse12B(const char* data)
{
    TIMEDSCOPE("12B Total");
    int64_t combos = 0;
    int lineNumber = 1;

    std::vector<NewState> states;
    // 8 bits current line character index
    // 8 bits number index

    std::unordered_map<uint32_t, int64_t> visited;
    while(*data)
    {
        //TIMEDSCOPE("12A Loop");

        //printf("\n\n------\nLine: %i\n", lineNumber);
        //const char* lineStart = data;
        //const char* end = data;
        uint8_t numbers[32] = {};
        int numberCount = 0;

        //uint8_t unknownPositions[128] = {};
        int unknowns = 0;

        //int64_t found = 0;
        int position = 0;

        char line[256] = {};
        uint8_t lineLen = 0;

        states.clear();
        visited.clear();
        const char* start = data;
        for(int i = 0; i < 5; ++i)
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
                    //unknownPositions[unknowns++] = position;
                    line[position] = c;
                }
                else if (c == ' ')
                {
                    lineLen = position + 1;
                    //end = data + 1;
                }
                else if (c == '#' || c == '.')
                {
                    line[position] = c;
                }
                data++;
                position++;
            }
            position = lineLen - 1;
            //unknownPositions[unknowns++] = position;
            line[position++] = '?';

        }
        //printf("unknowns: %i, line len: %i\n", unknowns, lineLen);
        int64_t found = 0;
        //NewState a = { .values{ .index = 0, .continuous = 0, .number = 0, .choice = '.'}};
        NewState b = { .values{ .index = 0, .continuous = 0, .number = 0, .choice = '#'}};

        found += visit(b, line, lineLen, numbers, numberCount, visited);
        printf("Found: %" PRIi64 "\n", found);
        combos += found;
#if 0
        while(!states.empty())
        {

            const NewState& state = states.back();
            states.pop_back();
            //for(const State& state : *currentStates)
            {
                uint8_t continuous = state.values.continuous;
                uint8_t number = state.values.number;
                for(uint8_t i = state.values.index; i < lineLen; ++i)
                {
                    char c = line[i];
                    if(c == '?')
                    {
                        if(i == state.values.index)
                        {
                            c = state.values.choice;
                        }
                        else
                        {
                            states.push_back(NewState{.values {.index = i,  .continuous = continuous, .number = number, .choice = '.' }});
                            c = '#';
                        }
                    }

                    if(c == '#')
                    {
                        ++continuous;
                        if(continuous > numbers[number])
                        {
                            break;
                        }
                    }
                    else
                    {
                        uint16_t index = i | (number << 8);
                        if(visited.contains(index))
                        {
                            break;
                        }
                        visited.insert({index, 0});
                        if(continuous)
                        {
                            if(numbers[number] != continuous)
                            {
                                break;
                            }
                            ++number;
                            if(number == numberCount)
                            {
                                while(i < lineLen)
                                {
                                    if(line[i] == '#')
                                        break;
                                    ++i;
                                }
                                if(i != lineLen)
                                    break;
                                found++;
                                combos++;
                                break;
                            }
                        }
                        continuous = 0;
                    }

                }

            }

            //std::swap(currentStates, otherStates);
        }
        //printf("Rec Line: %i, found: %" PRIu64 "\n", lineNumber, found);
        //assert(found);
#endif

        ++data;
        ++lineNumber;
    }
    return combos;
}



// 10 5 bit numbers,
// left and right
struct HashOne
{
    static constexpr uint64_t NumbersSingleBitsAmount = 5;
    static constexpr uint64_t NumbersSingleCount = 10;

    static constexpr uint64_t NumbersBitAmount = NumbersSingleCount * NumbersSingleBitsAmount;
    static constexpr uint64_t LeftContinuousBitAmount = 5;
    static constexpr uint64_t RightContinuousBitAmount = 5;
    static constexpr uint64_t NumberCountBitAmount = 4;

    static constexpr uint64_t NumbersStartBit = 0;
    static constexpr uint64_t LeftContinuousStartBit = NumbersStartBit + NumbersBitAmount;
    static constexpr uint64_t RightContinuousStartBit = LeftContinuousStartBit + LeftContinuousBitAmount;
    static constexpr uint64_t NumbersCountStartBit = RightContinuousStartBit + RightContinuousBitAmount;

    static constexpr uint64_t NumbersMask = ((uint64_t(1) << uint64_t(NumbersBitAmount)) - 1) << NumbersStartBit;
    static constexpr uint64_t LeftContinuousMask = ((uint64_t(1) << uint64_t(LeftContinuousBitAmount)) - 1) << LeftContinuousStartBit;
    static constexpr uint64_t RightContinuousMask = ((uint64_t(1) << uint64_t(RightContinuousBitAmount)) - 1) << RightContinuousStartBit;
    static constexpr uint64_t NumbersCountMask = ((uint64_t(1) << uint64_t(NumberCountBitAmount)) - 1) << NumbersCountStartBit;
    static constexpr uint64_t NumbersSingleMask = ((uint64_t(1) << uint64_t(NumbersSingleBitsAmount)) - 1);

    /*
    uint64_t numbers : 50;
    uint64_t numberCount : 4;
    uint64_t leftContinuous : 5;
    uint64_t rightContinuous : 5;
     */
    uint64_t value;
};
static_assert(HashOne::NumbersSingleCount <= (1 << HashOne::NumberCountBitAmount));
static_assert(HashOne::NumbersStartBit + HashOne::NumbersBitAmount <= 64);
static const size_t HashSize = sizeof(HashOne);
static_assert(HashSize == 8);


static constexpr int MaxNumberCount = 13;

struct State
{
    uint8_t numbers[MaxNumberCount];
    uint8_t startBits;
    uint8_t endBits;
    uint8_t numberCount;
};


static uint64_t sGetState(const char* line, int lineLen, uint64_t& hashIndex)
{
    bool beginningFound = false;
    uint64_t beginningContinuous = 0;
    uint64_t continuous = 0;
    uint64_t numbers = 0;
    uint64_t numbersBits = 0;
    uint64_t hash = 0;
    uint64_t firstNumber = 0;
    for(int i = 0; i < lineLen; ++i)
    {
        char c = line[i];
        if(c == '#')
        {
            continuous++;
            assert(continuous < 32);
        }
        else if(continuous)
        {
            if(!beginningFound)
            {
                beginningContinuous = continuous;
                beginningFound = true;
            }
            else
            {
                firstNumber = firstNumber == 0 ? continuous : firstNumber;
                numbersBits |= continuous << (numbers * HashOne::NumbersSingleBitsAmount);
                numbers++;
            }
            continuous = 0;
        }
    }
    assert(numbers < HashOne::NumbersSingleCount);
    if(!beginningFound)
    {
        beginningContinuous = continuous;
    }

    hashIndex = beginningContinuous != 0
        ? beginningContinuous
        : numbers != 0 ? firstNumber : continuous;

    hash |= numbersBits << HashOne::NumbersStartBit;
    hash |= beginningContinuous << HashOne::LeftContinuousStartBit;
    hash |= continuous << HashOne::RightContinuousStartBit;
    hash |= numbers << HashOne::NumbersCountStartBit;
    return hash;
}


static int64_t  sGetValidArrangements(
    const std::unordered_map<uint64_t, uint64_t> *hashCounts,
    const uint8_t* numbers,
    uint64_t numberCount,
    uint64_t numberIndex,
    uint64_t continuous,
    int recursionLevel)
{
    int64_t result = 0;
    if(numberIndex >= numberCount)
        return 0;
    uint64_t nextNumber = numbers[numberIndex];
    if(nextNumber < continuous)
        return 0;

    uint64_t searchNumberIndex = nextNumber - continuous;
    for(auto iter : hashCounts[searchNumberIndex])
    {
        uint64_t nextNumberIndex = numberIndex;
        uint64_t hashContinuous = (iter.first & HashOne::LeftContinuousMask) >> HashOne::LeftContinuousStartBit;
        hashContinuous = continuous + hashContinuous;
        bool canContinue = false;

        if(hashContinuous == nextNumber)
        {
            nextNumberIndex = numberIndex + 1;
            canContinue = true;
        }
        else if(continuous == 0 && hashContinuous == 0)
        {
            canContinue = true;
        }
        if(canContinue)
        {
            uint64_t hashNumberCount = (iter.first & HashOne::NumbersCountMask)  >> HashOne::NumbersCountStartBit;
            uint64_t hashNumbers64 = (iter.first & HashOne::NumbersMask) >> HashOne::NumbersStartBit;
            uint8_t hashRightContinuous = (iter.first & HashOne::RightContinuousMask) >> HashOne::RightContinuousStartBit;

            bool legitNumbers = true;
            for(uint64_t i = 0; i < hashNumberCount; ++i)
            {
                if(nextNumberIndex >= numberCount)
                {
                    legitNumbers = false;
                    break;
                }
                uint64_t nextNumberFromHash = hashNumbers64 & HashOne::NumbersSingleMask;
                if(numbers[nextNumberIndex++] != nextNumberFromHash)
                {
                    legitNumbers = false;
                    break;
                }
                hashNumbers64 >>= HashOne::NumbersSingleBitsAmount;
            }
            if(legitNumbers)
            {
                if(recursionLevel == 4 &&
                    (
                        (hashRightContinuous == 0 && nextNumberIndex == numberCount)
                        || (nextNumberIndex == numberCount - 1 && hashRightContinuous == numbers[nextNumberIndex])))
                {
                    result += iter.second;
                }
                else if(recursionLevel < 4)
                {
                    result += iter.second * sGetValidArrangements(hashCounts, numbers, numberCount, nextNumberIndex,
                            hashRightContinuous, recursionLevel + 1);
                }
            }
        }
    }
    return result;
}


int64_t sParse12BA(const char* data)
{
    TIMEDSCOPE("12B Total");


    std::unordered_map<uint64_t, uint64_t> hashCounts[32];
    uint8_t unknownPositions[32] = {};
    uint8_t numbers[256] = {};

    int64_t combos = 0;
    int64_t found = 0;
    //int lineNumber = 0;
    while(*data)
    {
        //const char* lineStart = data;
        //const char* end = data;
        int numberCount = 0;

        int unknowns = 0;

        //int64_t found = 0;
        int position = 0;

        char line[24] = {};


        while(*data != '\n')
        {
            char c = *data;
            if(isdigit(c))
            {
                numbers[numberCount++] = sParserNumber(0, &data);
                if(*data == '\n')
                    break;
            }
            else if(c == '?')
            {
                line[position] = c;
                unknownPositions[unknowns++] = position;
                position++;
            }
            else if(c == ' ')
            {
                //end = data + 1;
            }
            else if(c == '#' || c == '.')
            {
                line[position] = c;
                position++;
            }
            data++;
        }

        {
            for(int j = 0; j < (1 << unknowns); ++j)
            {
                for(int k = 0; k < unknowns; ++k)
                {
                    char c = ((j >> k) & 1) ? '#' : '.';
                    line[unknownPositions[k]] = c;
                }
                uint64_t hashIndex = 0;
                uint64_t hash = sGetState(line, position, hashIndex);
                hashCounts[hashIndex][hash]++;

            }

            for(int i = 1; i < 5; ++i)
            {
                for(int j = 0; j < numberCount; ++j)
                {
                    numbers[i * numberCount + j] = numbers[j];
                }
            }
            numberCount *= 5;

            //printf("amount: %i\n", (int)hashCounts.size());
            //for(const auto& hashCount : hashCounts)
            //    assert(hashCount.size() < 16384);
            /*
            for(const auto iter : hashCounts)
            {
                printf("%" PRIu64 ": counts: %i\n", iter.first, iter.second);
            }
            */
            found = sGetValidArrangements(hashCounts, numbers, numberCount, 0, 0, 0);
            combos += found;
            printf("Found: %" PRIi64 " ones\n", found);


            for(auto& hashCount : hashCounts)
                hashCount.clear();
            //printf("\n\n");

        }
/*
        for(int i = 0; i < (1 << unknowns); ++i)
        {
            const char* ptr = lineStart;
            int foundUnknown = 0;
            int j = 0;
            while(ptr < end)
            {
                char c = *ptr;
                if(c == '?')
                {

                    line[j] = (i >> foundUnknown) & 1 ? '#' : '.';
                    ++foundUnknown;
                }
                else
                {
                    line[j] = c;
                }
                j++;
                ptr++;
            }
            int number = 0;
            int continuous = 0;
            bool isRight = true;
            for(int k = 0; k < j; k++)
            {
                if(line[k] == '#')
                {
                    ++continuous;
                }
                else
                {
                    if(continuous)
                    {
                        if(numbers[number] != continuous)
                        {
                            isRight = false;
                            break;
                        }
                        ++number;
                    }
                    continuous = 0;
                }
            }
            if(isRight && number == numberCount)
            {
                combos++;
                found++;
            }
        }
        */
        //printf("Other Line: %i, found: %" PRIu64 "\n", lineNumber++, found);

        //assert(found);

        ++data;
    }
    return combos;
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
