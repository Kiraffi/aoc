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

alignas(16) static constexpr char testA[] =
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

bool sCheckValid(char c)
{
    if(isdigit(c))
        return false;

    if(c == '.')
        return false;

    return true;
}

void parseA(const char* data)
{
    const char* start = data;
    const char* tmp = data;
    int sum = 0;
    int width = 0;
    int height = 0;
    int tmpWidth = 0;
    while(*tmp)
    {
        if(*tmp++ == '\n')
        {
            ++height;
            tmpWidth = 0;
            continue;
        }
        tmpWidth++;
        width = width < tmpWidth ? tmpWidth : width;
    }

    int x = 0;
    int y = 0;
    int digit = 0;
    bool isValidDigit = false;
    while(*data)
    {
        if(*data == '\n')
        {
            x = 0;
            y++;
            data++;
            continue;
        }
        else if(isdigit(*data))
        {
            digit = digit * 10 + (*data - '0');
            for(int j = y - 1; j <= y + 1; ++j)
            {
                for (int i = x - 1; i <= x + 1; ++i)
                {
                    if(i >= 0 && j >= 0 && i < width && j < height)
                    {
                        isValidDigit |= sCheckValid(*(start + (width + 1) * j + i));
                    }
                }
            }
        }
        else
        {
            if(digit > 0 && isValidDigit)
            {
                sum += digit;
            }
            digit = 0;
            isValidDigit = false;
        }
        ++data;
        ++x;
    }


    printf("3A: Sum of valid digits: %i\n", sum);
}

void parseB(const char* data)
{
    std::unordered_map<int, std::vector<int>> gearNumbers;
    const char* start = data;
    const char* tmp = data;
    int width = 0;
    int height = 0;
    int tmpWidth = 0;
    while(*tmp)
    {
        if(*tmp++ == '\n')
        {
            ++height;
            tmpWidth = 0;
            continue;
        }
        tmpWidth++;
        width = width < tmpWidth ? tmpWidth : width;
    }

    int x = 0;
    int y = 0;
    int digit = 0;
    std::unordered_set<int> gearIndices;
    while(*data)
    {
        if(*data == '\n')
        {
            x = 0;
            y++;
            data++;
            continue;
        }
        else if(isdigit(*data))
        {
            digit = digit * 10 + (*data - '0');
            for(int j = y - 1; j <= y + 1; ++j)
            {
                for (int i = x - 1; i <= x + 1; ++i)
                {
                    if(i >= 0 && j >= 0 && i < width && j < height)
                    {
                        //if(sCheckValid(*(start + width * j + i)))
                        //    printf("%i,%i is valid\n", i, j);
                        char cc = *(start + (width + 1) * j + i);
                        if(cc == '*')
                        {
                            gearIndices.insert(width * j + i);
                        }
                    }
                }
            }
        }
        else
        {
            if(digit > 0 && !gearIndices.empty())
            {
                for(int index : gearIndices)
                {
                    gearNumbers[index].push_back(digit);
                }
            }
            digit = 0;
            gearIndices.clear();
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

    printf("3B: Sum of gear ratios: %i\n", sum);
}


int main()
{
    parseA(dataA);
    parseB(dataA);
    return 0;
}
