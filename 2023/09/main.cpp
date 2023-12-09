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

alignas(16) static constexpr char test09A[] =
    R"(0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45
)";

using Fn = void(*)(const int64_t* numbers, int numberIndex);

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

template<typename T>
static void sGetValues(const char* data, T&& lambdaFn)
{
    while (*data)
    {
        int64_t numbers[32][32] = {};
        int64_t number = sParserNumber(0, &data);
        numbers[0][0] = number;

        int numberIndex = 1;
        int64_t zeros = 0;
        while (*data != '\n')
        {
            int64_t newNumber = sParserNumber(0, &data);
            int64_t diff = newNumber - number;
            number = newNumber;

            numbers[0][numberIndex] = newNumber;
            numbers[1][numberIndex - 1] = diff;

            zeros |= diff;
            numberIndex++;
        }
        int level = 2;
        numberIndex -= 2;
        while (zeros)
        {
            zeros = 0;
            for (int i = 0; i < numberIndex; ++i)
            {
                int64_t newNumber = numbers[level - 1][i + 1] - numbers[level - 1][i];
                numbers[level][i] = newNumber;
                zeros |= newNumber;
            }
            level++;
            numberIndex--;
        }
        level--;
        numberIndex++;
        while (level)
        {
            lambdaFn(numbers[level - 1], numberIndex, level - 1);
            ++numberIndex;
            --level;
        }
        ++data;
    }
}

static int64_t sParse09A(const char* data)
{
    int64_t sum = 0;
    sGetValues(data, [&](const int64_t* numbers, int numberIndex, int)
    {
        sum += numbers[numberIndex];
    });
    return sum;
}

static int64_t sParse09B(const char* data)
{
    int64_t sum = 0;
    int64_t previousNumber = 0;
    sGetValues(data, [&](const int64_t* numbers, int, int level)
    {
        int64_t newNumber = numbers[0];
        previousNumber = newNumber - previousNumber;
        if(level == 0)
        {
            sum += previousNumber;
            previousNumber = 0;
        }
    });
    return sum;
}

#ifndef RUNNER
int main()
{
    printf("9A: Potential records: %" PRIi64 "\n", sParse09A(data09A));
    printf("9B: Potential records: %" PRIi64 "\n", sParse09B(data09A));
    return 0;
}
#endif

int run09A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse09A(data09A);

    if(printOut)
        charsAdded = sprintf(buffer, "9A: Potential records: %" PRIi64, aResult);
    return charsAdded;
}

int run09B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse09B(data09A);

    if(printOut)
        charsAdded = sprintf(buffer, "9B: Potential records: %" PRIi64, resultB);

    return charsAdded;
}

