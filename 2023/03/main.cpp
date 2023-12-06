#include <algorithm> // std::max
#include <assert.h> // assert
#include <ctype.h> //isdigit
#include <stdint.h> // intptr
#include <stdio.h> // printf
#include <stdlib.h> //strtol
#include <string.h> //strlen

#include <unordered_map>
#include <unordered_set>

#include "input.cpp"

alignas(16) static constexpr char test03A[] =
    R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)";

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

static bool sCheckValid(char c)
{
    if(isdigit(c))
        return false;

    if(c == '.')
        return false;

    return true;
}

static void sParse03A(const char* data, bool printOut)
{
    const char* start = data;
    int sum = 0;
    int width = 0;
    int height = 0;

    sGetSize(data, width, height);

    int x = 0;
    int y = 0;
    int digit = 0;
    int digitCount = 0;

    while(*data)
    {
        if(isdigit(*data))
        {
            ++digitCount;
            digit = digit * 10 + (*data - '0');
        }
        else
        {
            if(digit > 0)
            {
                int startX = x - digitCount - 1;
                startX = startX > 0 ? startX : 0;
                int endX = x < width ? x : width - 1;

                int startY = y > 0 ? y - 1 : 0;
                int endY = y + 1 < height ? y + 1 : height - 1;

                while(startY <= endY)
                {
                    int i = startX;
                    const char* pos = start + startY * (width + 1) + startX;
                    while(i++ <= endX)
                    {
                        if(sCheckValid(*pos++))
                        {
                            startY = endY + 1;
                            sum += digit;
                            break;

                        }
                    }
                    startY++;
                }
            }
            digit = 0;
            digitCount = 0;
            if(*data == '\n')
            {
                x = 0;
                y++;
                data++;
                continue;
            }
        }
        ++data;
        ++x;
    }

    if(printOut)
        printf("3A: Sum of valid digits: %i\n", sum);
}

static void sParse03B(const char* data, bool printOut)
{
    std::unordered_map<int, std::vector<int>> gearNumbers;
    const char* start = data;
    int width = 0;
    int height = 0;
    sGetSize(data, width, height);
    int x = 0;
    int y = 0;
    int digit = 0;
    int digitCount = 0;

    while(*data)
    {
        if(isdigit(*data))
        {
            ++digitCount;
            digit = digit * 10 + (*data - '0');
        }
        else
        {
            if(digit > 0)
            {
                int startX = x - digitCount - 1;
                startX = startX > 0 ? startX : 0;
                int endX = x < width ? x : width - 1;

                int startY = y > 0 ? y - 1 : 0;
                int endY = y + 1 < height ? y + 1 : height - 1;

                while(startY <= endY)
                {
                    int i = startX;
                    const char* pos = start + startY * (width + 1) + startX;
                    while(i++ <= endX)
                    {
                        char cc = *pos++;
                        if(cc == '*')
                        {
                            gearNumbers[width * startY + i].push_back(digit);
                        }
                    }
                    startY++;
                }
            }
            digit = 0;
            digitCount = 0;
            if(*data == '\n')
            {
                x = 0;
                y++;
                data++;
                continue;
            }

        }
        ++data;
        ++x;

    }
    int sum = 0;
    for(const auto& value : gearNumbers)
    {
        if(value.second.size() == 2)
        {
            sum += value.second[0] * value.second[1];
        }
    }
    if(printOut)
        printf("3B: Sum of gear ratios: %i\n", sum);
}

#ifndef RUNNER
int main()
{
    sParse03A(data03A, true);
    sParse03B(data03A, true);
    return 0;
}
#endif


void run03A(bool printOut)
{
    sParse03A(data03A, printOut);
}
void run03B(bool printOut)
{
    sParse03B(data03A, printOut);
}
