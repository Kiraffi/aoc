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

#include <vector>

#include <unordered_map>
#include <unordered_set>

#include "input.cpp"

struct ConversionMap
{
    int64_t offset;
    int64_t first;
    int64_t last;
};

struct Range
{
    int64_t first;
    int64_t last;
};

// Globals
std::vector<ConversionMap> sConversionMaps[7];
std::vector<int64_t> sSeeds;

alignas(16) static constexpr char testA[] =
    R"(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4
)";

int64_t sParserNumber(const char** data)
{
    int64_t number = 0;
    while(**data == ' ') ++*data;
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return number;
}

void sParseValues(const char* data)
{
    while(*data++ != ':');
    while(*data != '\n')
    {
        sSeeds.push_back(sParserNumber(&data));
    }
    data++;
    ConversionMap map = {};
    for(std::vector<ConversionMap>& conversionMaps : sConversionMaps)
    {
        while(*data++ != '\n');
        while(*data++ != '\n');
        while(*data >= '0')
        {
            // Calculate begin and end inclusive. Also calculate dst - src offset.
            int64_t dst = sParserNumber(&data);
            int64_t src = sParserNumber(&data);
            int64_t range = sParserNumber(&data);
            map.first = src;
            map.last = src + range - 1;
            map.offset = dst - src;
            conversionMaps.emplace_back(map);

            data++;
        }
        std::sort(conversionMaps.begin(), conversionMaps.end(),
            [](const ConversionMap& a, const ConversionMap& b)
            {
                return a.first < b.first;
            });
    }
}

void parseA()
{
    int64_t smallestSeed = 1 << 30;
    for(int64_t seed : sSeeds)
    {
        for(const std::vector<ConversionMap>& conversionMaps : sConversionMaps)
        {
            for(const ConversionMap& map : conversionMaps)
            {
                if(seed >= map.first && seed <= map.last)
                {
                    seed += map.offset;
                    break;
                }
            }
        }
        smallestSeed = seed < smallestSeed ? seed : smallestSeed;
    }
    printf("5A: Smallest seed: %" PRIi64 "\n", smallestSeed);
}

void parseB()
{
    int64_t smallestSeed = 1 << 30;

    std::vector<Range> rangeA;
    std::vector<Range> rangeB;

    // Reserving some size to reduce resize from pushbacks.
    rangeA.reserve(1024);
    rangeB.reserve(1024);

    for(size_t i = 0; i < sSeeds.size(); i += 2)
    {
        // Transform begin, range -> [begin, end] inclusive
        int64_t begin = sSeeds[i];
        int64_t range = sSeeds[i + 1];
        rangeA.push_back({begin, begin + range - 1});
    }

    std::vector<Range>* current = &rangeA;
    std::vector<Range>* other = &rangeB;

    for(const std::vector<ConversionMap>& conversionMaps : sConversionMaps)
    {
        other->clear();

        for(Range range : *current)
        {
            for(const ConversionMap& map : conversionMaps)
            {
                // Skip until we find something that starts after range.first
                if(range.first > map.last)
                {
                    continue;
                }
                // Remove the range until map.first - 1
                if(range.first < map.first)
                {
                    int64_t last = range.last < map.first ? range.last : map.first - 1;
                    other->push_back({range.first, last});
                    range.first = last + 1;
                    if(range.first > range.last)
                    {
                        break;
                    }
                }
                // Remove the range until map.last. The range.first must be >= map.first
                if(range.first <= map.last)
                {
                    int64_t last = range.last < map.last ? range.last : map.last;
                    other->push_back({range.first + map.offset, last + map.offset});
                    range.first = last + 1;
                    if(range.first > range.last)
                    {
                        break;
                    }
                }
            }
            // If anything is left, push it to next round.
            if(range.first <= range.last)
            {
                other->push_back(range);
            }

            std::swap(current, other);
        }
    }
    for(const Range& range : *current)
    {
        smallestSeed = range.first < smallestSeed ? range.first : smallestSeed;
    }
    printf("5B: Smallest seed: %" PRIi64 "\n", smallestSeed);
}


int main()
{
    sParseValues(dataA);
    parseA();
    parseB();
    return 0;
}
