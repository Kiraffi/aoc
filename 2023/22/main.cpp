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

static constexpr int XSize = 10;
static constexpr int YSize = 10;
static constexpr int RowSize = 128;
static constexpr int ZSize = 512;
static constexpr int MaxBlocks = (1250 + 15) / 16 * 16;

static int sGetIndex(int x, int y, int z)
{
    return x + y * XSize + z * RowSize;
}

static void sSetTile(int x, int y, int z, int16_t value, int16_t* map)
{
    map[sGetIndex(x, y, z)] = value;
}

static int16_t sGetTile(int x, int y, int z, const int16_t* map)
{
    int16_t value = map[sGetIndex(x, y, z)];
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

static void sReadMapAndSort(const char* data, Block* blocks, int16_t& numberCount)
{
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
}

static void sDropPiece(Block* blocks, int16_t numberCount)
{
    alignas(16) uint64_t hitTest[ZSize * 4] = {};

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
            uint64_t newBits2 = (uint64_t(1) << (x1 + 0)) - 1;
            uint64_t newBits = newBits1 - newBits2;
            newBits = newBits << ((j % 4) * 16);
            bHitB |= newBits;
        }
        while(!hit)
        {
            uint64_t testValue[4] = {};
            testValue[0] = hitTest[z1 * 4 + 0] & bHit[0];
            testValue[1] = hitTest[z1 * 4 + 1] & bHit[1];
            testValue[2] = hitTest[z1 * 4 + 2] & bHit[2];
            testValue[3] = hitTest[z1 * 4 + 3] & bHit[3];
            testValue[0] = testValue[0] | testValue[1];
            testValue[2] = testValue[2] | testValue[3];
            testValue[0] = testValue[0] | testValue[2];
            hit = testValue[0] != 0;

            assert(!hit || z1 >= 1);
            if(z1 == 1)
            {
                hit = true;
            }
            --z1;
            --z2;
            ++loop;
        }
        z1 += 2;
        z2 += 2;
        for(int k = z1; k <= z2; ++k)
        {
            hitTest[k * 4 + 0] |= bHit[0];
            hitTest[k * 4 + 1] |= bHit[1];
            hitTest[k * 4 + 2] |= bHit[2];
            hitTest[k * 4 + 3] |= bHit[3];
        }
    }

}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("22A Total");
    int16_t map[RowSize * ZSize] = {};
    Block blocks[MaxBlocks] = {};
    int16_t numberCount = 1;
    sReadMapAndSort(data, blocks, numberCount);


    alignas(16) uint8_t onlyOneSupport[MaxBlocks] = {};
    onlyOneSupport[0] = 1;

    sDropPiece(blocks, numberCount);

    for(int n = 1; n < numberCount; ++n)
    {
        const Block& b = blocks[n];

        uint16_t z1 = b.z1;
        uint16_t z2 = b.z2;
        uint8_t x1 = b.x1;
        uint8_t x2 = b.x2;
        uint8_t y1 = b.y1;
        uint8_t y2 = b.y2;

        uint16_t supports[16] = {};
        uint8_t supportCount = 0;

        for(int k = z1; k <= z2; ++k)
        {
            for (int j = y1; j <= y2; ++j)
            {
                for (int i = x1; i <= x2; ++i)
                {
                    sSetTile(i, j, k, n, map);
                    uint16_t underValue = sGetTile(i, j, z1 - 1, map);
                    if((underValue != n) & (underValue != 0))
                    {
                        assert(n);

                        supports[supportCount++] = underValue;
                    }
                }
            }
        }
        uint16_t value = supports[0];
        bool isOnlyOne = true;
        for(int i = 1; i < supportCount; ++i)
        {
            if(supports[i] != value)
            {
                isOnlyOne = false;
                break;
            }
        }
        if(isOnlyOne)
        {
            onlyOneSupport[value] = 1;
        }
    }

    int64_t removalCount = 0;
    {
        //TIMEDSCOPE("22A Summing");

        for (int counter = 0; counter < numberCount; ++counter)
        {
            if (onlyOneSupport[counter] == 0)
                ++removalCount;
        }
    }
    return removalCount;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("22B Total");
    int16_t map[RowSize * ZSize] = {};
    Block blocks[MaxBlocks] = {};
    int16_t numberCount = 1;
    sReadMapAndSort(data, blocks, numberCount);

    static const int BlocksMaxSupport = 4;
    alignas(16) int16_t supportedByBlocks[MaxBlocks * BlocksMaxSupport] = {};
    uint8_t supportedByBlocksCounts[MaxBlocks] = {};
    uint8_t onlyOneSupport[MaxBlocks] = {};

    sDropPiece(blocks, numberCount);

    std::sort(blocks + 1, blocks + numberCount, [](const Block &a, const Block &b) {
        return a.z1 < b.z1;
    });

    for (int n = 1; n < numberCount; ++n)
    {
        Block &b = blocks[n];

        uint16_t z1 = b.z1;
        uint16_t z2 = b.z2;
        uint8_t x1 = b.x1;
        uint8_t x2 = b.x2;
        uint8_t y1 = b.y1;
        uint8_t y2 = b.y2;

        int16_t supports[16] = {};
        uint8_t supportCount = 0;

        for(int k = z1; k <= z2; ++k)
        {
            for (int j = y1; j <= y2; ++j)
            {
                for (int i = x1; i <= x2; ++i)
                {
                    sSetTile(i, j, k, n, map);
                    int16_t underValue = sGetTile(i, j, k - 1, map);
                    if((underValue != n) & (underValue != 0))
                    {
                        assert(n);
                        supports[supportCount++] = underValue;
                    }
                }
            }
        }
        int supportedByBlocksCount = supportCount > 0 ? 1 : 0;
        // Find uniques
        if(supportCount > 0)
        {
            std::sort(supports, supports + supportCount, [](int16_t a, int16_t b){
                return a > b;
            });

            int16_t value = supports[0];
            supportedByBlocks[n * BlocksMaxSupport] = value;
            for (int i = 1; i < supportCount; ++i)
            {
                if (supports[i] != value)
                {
                    value = supports[i];
                    assert(supportedByBlocksCount + 1 < BlocksMaxSupport);
                    supportedByBlocks[n * BlocksMaxSupport + supportedByBlocksCount] = value;
                    supportedByBlocksCount++;
                }
            }

            // If only one support
            if (supportedByBlocksCount == 1)
            {
                int16_t value = supports[0];
                onlyOneSupport[value] = 0xff;
            }

        }
        supportedByBlocksCounts[n] = supportedByBlocksCount;
    }
    int64_t fallingBlockCount = 0;


    //uint8_t drops[MaxBlocks] = {};
    {
        TIMEDSCOPE("22B Counting");

        for (int16_t i = 1; i < numberCount; ++i)
        {
            if (onlyOneSupport[i] == 0)
                continue;
            uint8_t drops[MaxBlocks] = {};
            drops[i] = 0xff;

            for (int16_t j = i + 1; j < numberCount; j++)
            {
                // Check if index j has already dropped or if this is root block. Since we remove root block
                // in the beginning, these blocks wont be affected.
                if (drops[j] != 0 || supportedByBlocksCounts[j] == 0)
                    continue;
                bool isValid = true;
                for (int16_t k = 0; k < supportedByBlocksCounts[j]; k++)
                {
                    int16_t index = supportedByBlocks[j * BlocksMaxSupport + k];
                    isValid &= drops[index] != 0;
                }
                if (isValid)
                {
                    drops[j] = 0xff;
                    fallingBlockCount++;
                }
            }
        }
    }
    return fallingBlockCount;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "22A: Removable blocks: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "22B: Falling blocks: %" PRIi64, value);
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

int run22A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data22A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run22B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data22A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

