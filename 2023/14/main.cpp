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

#define PROFILE 1
#include "../profile.h"

alignas(32) static constexpr char test14A[] =
    R"(O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....
)";

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("14A Total");
    int64_t sum = 0;

    uint8_t freeSlots[128] = {};
    int rockCount = 0;
    int8_t x = 0;
    int8_t y = 0;
    while(*data)
    {
        switch(*data)
        {
            case '\n':
                x = -1;
                ++y;
                break;
            case 'O':
                rockCount++;
                sum += freeSlots[x];
                freeSlots[x]++;
                break;
            case '#':
                freeSlots[x] = y + 1;
        }
        ++x;
        ++data;
    }
    sum = y * rockCount - sum;
    return sum;
}

void bitShiftRightOne(__m128i* value)
{
    __m128i movedTop = _mm_bsrli_si128(*value, 8);
    movedTop = _mm_slli_epi64(movedTop, 63);
    *value = _mm_srli_epi64(*value, 1);
    *value = _mm_or_si128(*value, movedTop);
}

void bitShiftLeftOne(__m128i* value)
{
    __m128i movedBot = _mm_bslli_si128(*value, 8);
    movedBot = _mm_srli_epi64(movedBot, 63);
    *value = _mm_slli_epi64(*value, 1);
    *value = _mm_or_si128(*value, movedBot);
}

void sDrawMap14(__m128i* map, int height, char printChar)
{
    for(int j = 0; j < height + 2; ++j)
    {
        __m128i bitShift = _mm_set_epi32(0, 0, 0, 1);
        for(int i = 0; i < 128; ++i)
        {
            if(!_mm_test_all_ones(~(map[j] & bitShift)))
            {
                printf("%c", printChar);
            }
            else
            {
                printf(".");
            }
            bitShift += bitShift;
            if(_mm_test_all_ones(~bitShift))
                bitShift = _mm_set_epi32(0, 1, 0, 0);
        }
        printf("\n");
    }
    printf("\n");
}

void sDrawMaps14(__m128i* wallMap, __m128i* rockMap, int height)
{
    for(int j = 0; j < height + 2; ++j)
    {
        __m128i bitShift = _mm_set_epi32(0, 0, 0, 1);
        for(int i = 0; i < 128; ++i)
        {
            assert(_mm_test_all_ones(~((rockMap[j] & wallMap[j]) & bitShift)));
            if(!_mm_test_all_ones(~(rockMap[j] & bitShift)))
            {
                printf("O");
            }
            else if(!_mm_test_all_ones(~(wallMap[j] & bitShift)))
            {
                printf("#");
            }
            else
            {
                printf(".");
            }
            bitShift += bitShift;
            if(_mm_test_all_ones(~bitShift))
                bitShift = _mm_set_epi32(0, 1, 0, 0);

        }
        printf("\n");
    }
    printf("\n");
}

static bool sMoveRocksUpDown(const __m128i* __restrict__ wallMap,
    __m128i* rockMap,
    int startIndex,
    //int endIndex,
    int moveDir)
{

    __m128i changed = _mm_setzero_si128();
    __m128i row = rockMap[startIndex];
    rockMap[startIndex] = _mm_setzero_si128();
    int j = startIndex + moveDir;
    //__m128i prevRock = rockMap[j - moveDir];
    while(!_mm_test_all_ones(~row))
    //while(!_mm_test_all_ones(~row) && j > 0 && j < 128)
    //while(j - moveDir != endIndex)
    //while(j > 0 && j < 128)
    {
        __m128i rocks = rockMap[j];
        __m128i colliders = _mm_or_si128(rocks, wallMap[j]);
        __m128i collided = _mm_and_si128(row, colliders);
        rockMap[j - moveDir] = _mm_or_si128(collided, rockMap[j - moveDir]);
        row = _mm_xor_si128(row, collided);
        changed = _mm_or_si128(changed, row);
        //row = _mm_or_si128(row, rocks);
        j += moveDir;
    }
    return !_mm_test_all_ones(~changed);
}
template<int moveDir>
constexpr static void sBitShift(__m128i* value) //, int dir)
{

    if(moveDir == -1)
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        //*value >>= 1;
        bitShiftRightOne(value);
    }
//    else if(dir == 1)
    else
    {
        // this doesnt work on simd, you have to manually do bit shifting
        // over 64 bit boundaries
        //*value <<= 1;
        bitShiftLeftOne(value);
    }
}
template<int moveDir>
static bool sMoveRocksLeftRight(const __m128i* __restrict__ wallMap,
    __m128i* rockMap,
    int rowIndex)
{
    __m128i row1 = rockMap[rowIndex];
    __m128i collisions1 = wallMap[rowIndex];
    __m128i origColl1 = collisions1;
/*
    __m128i row2 = rockMap[rowIndex + 1];
    __m128i collisions2 = wallMap[rowIndex + 1];
    __m128i origColl2 = collisions2;
*/
    sBitShift<-moveDir>(&collisions1);
//    sBitShift<-moveDir>(&collisions2);

    //sBitShift(&collisions1, -moveDir);
    //sBitShift(&collisions2, -moveDir);

    __m128i newRow1 = _mm_setzero_si128();
    //__m128i newRow2 = _mm_setzero_si128();

    while(!_mm_testz_si128(row1, row1))// || !_mm_testz_si128(row2, row2))
    {
        __m128i collided1 = _mm_and_si128(collisions1, row1);
//        __m128i collided2 = _mm_and_si128(collisions2, row2);

        while (!_mm_testz_si128(collided1, row1))// || !_mm_testz_si128(collided2, row2))
        {

            for (int i = 0; i < 2; ++i)
            {
                collided1 = _mm_and_si128(collided1, row1);
                //newRow1 = _mm_or_si128(collided1, newRow1);
                //sBitShift<-moveDir>(&collided1, -moveDir);
                sBitShift<-moveDir>(&collided1);
                collisions1 = _mm_or_si128(collided1, collisions1);
/*
                collided2 = _mm_and_si128(collided2, row2);
                //newRow2 = _mm_or_si128(collided2, newRow2);
                //sBitShift(&collided2, -moveDir);
                sBitShift<-moveDir>(&collided2);
                collisions2 = _mm_or_si128(collided2, collisions2);
                */
            }
        }

        /*
        // Remove the added rocks from collided
        row1 = _mm_xor_si128(collided1, row1);
        row2 = _mm_xor_si128(collided2, row2);

        // Add new collided rocks into writable
        newRow1 = _mm_or_si128(newRow1, collided1);
        newRow2 = _mm_or_si128(newRow2, collided2);

        // Move the collision one in negative direction
        sBitShift(&collided1, -moveDir);
        sBitShift(&collided2, -moveDir);
        collisions1 = _mm_or_si128(collisions1, collided1);
        collisions2 = _mm_or_si128(collisions2, collided2);

        // Shift the rocks that would not collide with the moved collisions
        __m128i collRow1 = _mm_and_si128(row1, ~collided1);
        row1 = _mm_and_si128(row1, collided1);
        sBitShift(&collRow1, moveDir);
        row1 = _mm_or_si128(row1, collRow1);

        __m128i collRow2 = _mm_and_si128(row2, ~collided2);
        row2 = _mm_and_si128(row2, collided2);
        sBitShift(&collRow2, moveDir);
        row2 = _mm_or_si128(row2, collRow2);
*/
        newRow1 = _mm_or_si128(newRow1, _mm_and_si128(row1, collisions1));
        row1 = _mm_and_si128(~collisions1, row1);
        sBitShift<moveDir>(&row1);
        //sBitShift(&row1, moveDir);
/*
        newRow2 = _mm_or_si128(newRow2, _mm_and_si128(row2, collisions2));
        row2 = _mm_and_si128(~collisions2, row2);
        sBitShift<moveDir>(&row2);
*/

        //sBitShift(&collided2, -moveDir);
    }
    //sBitShift(&collisions1, moveDir);
    //sBitShift(&collisions2, moveDir);

    // The leftmost column changes otherwise when moving right.
    //collisions1 = _mm_or_si128(collisions1, _mm_set_epi32(0, 0, 0, 1));
    //collisions2 = _mm_or_si128(collisions2, _mm_set_epi32(0, 0, 0, 1));

    //rockMap[rowIndex + 0] = _mm_xor_si128(collisions1, wallMap[rowIndex + 0]);
    //rockMap[rowIndex + 1] = _mm_xor_si128(collisions2, wallMap[rowIndex + 1]);

    rockMap[rowIndex + 0] = newRow1;
    //rockMap[rowIndex + 1] = newRow2;

    return !_mm_testc_si128(collisions1, origColl1)
    //    || !_mm_testc_si128(collisions2, origColl2)
    ;
}
static int64_t sCountScore(const __m128i* __restrict__ rockMap, int height)
{
    int64_t sum = 0;
    const uint64_t* values = (const uint64_t*)(rockMap + 1);
    for(int i = 1; i < height + 2; ++i)
    {
        int multiplier = height - i + 1;
        sum += multiplier * std::popcount((uint64_t)values[0]);
        sum += multiplier * std::popcount((uint64_t)values[1]);
        values += 2;
    }
    return sum;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("14B Total");
    int64_t sum = 0;

    __m128i rockMap[128] = {};
    __m128i wallMap[128] = {};
    __m128i one = _mm_set_epi32(0, 0, 0, 1);;
    __m128i minusOne = _mm_set1_epi32(-1);

    int8_t width = 0;
    int8_t height = 0;

    int8_t x = 1;
    int8_t y = 0;
    __m128i bitShift = one << 2;
    while(*data)
    {
        [[__maybe_unused__]] __m128i old = rockMap[y + 1];
        switch(*data)
        {
            case '\n':
                width = x;
                y += 1;
                x = 0;
                bitShift = one << 1;
                break;
            case 'O':
                rockMap[y + 1] = _mm_or_si128(rockMap[y + 1], bitShift);
                assert(!_mm_test_all_ones(~(old ^ rockMap[y + 1])));
                break;
            case '#':
                wallMap[y + 1] = _mm_or_si128(wallMap[y + 1], bitShift);
                assert(!_mm_test_all_ones(~(old ^ wallMap[y + 1])));
                break;

        }
        x += 1;
        bitShift += bitShift;
        if(_mm_test_all_ones(~bitShift))
        {
            bitShift = _mm_set_epi32(0, 1, 0, 0);
        }
        assert(!_mm_test_all_ones(~bitShift));
        ++data;
    }
    height = y;
    wallMap[0] = minusOne;
    wallMap[height + 1] = minusOne;

    int moveAmount = width + 1;
    __m128i rightWall;
    if(moveAmount < 64)
    {
        rightWall = one << moveAmount;
    }
    else
    {
        rightWall = _mm_set_epi32(0, 1, 0, 0);
        rightWall <<= moveAmount - 64;
    }
    __m128i leftRightWall = rightWall | _mm_set_epi32(0, 0, 0, 3);
    for(__m128i& i : wallMap)
    {
        i |= leftRightWall;
    }

    //sDrawMap14(wallMap, height, '#');
    //sDrawMap14(rockMap, height, 'O');
    //sDrawMaps14(wallMap, rockMap, height);

    std::unordered_map<int64_t, int64_t> diffMap;

    static const int64_t LoopCounts = 1000000000;
    int64_t superLoop = 0;
    for(int64_t j = 0; j < LoopCounts; ++j)
    {
        ++superLoop;
        //TIMEDSCOPE("14 Loop");
        {
            //TIMEDSCOPE("rockupdown2");

            for (int i = 2; i <= height; ++i)
            {
                (sMoveRocksUpDown(wallMap, rockMap, i, -1));
                //while(sMoveRocksUpDown(wallMap, rockMap, i, -1));
                //while(sMoveRocksUpDown(wallMap, rockMap, height - 1, 1, -1));
            }
        }
        __m128i rowDiffs = _mm_setzero_si128();

        for(int i = 1; i <= height; ++i)
        {
            //previous = _mm_or_si128(previous, _mm_xor_si128(previousMap[i], rockMap[i]));
            rowDiffs = _mm_xor_si128(rowDiffs,rockMap[i]);
            //previousMap[i] = rockMap[i];
        }


        int64_t* values = (int64_t *) &rowDiffs;
        int64_t hashValue = values[0] ^ values[1];

        //int64_t score = sCountScore(rockMap, height);
        //hashValue ^= score;
        auto iter = diffMap.find(hashValue);
        if(iter != diffMap.end())
        {
            int64_t loop = j - iter->second;
            //printf("index: %i, loop: %i\n", int(j), int(loop));
            while(j < LoopCounts - loop)
                j += loop;
        }
        diffMap[hashValue] = j;

        if(_mm_test_all_ones(~rowDiffs))
        {
            break;
        }
        //sDrawMaps14(wallMap, rockMap, height);



/*
        int end = (height - 1) % 4;

        for(int i = 1; i <= end + 1; i += 4)
        {
            sMoveRocksLeftRight(wallMap, rockMap, i + 0, -1);
            sMoveRocksLeftRight(wallMap, rockMap, i + 1, -1);
            sMoveRocksLeftRight(wallMap, rockMap, i + 2, -1);
            sMoveRocksLeftRight(wallMap, rockMap, i + 3, -1);
        }
        for(int i = end + 1; i <= height + 1; i++)
        */
        {
            //TIMEDSCOPE("rockupleftright2");

            for (int i = 1; i <= height + 1; i += 1)
            {
                //sMoveRocksLeftRight(wallMap, rockMap, i + 0, -1);
                sMoveRocksLeftRight<-1>(wallMap, rockMap, i + 0);
            }
        }
        //sDrawMaps14(wallMap, rockMap, height);


//        while(sMoveRocksUpDown(wallMap, rockMap, 1, 1));
//        while(sMoveRocksUpDown(wallMap, rockMap, 1, height - 1, 1));

        {
            //TIMEDSCOPE("rockupdown");
            for (int i = height - 1; i > 0; --i)
            {
                sMoveRocksUpDown(wallMap, rockMap, i, 1);
            }
        }
        //sDrawMaps14(wallMap, rockMap, height);
/*
        for(int i = 1; i <= end + 1; i += 4)
        {
            sMoveRocksLeftRight(wallMap, rockMap, i + 0, 1);
            sMoveRocksLeftRight(wallMap, rockMap, i + 1, 1);
            sMoveRocksLeftRight(wallMap, rockMap, i + 2, 1);
            sMoveRocksLeftRight(wallMap, rockMap, i + 3, 1);
        }
        for(int i = end + 1; i <= height; ++i)
*/
        {
            //TIMEDSCOPE("rockupleftright");

            for (int i = 1; i <= height; i += 1)
            {
                sMoveRocksLeftRight<1>(wallMap, rockMap, i + 0);
            }
        }
        //sDrawMaps14(wallMap, rockMap, height);

    }
    //sDrawMap14(rockMap, height, 'O');
    //sDrawMaps14(wallMap, rockMap, height);
    printf("superloop: %" PRIi64 "\n", superLoop);
    sum = sCountScore(rockMap, height);

    return sum;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "14A: Sum of pressure: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "14B: Sum of pressure: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data14A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data14A));
    printf("%s\n", printBuffer);

    return 0;
}
#endif

int run14A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data14A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run14B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data14A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

