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

static constexpr int MapWidth = 1024;
static constexpr int MapHeight = 512;

alignas(32) static constexpr char test18A[] =
    R"(R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3)
)";

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


struct Point
{
    int x;
    int y;
};

struct Line
{
    Point top;
    Point bot;
};

static int64_t sGetArea(const char* data, bool bPart)
{
    Line lines[1024];
    int lineCount = 0;
    int64_t area = 0;

    int x = 0;
    int y = 0;

    while(*data)
    {
        char dir = *data++;
        int amount = sParserNumber(0, &data);

        if(bPart)
        {
            data += 2;
            amount = 0;
            for (int i = 0; i < 5; ++i)
            {
                if (isdigit(*data))
                    amount = amount * 16 + (*data) - '0';
                else
                    amount = amount * 16 + ((*data) - 'a') + 10;
                data++;
            }
            dir = *data;
        }
        switch(dir)
        {
            case '0': dir = 'R'; break;
            case '1': dir = 'D'; break;
            case '2': dir = 'L'; break;
            case '3': dir = 'U'; break;
        }
        // add outer
        area += amount;

        if(dir == 'U')
        {
            lines[lineCount++] = {.top = {.x = x, .y = y - amount}, .bot = {.x = x, .y = y}  };
        }
        else if(dir == 'D')
        {
            lines[lineCount++] = {.top = {.x = x, .y = y}, .bot = {.x = x, .y = y + amount}  };
        }

        switch(dir)
        {
            case 'R': x += amount; break;
            case 'L': x -= amount; break;
            case 'U': y -= amount; break;
            case 'D': y += amount; break;
        }

        while (*data++ != '\n');
    }

    std::sort(lines, lines + lineCount, [](const Line& a, const Line& b){
        if(a.top.y == b.top.y)
            return a.top.x < b.top.x;
        return a.top.y < b.top.y;
    });
    // divide outer area by 2, adding only half a block
   area /= 2;

    y = lines[0].top.y;
    Line lines2[1024] = {};
    int lineCount2 = 2;
    int nextLineCount = 2;
    lines2[0] = lines[0];
    lines2[1] = lines[1];
    while(lineCount2)
    {
        assert((lineCount2 % 2) == 0);
        int topLow = lines2[0].bot.y;
        for(int i = 0; i < lineCount2; ++i)
        {
            topLow = sMin(topLow, lines2[i].bot.y);
        }
        if(nextLineCount < lineCount)
        {
            topLow = sMin(lines[nextLineCount].top.y, topLow);
        }

        int64_t width = 0;
        for(int i = 0; i < lineCount2; ++i)
        {
            if((i % 2) == 1)
            {
                int64_t change = lines2[i].top.x - lines2[i - 1].top.x;
                assert(change >= 0);
                width += change;
            }
        }
        int64_t height = topLow - y;
        area += width * height;
        y = topLow;

        // add new
        while(lines[nextLineCount].top.y == topLow)
        {
            lines2[lineCount2++] = lines[nextLineCount++];
        }
        // remove old ones
        for(int i = lineCount2 - 1; i >= 0; --i)
        {
            if (topLow == lines2[i].bot.y)
            {
                lines2[i] = lines2[lineCount2 - 1];
                lineCount2--;
            }
        }
        if(lineCount2 > 0)
        {
            std::sort(lines2, lines2 + lineCount2, [](const Line &a, const Line &b) {
                return a.top.x < b.top.x;
            });
        }
    }
    // add one block for some reason. Probably since it would otherwise miss 4 x 1/4 corner blocks.
    return area + 1;
}


static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("18A Total");
    return sGetArea(data, false);
}


static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("18B Total");

    return sGetArea(data, true);
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "18A: Area: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "18B: Total area: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data18A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data18A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run18A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data18A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run18B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data18A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

