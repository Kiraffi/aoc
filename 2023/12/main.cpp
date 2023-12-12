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

    while(*data)
    {
        //const char* lineStart = data;
        //const char* end = data;
        uint8_t numbers[32] = {};
        int numberCount = 0;

        uint8_t unknownPositions[128] = {};
        int unknowns = 0;

        int found = 0;
        int position = 0;

        char line[128] = {};
        int lineLen = 0;

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
                    unknownPositions[unknowns++] = position;
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
            unknownPositions[unknowns++] = position;
            line[position++] = '?';

        }
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
        }

        printf("found: %i\n", found);
        assert(found);

        ++data;
    }
    return combos;
}



static int64_t sParse12B(const char* data)
{
    TIMEDSCOPE("12B Total");
    int64_t combos = 0;

    while(*data)
    {
        const char* lineStart = data;
        const char* end = data;
        uint8_t numbers[8] = {};
        int numberCount = 0;

        //uint8_t unknownPositions[8] = {};
        int unknowns = 0;

        int found = 0;
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
                ++unknowns;
                //unknownPositions[unknowns++] = position;
            }
            else if(c == ' ')
            {
                end = data + 1;
            }
            data++;
            position++;
        }

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
            int continous = 0;
            bool isRight = true;
            for(int k = 0; k < j; k++)
            {
                if(line[k] == '#')
                {
                    ++continous;
                }
                else
                {
                    if(continous)
                    {
                        if(numbers[number] != continous)
                        {
                            isRight = false;
                            break;
                        }
                        ++number;
                    }
                    continous = 0;
                }
            }
            if(isRight && number == numberCount)
            {
                combos++;
                found++;
            }
        }

        assert(found);

        ++data;
    }
    return combos;
}

#ifndef RUNNER
int main()
{
    printf("12A: Distances: %" PRIi64 "\n", sParse12A(test12A));
    printf("12B: Distances: %" PRIi64 "\n", sParse12B(data12A));
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

