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

#define PROFILE 0
#include "../profile.h"

alignas(32) static constexpr char test24A[] =
    R"(19, 13, 30 @ -2,  1, -2
18, 19, 22 @ -1, -1, -2
20, 25, 34 @ -2, -2, -4
12, 31, 28 @ -1, -2, -1
20, 19, 15 @  1, -5, -3
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

struct P3
{
    int64_t x;
    int64_t y;
    int64_t z;
    int64_t padding;
};

struct P2T
{
    double x;
    double y;
    double t;
    double u;
};


bool sGetIntersection(const P3& p1, const P3& v1, const P3& p2, const P3& v2, P2T& out)
{
    int64_t dx = p2.x - p1.x;
    int64_t dy = p2.y - p1.y;

    int64_t det = v2.x * v1.y - v2.y * v1.x;
    if(det == 0)
    {
        return false;
    }
    double u = (dy * v2.x - dx * v2.y) / (double)det;
    double v = (dy * v1.x - dx * v1.y) / (double)det;
    // Don't cross in the past
    if(u < 0.0 || v < 0.0)
    {
        return false;
    }
    double p1x = p1.x + v1.x * u;
    double p1y = p1.y + v1.y * u;

    out.x = p1x;
    out.y = p1y;
    out.t = u;
    out.u = v;

    return true;
}

static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("24A Total");
    std::vector<P3> points;
    std::vector<P3> velocities;

    static constexpr int64_t MinValue = 200000000000000;
    static constexpr int64_t MaxValue = 400000000000000;

    //static constexpr int64_t MinValue = 7;
    //static constexpr int64_t MaxValue = 27;

    while(*data)
    {
        P3 p{};
        p.x = sParserNumber(0, &data);
        ++data;
        p.y = sParserNumber(0, &data);
        ++data;
        p.z = sParserNumber(0, &data);
        ++data;
        points.push_back(p);

        p.x = sParserNumber(0, &data);
        ++data;
        p.y = sParserNumber(0, &data);
        ++data;
        p.z = sParserNumber(0, &data);
        ++data;
        velocities.push_back(p);

    }
    int64_t intersectPoints = 0;
    for(int j = 0; j < int(points.size()); ++j)
    {
        for(int i = j + 1; i < int(points.size()); ++i)
        {
            P2T result {};
            if(sGetIntersection(points[j], velocities[j], points[i], velocities[i], result))
            {
                if (result.x >= MinValue && result.x <= MaxValue
                    && result.y >= MinValue && result.y <= MaxValue)
                {
                    intersectPoints++;
                }
            }
        }
    }

    return intersectPoints;
}

static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("24B Total");

    std::vector<P3> points;
    std::vector<P3> velocities;

    while(*data)
    {
        P3 p{};
        p.x = sParserNumber(0, &data);
        ++data;
        p.y = sParserNumber(0, &data);
        ++data;
        p.z = sParserNumber(0, &data);
        ++data;
        points.push_back(p);

        p.x = sParserNumber(0, &data);
        ++data;
        p.y = sParserNumber(0, &data);
        ++data;
        p.z = sParserNumber(0, &data);
        ++data;
        velocities.push_back(p);
    }

    P3 finalResult = {};

    for(int x = -1000; x <= 1000; ++x)
    {
        for(int y = -1000; y <= 1000; ++y)
        {
            P3 v0 = velocities[0];
            v0.x += x;
            v0.y += y;

            P3 v1 = velocities[1];
            v1.x += x;
            v1.y += y;

            P2T result[4]{};

            if (sGetIntersection(points[0], v0, points[1], v1, result[0]))
            {
                bool valid = true;
                for(int i = 2; i < 4; ++i)
                {
                    P3 v2 = velocities[i];
                    v2.x += x;
                    v2.y += y;
                    if (sGetIntersection(points[0], v0, points[i], v2, result[i]))
                    {
                        double dx = abs(result[i].x - result[0].x);
                        double dy = abs(result[i].y - result[0].y);
                        if(dx >= 1e-1 || dy >= 1e-1)
                        {
                            valid = false;
                            break;
                        }
                    }
                    else
                    {
                        valid = false;
                    }
                }
                if(valid)
                {
                    result[1] = result[0];
                    for(int z = -1000; z <= 1000; ++z)
                    {
                        valid = true;
                        double zz = points[0].z + (velocities[0].z + z) * result[0].t;
                        for(int i = 1; i < 4; ++i)
                        {
                            double zz1 = points[i].z + (velocities[i].z + z) * result[i].u;
                            double diff = abs(zz - zz1);
                            if(diff >= 1e-1)
                            {
                                valid = false;
                                break;
                            }

                        }
                        if(valid)
                        {
                            finalResult.x = result[0].x;
                            finalResult.y = result[0].y;
                            finalResult.z = zz;
                        }
                    }
                }
            }
        }
    }

    return finalResult.x + finalResult.y + finalResult.z;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "24A: Result: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "24B: Result: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data24A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data24A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run24A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data24A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run24B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data24A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

