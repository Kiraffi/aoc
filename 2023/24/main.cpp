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

alignas(32) static constexpr char test24A[] =
    R"()";

/*
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
/*
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
*/
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


static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("24A Total");
    return *data;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("24B Total");
    return *data;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "24A: Result: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "24B: Result: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data24A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data24A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run24A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data24A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run24B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data24A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

