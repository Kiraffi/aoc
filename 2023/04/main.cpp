#include <algorithm> // std::max
#include <assert.h> // assert
#include <bit> //std::popcount
#include <ctype.h> //isdigit
#include <immintrin.h> // SIMD
#include <inttypes.h> // PRI64
#include <stdint.h> // intptr
#include <stdio.h> // printf
#include <stdlib.h> //strtol
#include <string.h> //strlen

#include <unordered_map>
#include <unordered_set>

#include "input.cpp"

alignas(16) static constexpr char test04A[] =
    R"(Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11
)";

static char sParserNumber(const char** data)
{
    char number = 0;
    while(**data == ' ') ++*data;
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return number;
}
static int sGetFoundMatchesCount(const char** rowStart)
{
    alignas(32) char rightNumbers[32] = {};
    char leftNumbers[16] = {};
    int leftNumberCount = 0;

    const char* data = *rowStart;

    while(*data++ != ':');

    while(*data != '|')
    {
        char testNumber = sParserNumber(&data);
        leftNumbers[leftNumberCount] = testNumber;
        leftNumberCount++;
    }
    data++;
    int rIndex = 0;
    while(*data != '\n')
    {
        rightNumbers[rIndex] = sParserNumber(&data);
        ++rIndex;
    }

    // Load rightNumbers array into 256bitSimdVector
    __m256i rightNumbersVec = _mm256_lddqu_si256((const __m256i*)rightNumbers);

    // Set the found number bits all 0 on SimdVector
    __m256i foundNumbersVec = _mm256_set1_epi8(0);
    for(int i = 0; i < leftNumberCount; ++i)
    {
        // Fill all the characters in SimdVector with same character
        __m256i number = _mm256_set1_epi8(leftNumbers[i]);
        // Do comparison against rightNumbersVec if any of the characters are matching,
        // this sets all found characters to 0xFF
        __m256i found = _mm256_cmpeq_epi8(number, rightNumbersVec);
        // Or with the foundNumbersVec, this sets the found characters in foundNumbersVec
        foundNumbersVec = _mm256_or_si256(found, foundNumbersVec);
    }
    // Get highest bit on all characters in foundNumbersVec.
    uint32_t matches = _mm256_movemask_epi8(foundNumbersVec);
    // Count set bits on matches.
    matches = std::popcount(matches);


    // Take every other bit
    /* Trying to do popcount manually...
    points = (points & 0x5555'5555)
        // and add them with the other bits after bit shift to right
        + ((points >> 1) & 0x5555'5555);

    // Now 2 bits
    points = (points & 0x3333'3333)
             | ((points >> 2) & 0x3333'3333);
    // Now 4 bits
    points = (points & 0x0f0f'0f0f)
             + ((points >> 4) & 0x0f0f'0f0f);

    // Now 8 bits
    points = (points & 0x00ff'00ff)
             + ((points >> 8) & 0x00ff'00ff);

    // Now 16 bits
    points = (points & 0x0000'ffff)
             + ((points >> 16) & 0x0000'ffff);
    */

    *rowStart = data;
    return (int)matches;
}

static int64_t sParse04A(const char* data)
{
    int64_t sum = 0;
    while(*data)
    {
        int points = sGetFoundMatchesCount(&data);
        points = points > 0 ? (1 << (points - 1)) : 0;

        sum += points;
        ++data;
    }
    return sum;
}

static int64_t sParse04B(const char* data)
{
    int64_t sum = 0;
    int index = 0;
    int64_t cards[256] = {};
    for(int i = 0; i < 256; ++i)
    {
        cards[i] = 1;
    }
    while(*data)
    {
        int points = sGetFoundMatchesCount(&data);
        for(int i = 0; i < points; ++i)
        {
            cards[i + index + 1] += cards[index];
        }
        ++index;
        ++data;
    }

    for(int i = 0; i < index; ++i)
    {
        sum += cards[i];
    }
    return sum;
}

#ifndef RUNNER
int main()
{
    printf("4A: Card points: %" PRIi64 "\n", sParse04A(data04A));

    printf("4B: Cards: %" PRIi64 "\n", sParse04B(data04A));
    return 0;
}
#endif

int run04A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse04A(data04A);

    if(printOut)
        charsAdded = sprintf(buffer, "4A: Card points: %" PRIi64, aResult);
    _mm256_zeroupper();
    return charsAdded;
}

int run04B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse04B(data04A);
    if(printOut)
        charsAdded = sprintf(buffer, "4B: Cards: %" PRIi64, resultB);
    _mm256_zeroupper();

    return charsAdded;
}

