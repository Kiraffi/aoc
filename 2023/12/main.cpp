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
        assert(found);

        ++data;
        ++lineNumber;
    }
    return combos;
}



// 10 5 bit numbers,
// left and right
struct HashOne
{
    static constexpr int getLeftContinuousBitAmount() { return 5; }
    static constexpr int getRightContinuousBitAmount() { return 5; }
    static constexpr int getNumberCountBitAmount() { return 4; }
    static constexpr int getNumbersBitAmount() { return 50; }
    static constexpr int getNumbersSingleBitAmount() { return 5; }

    static constexpr int getNumbersStartBit() { return 0; }
    static constexpr int getLeftContinuousStartBit() { return getNumbersStartBit() + getNumbersBitAmount(); }
    static constexpr int getRightContinuousStartBit() { return getLeftContinuousStartBit() + getLeftContinuousBitAmount(); }
    static constexpr int getNumberCountStartBit() { return getRightContinuousStartBit() + getRightContinuousBitAmount(); }

    /*
    uint64_t numbers : 50;
    uint64_t numberCount : 4;
    uint64_t leftContinuous : 5;
    uint64_t rightContinuous : 5;
     */
    uint64_t value;
};

static_assert(HashOne::getNumbersStartBit() + HashOne::getNumbersBitAmount() <= 64);
static const size_t HashSize = sizeof(HashOne);
static_assert(HashSize == 8);

static uint64_t sGetState(const char* line, int lineLen)
{
    bool isBeginning = true;
    uint64_t beginningContinuous = 0;
    uint64_t continuous = 0;
    uint64_t numbers = 0;
    uint64_t numbersBits = 0;
    uint64_t hash = 0;
    for(int i = 0; i < lineLen; ++i)
    {
        char c = line[i];
        if(c == '#')
        {
            continuous++;
        }
        else if(continuous)
        {
            assert(continuous < 32);
            if(isBeginning)
            {
                beginningContinuous = continuous;
            }
            else
            {
                numbersBits |= continuous << (numbers * HashOne::getNumbersSingleBitAmount());
                numbers++;
            }
            isBeginning = false;
            continuous = 0;
        }
        else
        {
            isBeginning = false;
        }

    }
    assert(numbers < 10);
    if(isBeginning && continuous > 0)
    {
        beginningContinuous = continuous;
    }
    hash |= numbersBits << HashOne::getNumbersStartBit();
    hash |= beginningContinuous << HashOne::getLeftContinuousStartBit();
    hash |= continuous << HashOne::getRightContinuousStartBit();
    hash |= numbers << HashOne::getNumberCountStartBit();
    return hash;
}


static int64_t  sGetValidArrangements(
    const std::unordered_map<uint64_t, int> &hashCounts,
    const uint8_t* numbers,
    int numberCount,
    int numberIndex,
    int continuous,
    int recursionLevel)
{
    int64_t result = 0;
    if(numberIndex >= numberCount)
        return 0;
    int nextNumber = numbers[numberIndex];

    for(auto iter : hashCounts)
    {
        int nextNumberIndex = numberIndex;
        int hashContinous = iter.first >> HashOne::getLeftContinuousStartBit();
        hashContinous &= (1 << HashOne::getLeftContinuousBitAmount()) - 1;

        bool canContinue = false;

        if(continuous + hashContinous == nextNumber)
        {
            nextNumberIndex = numberIndex + 1;
            canContinue = true;
        }
        else if(continuous == 0 && hashContinous == 0)
        {
            canContinue = true;
        }
        if(canContinue)
        {
            int hashNumberCount = iter.first >> HashOne::getNumberCountStartBit();
            hashNumberCount &= (1 << HashOne::getNumberCountBitAmount()) - 1;
            uint64_t hashNumbers64 = iter.first >> HashOne::getNumbersStartBit();
            hashNumbers64 &= (uint64_t(1) << HashOne::getNumbersBitAmount()) - uint64_t(1);

            uint8_t hashRightContinuous = iter.first >> HashOne::getRightContinuousStartBit();
            hashRightContinuous &= (1 << (HashOne::getRightContinuousBitAmount())) - 1;

            bool legitNumbers = true;
            for(int i = 0; i < hashNumberCount; ++i)
            {
                if(nextNumberIndex >= numberCount)
                {
                    legitNumbers = false;
                    break;
                }
                uint64_t numberFromHash = hashNumbers64 & ((1 << HashOne::getNumbersSingleBitAmount()) - 1);
                if(numbers[nextNumberIndex++] != numberFromHash)
                {
                    legitNumbers = false;
                    break;
                }
                hashNumbers64 >>= HashOne::getNumbersSingleBitAmount();
            }
            if(legitNumbers)
            {
                if(recursionLevel == 4 && (hashRightContinuous == 0 && nextNumberIndex == numberCount)
                    && (nextNumberIndex == numberCount - 1 && hashRightContinuous == numbers[nextNumberIndex]))
                {
                    result += iter.second;
                }
                else
                {
                    result += iter.second * sGetValidArrangements(hashCounts, numbers, numberCount, nextNumberIndex,
                            hashRightContinuous, recursionLevel + 1);
                }
            }
        }
    }
    return result;
}


static int64_t sParse12B(const char* data)
{
    TIMEDSCOPE("12B Total");


    std::unordered_map<uint64_t, int> hashCounts;
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
                uint64_t hash = sGetState(line, position);
                hashCounts[hash]++;

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
            assert(hashCounts.size() < 16384);
            /*
            for(const auto iter : hashCounts)
            {
                printf("%" PRIu64 ": counts: %i\n", iter.first, iter.second);
            }
            */
            found = sGetValidArrangements(hashCounts, numbers, numberCount, 0, 0, 0);
            combos += found;
            printf("Found: %" PRIi64 " ones\n", found);


            hashCounts.clear();
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
    printf("12A: Distances: %" PRIi64 "\n", sParse12A(data12A));
    printf("12B: Distances: %" PRIi64 "\n", sParse12B(test12A));
    return 0;
}
#endif

int run12A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse12A(data12A);

    if(printOut)
        charsAdded = sprintf(buffer, "12A: Distances: %" PRIi64, aResult);
    return charsAdded;
}

int run12B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse12B(data12A);

    if(printOut)
        charsAdded = sprintf(buffer, "12B: Distances: %" PRIi64, resultB);

    return charsAdded;
}

