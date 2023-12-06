#include <algorithm> // std::max
#include <assert.h> // assert
#include <ctype.h> //isdigit
#include <emmintrin.h> // SIMD
#include <stdint.h> // intptr
#include <stdio.h> // printf
#include <stdlib.h> //strtol
#include <string.h> //strlen

#include "input.cpp"

alignas(16) static constexpr char test02A[] =
    R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
              )";
static constexpr size_t test02ASize = sizeof(test02A);
static_assert((test02ASize % 16) == 0, "Need to be dividable by 16 for alignment");


alignas(16) static const char test02B[] = R"(
              )";
static constexpr size_t test02BSize = sizeof(test02B);
static_assert((test02BSize % 16) == 0, "Need to be dividable by 16 for alignment");

static constexpr int MAX_RED = 12;
static constexpr int MAX_GREEN = 13;
static constexpr int MAX_BLUE = 14;

static void sParseSpace(const char** data)
{
    while(**data == ' ')
    {
        ++*data;
    }
}


static int sParse02A(const char* data)
{
    int gameIndex = 1;
    int sumOfValidIds = 0;
    while(*data)
    {
        while(*data != ':' && *data != '\0')
        {
            ++data;
        }
        if(*data == '\0')
        {
            break;
        }
        bool isValid = true;
        while(*data != '\n')
        {
            ++data;
            sParseSpace(&data);
            char* end = nullptr;
            int amount = strtol(data, &end, 10);
            data = end;
            sParseSpace(&data);
            switch(*data)
            {
                case 'r': isValid &= amount <= MAX_RED; data += 3; break;
                case 'g': isValid &= amount <= MAX_GREEN; data += 5; break;
                case 'b': isValid &= amount <= MAX_BLUE; data += 4; break;
            }
        }
        sumOfValidIds += isValid * gameIndex;
        ++gameIndex;
    }
    return sumOfValidIds;
}

static int sParse02B(const char* data)
{
    int sumOfMinProduct = 0;
    while(*data)
    {
        while(*data != ':' && *data != '\0')
        {
            ++data;
        }
        if(*data == '\0')
        {
            break;
        }
        int minR = 0;
        int minG = 0;
        int minB = 0;
        while(*data != '\n')
        {
            ++data;
            sParseSpace(&data);
            char* end = nullptr;
            int amount = strtol(data, &end, 10);
            data = end;
            sParseSpace(&data);
            switch(*data)
            {
                case 'r': minR = std::max(minR, amount); data += 3; break;
                case 'g': minG = std::max(minG, amount); data += 5; break;
                case 'b': minB = std::max(minB, amount); data += 4; break;
            }
        }
        int product = minR * minG * minB;
        sumOfMinProduct += product;
    }
    return sumOfMinProduct;
}

#ifndef RUNNER
int main()
{
    printf("2A: Sum of valid Ids: %i\n", sParse02A(data02A));
    printf("2B: Sum of min multiplies Ids: %i\n", sParse02B(data02A));

    return 0;
}
#endif


int run02A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int aResult = sParse02A(data02A);

    if(printOut)
        charsAdded = sprintf(buffer, "2A: Sum of valid Ids: %i", aResult);
    return charsAdded;
}

int run02B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int resultB = sParse02B(data02A);
    if(printOut)
        charsAdded = sprintf(buffer, "2B: Sum of min multiplies Ids: %i", resultB);

    return charsAdded;
}

