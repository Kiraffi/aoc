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

static void sMove(int& x, int& y, char dir, int amount, char* map)
{
    int xMove = 0;
    int yMove = 0;
    switch(dir)
    {
        case 'R': xMove += amount; break;
        case 'L': xMove -= amount; break;
        case 'U': yMove -= amount; break;
        case 'D': yMove += amount; break;
    }
    while(xMove != 0 || yMove != 0)
    {
        if(xMove > 0)
        {
            x++;
            xMove--;
        }
        else if(xMove < 0)
        {
            x--;
            xMove++;
        }
        if(yMove > 0)
        {
            y++;
            yMove--;
        }
        else if(yMove < 0)
        {
            y--;
            yMove++;
        }
        assert(x < MapWidth && x >= 0 && y < MapHeight && y >= 0);
        map[x + y * MapWidth] = '#';
    }
}



static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("18A Total");
    char map[MapWidth * MapHeight] = {};
    int x = 256;
    int y = 128;
    int xMin = x;
    int xMax = x;
    int yMin = y;
    int yMax = y;
    while(*data)
    {
        char dir = *data++;
        int amount = sParserNumber(0, &data);
        sMove(x, y, dir, amount, map);
        xMin = sMin(x, xMin);
        xMax = sMax(x, xMax);
        yMin = sMin(y, yMin);
        yMax = sMax(y, yMax);

        while (*data++ != '\n');
    }
    /*
    for(int i = 0; i < MapWidth * MapHeight; ++i )
    {
        if(map[i] == '#')
            printf("#");
        else
        {
            printf(" ");
        }
        if((i % MapWidth) ==MapWidth - 1)
        {
            printf("\n");
        }
    }*/
    printf("x min: %i, max: %i, y min: %i, max: %i\n", xMin, xMax, yMin, yMax);
    int64_t area = 0;
    bool isInside = false;
    for(int yp = 0; yp < MapHeight; ++yp)
    {
        for(int xp = 0; xp < MapWidth; ++xp)
        {
            bool wasUp = false;
            bool wasDown = false;
            while(map[xp + yp * MapWidth] == '#')
            {
                area++;
                wasUp   |= map[xp + (yp - 1) * MapWidth];
                wasDown |= map[xp + (yp + 1) * MapWidth];
                xp++;
            }
            if(wasUp && wasDown)
            {
                isInside = !isInside;
            }
            if(isInside)
            {
                area++;
            }
        }
    }

    return area;
}

struct Point
{
    int x;
    int y;
};

struct FindNext
{
    int64_t area;
    int newPointIndex;
};

static FindNext sFindNext(const Point* points, int pointCount, int pIndex, int dir)
{
    int currY = points[pIndex].y;
    int currX = points[pIndex].x;
    int yMin = currY;
    int xMin = currX;
    int topLeftPoint = pIndex;
    int nextIndex = (pIndex + dir + pointCount) % pointCount;
    int64_t area = 0;
    bool hasGoneDown = false;
    int prevY = currY;

    while(points[nextIndex].y < currY)
    {
        int x = points[nextIndex].x;
        int y = points[nextIndex].y;

        if(prevY > y && hasGoneDown)
        {
            nextIndex = (nextIndex - dir + pointCount) % pointCount;
            break;
        }
        if(y > prevY)
        {
            hasGoneDown = true;
        }
        if(y < yMin || (y == yMin && x < xMin))
        {
            yMin = y;
            xMin = x;
            topLeftPoint = nextIndex;
        }
        prevY = y;
        nextIndex = (nextIndex + dir + pointCount) % pointCount;
    }
    int lp = topLeftPoint;
    {
        const Point& p1 = points[lp];
        const Point& p2 = points[(lp + dir + pointCount) % pointCount];
        int x = p1.x;
        if(p2.x != x)
            dir = points[(lp + dir + pointCount) % pointCount].x > x ? dir : -dir;
        else
            dir = -dir;
    }
    int rp = (lp + dir + pointCount) % pointCount;
    Point lPoint = points[lp];
    Point rPoint = points[rp];

    while(rp != lp && ((lp != nextIndex && lp != pIndex) || (rp != nextIndex && rp != pIndex)))
    {
        assert(lPoint.x < rPoint.x);
        Point lPoint1 = points[(lp - dir + pointCount) % pointCount];
        Point rPoint1 = points[(rp + dir + pointCount) % pointCount];
        assert(lPoint1.x < rPoint1.x);

        lPoint1.y = sMin(lPoint1.y, currY);
        rPoint1.y = sMin(rPoint1.y, currY);

        if(lPoint.x != lPoint1.x)
        {
            lPoint = lPoint1;
            if(lp != pIndex && lp != nextIndex)
                lp = (lp - dir + pointCount) % pointCount;
        }
        else if(rPoint.x != rPoint1.x)
        {
            rPoint = rPoint1;
            if(rp != pIndex && rp != nextIndex)
                rp = (rp + dir + pointCount) % pointCount;
        }
        else if(lPoint.y > lPoint1.y)
        {
            int curr = lp;
            while(points[curr].x != 0) curr += dir;
            int64_t multiplier = points[curr].x < 0 ? 1 : -1;
            FindNext next = sFindNext(points, pointCount, lp, -dir);
            area += multiplier * next.area;
            lp = next.newPointIndex;
        }
        else if(rPoint.y > rPoint1.y)
        {
            int curr = rp;
            while(points[curr].x != 0) curr -= dir;
            int64_t multiplier = points[curr].x > 0 ? 1 : -1;
            FindNext next = sFindNext(points, pointCount, rp, +dir);
            area += multiplier * next.area;
            rp = next.newPointIndex;
        }

        else if(lPoint1.y < rPoint1.y)
        {
            assert(lPoint.x == lPoint1.x);
            int diffY = lPoint1.y - lPoint.y;
            if(lp == pIndex || lp == nextIndex)
            {
                diffY = currY - lPoint.y;
                area += diffY * (rPoint.x - lPoint.x);
                lPoint.y = currY;
                //rPoint.y = currY;
            }
            else
            {
                area += diffY * (rPoint.x - lPoint.x);
                lPoint = lPoint1;
                //rPoint.y = lPoint1.y;
                lp = (lp - dir + pointCount) % pointCount;
            }
        }
        else if(lPoint1.y > rPoint1.y)
        {
            assert(rPoint.x == rPoint1.x);
            int diffY = rPoint1.y - rPoint.y;
            if(rp == pIndex || rp == nextIndex)
            {
                diffY = currY - rPoint.y;
                area += diffY * (rPoint.x - lPoint.x);
                rPoint.y = currY;
                //lPoint.y = currY;
            }
            else
            {
                area += diffY * (rPoint.x - lPoint.x);
                rPoint = rPoint1;
                //lPoint.y = rPoint.y;
                rp = (rp + dir + pointCount) % pointCount;
            }

        }
        else
        {
            if(rp != nextIndex && rp != pIndex)
            {
                rp = (rp + dir + pointCount) % pointCount;
            }
            else if(lp != nextIndex && lp != pIndex)
            {
                lp = (lp - dir + pointCount) % pointCount;
            }
            else
            {
                assert(false);
            }
        }
    }
    return {.area = area, .newPointIndex = nextIndex };
}


static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("18B Total");

    int x = 0;
    int y = 0;
    //int xMin = x;
    int xMax = x;
    //int yMin = y;
    int yMax = y;

    Point points[1024] = {};

    int pointCount = 1;
    //int topLeftPoint = 0;
    int botRightPoint = 0;
    char lastDir = '\0';
    while(*data)
    {
        char dir = *data++;
        int amount = sParserNumber(0, &data);
        switch(dir)
        {
            case 'R': x += amount; break;
            case 'L': x -= amount; break;
            case 'U': y -= amount; break;
            case 'D': y += amount; break;
        }
        if(lastDir == dir)
            --pointCount;
        points[pointCount++] = {.x = x, .y = y};
        lastDir = dir;
        //if(x <= xMin && y <= yMin)
        //{
        //    yMin = y;
        //    xMin = x;
        //    topLeftPoint = pointCount - 1;
        //}
        if(y > yMax || (y == yMax && x > xMax))
        {
            yMax = y;
            xMax = x;
            botRightPoint = pointCount - 1;
        }
        while (*data++ != '\n');
    }
    assert(points[0].x == points[pointCount - 1].x && points[0].y == points[pointCount - 1].y);
    --pointCount;
    //int lp = topLeftPoint;
    //int dir = points[(lp + 1) % pointCount].x > xMin ? 1 : -1;
    //int64_t area = sFindNext(points, pointCount, topLeftPoint, dir).area;
    int lp = botRightPoint;
    int dir = points[(lp + 1) % pointCount].y < yMax ? 1 : -1;
    int64_t area = sFindNext(points, pointCount, botRightPoint, dir).area;
    /*
    int rp = (lp + dir + pointCount) % pointCount;
    int64_t width = points[rp].x - points[lp].x;
    while(lp != rp)
    {
        const Point& lPoint0 = points[lp];
        const Point& rPoint0 = points[rp];

        const Point& lPoint1 = points[(lp - dir + pointCount) % pointCount];
        const Point& rPoint1 = points[(rp + dir + pointCount) % pointCount];

        if(lPoint1.y < lPoint0.y)
        {
            int newLp =
        }
    }
*/
    /*
    for(int i = 0; i < MapWidth * MapHeight; ++i )
    {
        if(map[i] == '#')
            printf("#");
        else
        {
            printf(" ");
        }
        if((i % MapWidth) ==MapWidth - 1)
        {
            printf("\n");
        }
    }*/
    /*
    printf("x min: %i, max: %i, y min: %i, max: %i\n", xMin, xMax, yMin, yMax);
    int64_t area = 0;
    bool isInside = false;
    for(int yp = 0; yp < MapHeight; ++yp)
    {
        for(int xp = 0; xp < MapWidth; ++xp)
        {
            bool wasUp = false;
            bool wasDown = false;
            while(map[xp + yp * MapWidth] == '#')
            {
                area++;
                wasUp   |= map[xp + (yp - 1) * MapWidth];
                wasDown |= map[xp + (yp + 1) * MapWidth];
                xp++;
            }
            if(wasUp && wasDown)
            {
                isInside = !isInside;
            }
            if(isInside)
            {
                area++;
            }
        }
    }
*/

    return area;
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

