
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
#include <iostream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <ranges>
#include <bitset>

static constexpr int SIZE = 1024;

static const std::vector<std::string> lines = {
    "sesenwnenenewseeswwswswwnenewsewsw",
    "neeenesenwnwwswnenewnwwsewnenwseswesw",
    "seswneswswsenwwnwse",
    "nwnwneseeswswnenewneswwnewseswneseene",
    "swweswneswnenwsewnwneneseenw",
    "eesenwseswswnenwswnwnwsewwnwsene",
    "sewnenenenesenwsewnenwwwse",
    "wenwwweseeeweswwwnwwe",
    "wsweesenenewnwwnwsenewsenwwsesesenwne",
    "neeswseenwwswnwswswnw",
    "nenwswwsewswnenenewsenwsenwnesesenew",
    "enewnwewneswsewnwswenweswnenwsenwsw",
    "sweneswneswneneenwnewenewwneswswnese",
    "swwesenesewenwneswnwwneseswwne",
    "enesenwswwswneneswsenwnewswseenwsese",
    "wnwnesenesenenwwnenwsewesewsesesew",
    "nenewswnwewswnenesenwnesewesw",
    "eneswnwswnwsenenwnwnwwseeswneewsenese",
    "neswnwewnwnwseenwseesewsenwsweewe",
    "wseweeenwnesenwwwswnew"
};

struct Link
{
    Link* nw = nullptr;
    Link* ne = nullptr;
    Link* w = nullptr;
    Link* e = nullptr;
    Link* sw = nullptr;
    Link* se = nullptr;

    uint32_t flipped = 0;
};

enum Dir: int32_t
{
    NONE = 0,
    W = 1,
    E = 2,
    N = 4,
    NW = 5,
    NE = 6,
    S = 8,
    SW = 9,
    SE = 10
};

Dir parsePart(const char **ptr)
{
    const char *&p = ptr[0];
    Dir result = Dir::NONE;
    if(**ptr == '\0')
        return Dir::NONE;

    if(*p == 'n')
    {
        result = Dir::N;
        ++p;
    }
    if(**ptr == 's')
    {
        result = Dir::S;
        ++p;
    }
    if(*p == 'w')
    {
        result = Dir(int32_t(result) + Dir::W);
    }
    if(*p == 'e')
    {
        result = Dir(int32_t(result) + Dir::E);
    }
    ++p;
    return result;
}

int readValues(const char *filename, std::vector<std::string> &outStrs)
{
    std::ifstream f (filename);
    if (!f.is_open())
    {
        printf("Failed to open file: %s\n", filename);
        return -1;
    }

    std::string s;
    while(getline(f, s))
    {
        outStrs.push_back(s);
    }
    f.close();
    return 0;
}

std::vector<std::bitset<SIZE>> simulateBeginningFlips(const std::vector<std::string> &values)
{
    std::vector<std::bitset<SIZE>> bits;
    bits.resize(SIZE, 0);
    for (const std::string& s: values)
    {
        const char* ptr = s.c_str();
        int x = SIZE / 2;
        int y = SIZE / 2;
        while(true)
        {
            Dir d = parsePart(&ptr);
            switch(d)
            {
                case Dir::E:
                {
                    x += 1;
                    break;
                }
                case Dir::W:
                {
                    x -= 1;
                    break;
                }
                case Dir::NE:
                {
                    x += y % 2 == 0 ? 1 : 0;
                    y += 1;
                    break;
                }
                case Dir::NW:
                {
                    x -= y % 2 == 0 ? 0 : 1;
                    y += 1;
                    break;
                }
                case Dir::SE:
                {
                    x += y % 2 == 0 ? 1 : 0;
                    y -= 1;
                    break;
                }
                case Dir::SW:
                {
                    x -= y % 2 == 0 ? 0 : 1;
                    y -= 1;
                    break;
                }
                default:
                {
                    break;
                }
            }
            if(d == Dir::NONE)
            {
                break;
            }
        }
        bits[y].flip(x);
    }
    return bits;
}

int64_t calculatedFlipped(const std::vector<std::bitset<SIZE>>& board)
{
    int64_t flipped = 0;
    for(auto& v : board)
    {
        flipped += v.count();
    }
    return flipped;
}

void partA(const std::vector<std::string> &values)
{
    std::vector<std::bitset<SIZE>> bits = simulateBeginningFlips(values);

    printf("Part a: Flipped ones: %" PRIi64 "\n", calculatedFlipped(bits));
}

void partB(const std::vector<std::string> &values)
{
    std::vector<std::bitset<SIZE>> bits = simulateBeginningFlips(values);
    std::vector<int> ints;
    std::bitset<SIZE> empty = 0;
    for(int k = 0; k < 100; ++k)
    {
        std::bitset<SIZE> prev = 0;
        for(int j = 1; j < SIZE - 1; ++j)
        {
            std::bitset<SIZE> next = bits[j + 1];
            std::bitset<SIZE> curr = bits[j];
            for(int i = 1; i < SIZE - 1; ++i)
            {
                int xInd = i - ((j + 0) % 2);
                int count = 0;

                count += curr[i - 1] ? 1 : 0;
                count += curr[i + 1] ? 1 : 0;

                count += prev[xInd + 0] ? 1 : 0;
                count += prev[xInd + 1] ? 1 : 0;

                count += next[xInd + 0] ? 1 : 0;
                count += next[xInd + 1] ? 1 : 0;

                bits[j][i] = curr[i + 0] ? count == 1 || count == 2 : count == 2;
            }
            prev = curr;
        }
    }
    printf("Part b: winning score: %" PRIi64 "\n", calculatedFlipped(bits));
}


int main(int argc, char** argv)
{
    std::vector<std::string> values;

    if(readValues("data.txt", values) == -1)
    {
        printf("Failed to load map!");
        return 0;
    };

    //partA(lines);
    partA(values);
    partB(values);

    return 0;
}