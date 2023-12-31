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

alignas(16) static constexpr char test06A[] =
    R"(Time:      7  15   30
Distance:  9  40  200
)";

static int64_t sParserNumber(int64_t startNumber, const char** data)
{
    int64_t number = startNumber;
    while(**data == ' ') ++*data;
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return number;
}

static int64_t sCalculatePossibleWinAmounts(int64_t time, int64_t distance)
{
    // n * (t - n) = distance => -n^2 + t*n - distance = 0, solve n
    // x = (-b +- sqrt(b^2 - 4ac))/ (2a) to solve quadratic equation.
    double a = -1.0;
    double b = double(time);
    double c = double(-distance);

    double twoA = 2.0 * a;
    double twoC = 2.0 * c;
    double fourAc = twoA * twoC;
    double b2 = b * b;

    double sqrtValue = b2 - fourAc;
    assert(sqrtValue >= 0.0);

    // Since we do want positive value, we always take only + one from -b +- sqrt
    double x = (-b + sqrt(sqrtValue)) / twoA;
    int64_t n = ceil(x);

    // In case current record would be same distance, we need to go further
    if(n * (time - n) == distance)
    {
        n++;
    }
    // Calculated value should always be less than time / 2, otherwise we cannot find any value
    assert(n * 2 < time);
    int64_t newRecords = time - 2 * n + 1;
    return newRecords;
}

static int64_t sParse06A(const char* data)
{
    int64_t times[4];
    int64_t distances[4];

    int values = 0;

    while(*data++ != ':');
    while(*data != '\n')
    {
        times[values++] = sParserNumber(0, &data);
    }
    while(*data++ != ':');
    values = 0;
    while(*data != '\n')
    {
        distances[values++] = sParserNumber(0, &data);
    }

    int64_t records = 1;

    for(int i = 0; i < values; ++i)
    {
        records *= sCalculatePossibleWinAmounts(times[i], distances[i]);
    }
    return records;
}

static int64_t sParse06B(const char* data)
{
    int64_t time = 0;
    int64_t distance = 0;

    while(*data++ != ':');
    while(*data != '\n')
    {
        time = sParserNumber(time, &data);
    }
    while(*data++ != ':');
    while(*data != '\n')
    {
        distance = sParserNumber(distance, &data);
    }

    int64_t records = sCalculatePossibleWinAmounts(time, distance);
    return records;
}

#ifndef RUNNER
int main()
{
    printf("6A: Potential records: %" PRIi64 "\n", sParse06A(data06A));
    printf("6B: Potential records: %" PRIi64 "\n", sParse06B(data06A));
    return 0;
}
#endif

int run06A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse06A(data06A);

    if(printOut)
        charsAdded = sprintf(buffer, "6A: Potential records: %" PRIi64, aResult);
    return charsAdded;
}

int run06B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse06B(data06A);

    if(printOut)
        charsAdded = sprintf(buffer, "6B: Potential records: %" PRIi64, resultB);

    return charsAdded;
}

