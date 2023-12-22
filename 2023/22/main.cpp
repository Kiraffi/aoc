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

alignas(32) static constexpr char test22A[] =
    R"(1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9
)";

alignas(32) static constexpr char test22B[] =
    R"(0,0,4~2,0,4
3,0,3~3,3,3
0,0,3~0,3,3
1,0,3~1,3,3
2,0,3~2,3,3
0,0,2~9,9,2
)";

alignas(32) static constexpr char test22C[] =
    R"(0,0,4~2,0,4
0,0,2~0,0,2
0,0,3~0,3,3
1,0,3~1,3,3
2,0,3~2,3,3
0,0,2~9,9,2
)";

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

static constexpr int XSize = 16;
static constexpr int YSize = 16;
static constexpr int ZSize = 512;
static constexpr int MaxBlocks = 2048;

static int sGetIndex(int x, int y, int z)
{
    return x + y * XSize + z * XSize * YSize;
}

static void sSetTile(int x, int y, int z, uint16_t value, uint16_t* map)
{
    map[sGetIndex(x, y, z)] = value;
}

static uint16_t sGetTile(int x, int y, int z, const uint16_t* map)
{
    uint16_t value = map[sGetIndex(x, y, z)];
    return value;
}
template <typename T>
static void sSwapIfSmaller(T& a, T& b)
{
    if(b < a)
    {
        std::swap(a, b);
    }
}

struct Block
{
    uint16_t z1;
    uint16_t z2;
    uint8_t x1;
    uint8_t x2;
    uint8_t y1;
    uint8_t y2;
};

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("22A Total");
    uint16_t map[XSize * YSize * ZSize] = {};
    Block blocks[MaxBlocks] = {};
    uint16_t numberCount = 1;

    while(*data)
    {
        Block &b = blocks[numberCount];
        b.x1 = sParserNumber(0, &data);
        ++data;
        b.y1 = sParserNumber(0, &data);
        ++data;
        b.z1 = sParserNumber(0, &data);
        ++data;
        b.x2 = sParserNumber(0, &data);
        ++data;
        b.y2 = sParserNumber(0, &data);
        ++data;
        b.z2 = sParserNumber(0, &data);
        ++data;
        assert(b.x1 >= 0 && b.x2 >= 0 && b.y1 >= 0 && b.y2 >= 0 && b.z1 >= 0 && b.z2 >= 0);
        assert(b.x1 < XSize && b.x2 < XSize && b.y1 < YSize && b.y2 < YSize && b.z1 < ZSize && b.z2 < ZSize);
        sSwapIfSmaller(b.x1, b.x2);
        sSwapIfSmaller(b.y1, b.y2);
        sSwapIfSmaller(b.z1, b.z2);
        ++numberCount;
    }
    assert(numberCount < MaxBlocks);

    std::sort(blocks + 1, blocks + numberCount, [](const Block& a, const Block& b){
        return a.z1 < b.z1;
    });


    using SetMap = std::unordered_set<uint16_t>;
    SetMap supporteds[MaxBlocks];
    uint8_t supportsAnotherBlock[MaxBlocks] = {};
    uint64_t hitTest[ZSize * 4] = {};
    for(int n = 1; n < numberCount; ++n)
    {
        Block& b = blocks[n];

        uint16_t& z1 = b.z1;
        uint16_t& z2 = b.z2;
        uint8_t& x1 = b.x1;
        uint8_t& x2 = b.x2;
        uint8_t& y1 = b.y1;
        uint8_t& y2 = b.y2;

        [[maybe_unused]] int loop = 0;
        bool hit = false;

        uint64_t bHit[4] = {};
        for (int j = y1; j <= y2; ++j)
        {
            uint64_t& bHitB = bHit[j / 4];
            uint64_t newBits1 = (uint64_t(1) << (x2 + 1)) - 1;
            uint64_t newBits2 = (uint64_t(1) << (x1 + 1)) - 1;
            uint64_t newBits = newBits1 - newBits2;
            newBits = newBits << ((j % 4) * 16);
            bHitB |= newBits;
            /*
            for (int i = x1; i <= x2; ++i)
            {
                if(sGetTile(i, j, z1, map))
                {
                    hit = true;
                    assert(loop > 0);
                    break;
                }
            }
             */
        }

        while(!hit)
        {
            for (int j = y1; j <= y2; ++j)
            {
                for (int i = x1; i <= x2; ++i)
                {
                    if(sGetTile(i, j, z1, map))
                    {
                        hit = true;
                        assert(loop > 0);
                        break;
                    }
                }
            }
            assert(!hit || z1 >= 1);
            if(z1 == 1 && !hit)
            {
                hit = true;
                --z1;
                --z2;
            }
            --z1;
            --z2;
            ++loop;
        }
        z1 += 2;
        z2 += 2;
        for(int k = z1; k <= z2; ++k)
        {
            for (int j = y1; j <= y2; ++j)
            {
                for (int i = x1; i <= x2; ++i)
                {
                    sSetTile(i, j, k, n, map);
                    uint16_t underValue = sGetTile(i, j, k - 1, map);
                    if((underValue != n) & (underValue != 0))
                    {
                        assert(n);
                        supporteds[n].insert(underValue);
                        assert(supportsAnotherBlock[underValue] <= 255);
                        ++supportsAnotherBlock[underValue];
                    }
                }
            }
        }
    }
/*
    for(int i = 1; i < numberCount; ++i)
    {
        const Block& b = blocks[i];
        printf("%04i:  %i, %i, %i - %i, %i, %i\n", i, b.x1, b.y1, b.z1, b.x2, b.y2, b.z2);
    }
*/
    SetMap removals;
    uint8_t onlyOneSupport[MaxBlocks] = {};
    for(const auto& setMapIter : supporteds)
    {
        if(setMapIter.size() == 1)
        {
            //printf("Removals %04i:           ", setMapIter.first);
            for(const auto& iter : setMapIter)
            {
                onlyOneSupport[iter]++;
                //printf("%i, ", iter);
            }
            //printf("\n");
            //removals.insert(setMapIter.second.begin(), setMapIter.second.end());
        }
    }
    int64_t removalCount = 0;
    for(int i = 1; i < numberCount; ++i)
    {
        if(supportsAnotherBlock[i] == 0)
        {
            removalCount++;
            //printf("would add: %i\n", i);
            //removals.insert(i);
        }
        else if(onlyOneSupport[i] == 0)
        {
            removalCount++;
            //printf("Non support: %i\n", i);
            //removals.insert(i);
        }
    }
/*
    for(const auto& iter : removals)
    {
        printf("removed: %i\n", iter);
    }
*/
    return removalCount;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("22B Total");
    uint16_t map[XSize * YSize * ZSize] = {};
    Block blocks[MaxBlocks] = {};
    uint16_t numberCount = 1;

    while(*data)
    {
        Block &b = blocks[numberCount];
        b.x1 = sParserNumber(0, &data);
        ++data;
        b.y1 = sParserNumber(0, &data);
        ++data;
        b.z1 = sParserNumber(0, &data);
        ++data;
        b.x2 = sParserNumber(0, &data);
        ++data;
        b.y2 = sParserNumber(0, &data);
        ++data;
        b.z2 = sParserNumber(0, &data);
        ++data;
        assert(b.x1 >= 0 && b.x2 >= 0 && b.y1 >= 0 && b.y2 >= 0 && b.z1 >= 0 && b.z2 >= 0);
        assert(b.x1 < XSize && b.x2 < XSize && b.y1 < YSize && b.y2 < YSize && b.z1 < ZSize && b.z2 < ZSize);
        sSwapIfSmaller(b.x1, b.x2);
        sSwapIfSmaller(b.y1, b.y2);
        sSwapIfSmaller(b.z1, b.z2);
        ++numberCount;
    }
    assert(numberCount < MaxBlocks);

    std::sort(blocks + 1, blocks + numberCount, [](const Block& a, const Block& b){
        return a.z1 < b.z1;
    });


    using SetMap = std::unordered_set<uint16_t>;
    std::unordered_map<uint16_t, SetMap> supporteds;
    std::unordered_map<uint16_t, SetMap> supportBlocks;

    for(int n = 1; n < numberCount; ++n)
    {
        Block& b = blocks[n];

        uint16_t& z1 = b.z1;
        uint16_t& z2 = b.z2;
        uint8_t& x1 = b.x1;
        uint8_t& x2 = b.x2;
        uint8_t& y1 = b.y1;
        uint8_t& y2 = b.y2;

        [[maybe_unused]] int loop = 0;
        bool hit = false;
        while(!hit)
        {
            for (int j = y1; j <= y2 && !hit; ++j)
            {
                for (int i = x1; i <= x2; ++i)
                {
                    if(sGetTile(i, j, z1, map))
                    {
                        hit = true;
                        assert(loop > 0);
                        break;
                    }
                }
            }
            assert(!hit || z1 >= 1);
            if(z1 == 1 && !hit)
            {
                hit = true;
                --z1;
                --z2;
            }
            --z1;
            --z2;
            ++loop;
        }
        z1 += 2;
        z2 += 2;
        for(int k = z1; k <= z2; ++k)
        {
            for (int j = y1; j <= y2; ++j)
            {
                for (int i = x1; i <= x2; ++i)
                {
                    sSetTile(i, j, k, n, map);
                    uint16_t underValue = sGetTile(i, j, k - 1, map);
                    if((underValue != n) & (underValue != 0))
                    {
                        assert(n);
                        supporteds[n].insert(underValue);
                        supportBlocks[underValue].insert(n);
                    }
                }
            }
        }
    }
    int64_t removalCount = 0;




    //for(int i = 0; i < amou)

    return removalCount;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "22A: Removable blocks: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "22B: Spots: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data22A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data22A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run21A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data22A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run21B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data22A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

