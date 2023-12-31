#include <algorithm> // std::max
#include <assert.h> // assert
#include <immintrin.h> // SIMD
#include <inttypes.h> // PRI64
#include <stdio.h> // printf

#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>


#include "input.cpp"

#define PROFILE 0
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
    alignas(16) uint64_t hitTest[ZSize * 2] = {};

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

        alignas(16) uint64_t bHit[2] = {};
        for (int j = y1; j <= y2; ++j)
        {
            uint64_t newBits1 = (uint64_t(1) << (x2 + 1)) - 1;
            uint64_t newBits2 = (uint64_t(1) << (x1 + 0)) - 1;
            uint64_t newBits = newBits1 - newBits2;

            if(j % 10 < 6)
            {
                newBits1 = newBits << ((j % 10) * 10);
                newBits2 = 0;
            }
            else
            {
                newBits1 = 0;
                newBits2 = newBits << (((j % 10) - 6) * 10);
            }
            bHit[0] |= newBits1;
            bHit[1] |= newBits2;
        }
        __m128i hitTestValue = _mm_loadu_si128((const __m128i*)bHit);
        __m256i hitTestValue256 = _mm256_set_m128i(hitTestValue, hitTestValue);

        bool hit = false;

        z1 -= 1;
        z2 -= 1;

        while(z1 >= 1)
        {
            __m256i value256 = _mm256_loadu_si256((const __m256i *) (hitTest + z1 * 2));
            hit = !_mm256_testz_si256(value256, hitTestValue256);
            if(hit || z1 == 1)
                break;
            z1 -= 2;
            z2 -= 2;
        }
        if(hit || z1 == 0)
        {
            z1++;
            z2++;

            __m128i value = _mm_loadu_si128((const __m128i *) (hitTest + z1 * 2));
            if (!_mm_testz_si128(value, hitTestValue))
            {
                z1++;
                z2++;
            }
        }
        for(int k = z1; k <= z2; ++k)
        {
            hitTest[k * 2 + 0] |= bHit[0];
            hitTest[k * 2 + 1] |= bHit[1];
        }
    }
}
static constexpr int MaxSupportCount = 16;
static int sSetBlock(const Block& b, int16_t blockIndex, int16_t* map, int16_t* supports)
{
    int supportCount = 0;

    for (int j = b.y1; j <= b.y2; ++j)
    {
        for (int i = b.x1; i <= b.x2; ++i)
        {
            int16_t underValue = sGetTile(i, j, b.z1 - 1, map);
            if((underValue != blockIndex) & (underValue != 0))
            {
                assert(blockIndex);
                assert(supportCount < MaxSupportCount);
                supports[supportCount++] = underValue;
            }
        }
    }

    for(int k = b.z1; k <= b.z2; ++k)
    {
        for (int j = b.y1; j <= b.y2; ++j)
        {
            for (int i = b.x1; i <= b.x2; ++i)
            {
                sSetTile(i, j, k, blockIndex, map);
            }
        }
    }
    return supportCount;
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

    for(int16_t blockIndex = 1; blockIndex < numberCount; ++blockIndex)
    {
        const Block& b = blocks[blockIndex];
        int16_t supports[MaxSupportCount] = {};

        int supportCount = sSetBlock(b, blockIndex, map, supports);
        uint16_t value = supports[0];

        bool isOnlyOne = true;
        for(int i = 0; i < supportCount; ++i)
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

    sDropPiece(blocks, numberCount);
    // Sort tiles again after dropping them.
    std::sort(blocks + 1, blocks + numberCount, [](const Block &a, const Block &b) {
        return a.z1 < b.z1;
    });


    static const int BlocksMaxSupport = 4;
    alignas(16) int16_t supportedByBlocks[MaxBlocks * BlocksMaxSupport] = {};
    uint8_t supportedByBlocksCounts[MaxBlocks] = {};
    uint8_t onlyOneSupport[MaxBlocks] = {};

    for (int16_t blockIndex = 1; blockIndex < numberCount; ++blockIndex)
    {
        const Block& b = blocks[blockIndex];
        int16_t supports[MaxSupportCount] = {};

        int supportCount = sSetBlock(b, blockIndex, map, supports);

        int supportedByBlocksCount = supportCount > 0 ? 1 : 0;
        // Find uniques
        if(supportCount > 0)
        {
            // Sort them supports to check uniques easier
            std::sort(supports, supports + supportCount, [](int16_t a, int16_t b){
                return a < b;
            });

            int16_t value = supports[0];
            supportedByBlocks[blockIndex * BlocksMaxSupport] = value;
            for (int i = 1; i < supportCount; ++i)
            {
                if (supports[i] != value)
                {
                    value = supports[i];
                    assert(supportedByBlocksCount + 1 < BlocksMaxSupport);
                    supportedByBlocks[blockIndex * BlocksMaxSupport + supportedByBlocksCount] = value;
                    supportedByBlocksCount++;
                }
            }

            // If only one support
            if (supportedByBlocksCount == 1)
            {
                onlyOneSupport[value] = 0xff;
            }

        }
        supportedByBlocksCounts[blockIndex] = supportedByBlocksCount;
    }

    int64_t fallingBlockCount = 0;
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
                // in the beginning, these blocks will not be affected.
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

