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

alignas(16) static constexpr char test13A[] =
    R"(#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#
)";

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
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return neg ? -number : number;
}
*/

static bool sCheckMatchCols(const char* map, int cols, int rows, int col)
{
    for(int row = 0; row < rows; ++row)
    {

        int offset = 0;
        while(col - offset >= 0 && col + offset + 1 < cols)
        {
            // +1 per row for \n

            int index1 = row  * (cols + 1) + col - offset + 0;
            int index2 = row  * (cols + 1) + col + offset + 1;
            assert(index1 >= 0 && index1 < rows * (cols + 1));
            assert(index2 >= 0 && index2 < rows * (cols + 1));
            char ch1 = map[index1];
            char ch2 = map[index2];
            if(ch1 != ch2)
            {
                return false;
            }
            ++offset;
        }
    }
    return true;
}

static bool sCheckMatchRows(const char* map, int cols, int rows, int row)
{
    for(int col = 0; col < cols; ++col)
    {
        int offset = 0;
        while(row - offset >= 0 && row + offset + 1 < rows)
        {
            // +1 per row for \n
            int index1 = (row - offset + 0)  * (cols + 1) + col;
            int index2 = (row + offset + 1)  * (cols + 1) + col;
            assert(index1 >= 0 && index1 < rows * (cols + 1));
            assert(index2 >= 0 && index2 < rows * (cols + 1));
            if(map[index1] != map[index2])
            {
                return false;
            }
            ++offset;
        }
    }
    return true;
}

static int sFindMatch(const int32_t* arr, int count, int invalidIndex)
{
    for(int i = 0; i < count - 1; ++i)
    {
        if(i == invalidIndex)
            continue;
        bool found = true;
        int offset = 0;
        while(i - offset >= 0 && i + offset + 1 < count)
        {
            int32_t i1 = arr[(i - offset + 0)];
            int32_t i2 = arr[(i + offset + 1)];
            if (i1 != i2)
            {
                found = false;
                break;
            }
            ++offset;
        }
        if(found)
        {
            return i + 1;
        }
    }
    return 0;
}

static int sFindMatch(const char* map, int cols, int rows)
{
    for(int i = 0; i < cols - 1; ++i)
    {
        if(sCheckMatchCols(map, cols, rows, i))
            return (i + 1);
    }
    for(int j = 0; j < rows - 1; ++j)
    {
        if(sCheckMatchRows(map, cols, rows, j))
            return (j + 1) * 100;
    }
    assert(false);
    return 0;
}

static int64_t sParse13A(const char* data)
{
    TIMEDSCOPE("13A Total");
    int64_t sum = 0;
    while(*data)
    {
        const char* map = data;
        int rows = 1;
        int cols = 0;
        while(*data != '\n')
        {
            cols++;
            data++;
        }
        data++;
        while(*data && *data != '\n')
        {
            data += cols + 1;
            rows++;
        }

        {
            //TIMEDSCOPE("13A FindMatch");
            int matchCol = sFindMatch(map, cols, rows);
            sum += matchCol;

        }
        if(*data == '\n')
            data++;
    }


    return sum;
}


static int64_t sParse13B(const char* data)
{
    TIMEDSCOPE("13B Total");
    int64_t sum = 0;
    while(*data)
    {
        int rowCount = 0;
        int colCount = 0;
        int32_t cols[32] = {};
        int32_t rows[32] = {};

        while(*data && *data != '\n')
        {
            colCount = 0;
            while(*data != '\n')
            {
                cols[colCount] <<= 1;
                rows[rowCount] <<= 1;
                if(*data == '#')
                {
                    cols[colCount] |= 1;
                    rows[rowCount] |= 1;
                }
                colCount++;
                data++;
            }
            rowCount++;
            data++;
        }

        {
            //TIMEDSCOPE("13B FindMatch");
            bool found = false;
            int oldI = sFindMatch(cols, colCount, -1) - 1;
            int oldJ = sFindMatch(rows, rowCount, -1) - 1;
            for(int j = 0; j < rowCount && !found; ++j)
            {
                for (int i = 0; i < colCount; ++i)
                {
                    rows[j] ^= 1 << i;
                    cols[i] ^= 1 << j;
                    int matchCol = sFindMatch(cols, colCount, oldI);
                    if(matchCol)
                    {
                        rows[j] ^= 1 << i;
                        cols[i] ^= 1 << j;
                        sum += matchCol;
                        found = true;
                        break;

                    }
                    int matchRow = sFindMatch(rows, rowCount, oldJ);
                    if(matchRow)
                    {
                        rows[j] ^= 1 << i;
                        cols[i] ^= 1 << j;
                        sum += matchRow * 100;
                        found = true;
                        break;

                    }
                    rows[j] ^= 1 << i;
                    cols[i] ^= 1 << j;
                }
            }
        }
        if(*data == '\n')
            data++;
    }


    return sum;
}

#ifndef RUNNER
int main()
{
    printf("13A: Sum of mirrors: %" PRIi64 "\n", sParse13A(data13A));
    printf("13B: Sum of mirrors: %" PRIi64 "\n", sParse13B(data13A));
    return 0;
}
#endif

int run13A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse13A(data13A);

    if(printOut)
        charsAdded = sprintf(buffer, "13A: Mirrors: %" PRIi64, aResult);
    return charsAdded;
}

int run13B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse13B(data13A);

    if(printOut)
        charsAdded = sprintf(buffer, "13B: Mirrors: %" PRIi64, resultB);

    return charsAdded;
}

