
// this is meant for included file for both compute and c++

#ifdef __cplusplus

#include <cmath>
#include <cstdint>
using uint = uint32_t;
#define max std::max
struct uvec2
{
    uvec2(uint32_t xx) : x(xx), y(xx) {}
    uvec2(uint32_t xx, uint32_t yy) : x(xx), y(yy) {}
    uint32_t x;
    uint32_t y;
};

struct uvec4
{
    uvec4(uint32_t aa) : a(aa), b(aa) {}
    uvec4(uvec2 aa, uvec2 bb) : a(aa), b(bb) {}
    uvec4(uint32_t aa, uint32_t bb, uint32_t cc, uint32_t dd) : x(aa), y(bb), z(cc), w(dd) {}
    union
    {
        struct
        {
            uvec2 a;
            uvec2 b;
        };
        struct
        {
            uvec2 xy;
            uvec2 zw;
        };
        struct
        {
            uint32_t x;
            uint32_t y;
            uint32_t z;
            uint32_t w;
        };
    };
};

uvec2 operator+(uvec2 a, uvec2 b)
{
    return uvec2(a.x + b.x, a.y + b.y);
}
uvec2 operator-(uvec2 a, uvec2 b)
{
    return uvec2(a.x - b.x, a.y - b.y);
}

uvec4 operator+(uvec4 a, uvec4 b)
{
    return uvec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
uvec4 operator-(uvec4 a, uvec4 b)
{
    return uvec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static uint64_t get64(uvec2 a)
{
    return *((uint64_t*)&a);
}
static uvec2 getuvec2(uint64_t a)
{
    return *((uvec2*)&a);
}

#endif

bool l64(uvec2 a, uvec2 b)
{
    if(a.y != b.y)
    {
        return a.y < b.y;
    }
    return a.x < b.x;
}

bool le64(uvec2 a, uvec2 b)
{
    if(a.y != b.y)
    {
        return a.y < b.y;
    }
    return a.x <= b.x;
}


bool g64(uvec2 a, uvec2 b)
{
    if(a.y != b.y)
    {
        return a.y > b.y;
    }
    return a.x > b.x;
}

bool ge64(uvec2 a, uvec2 b)
{
    if(a.y != b.y)
    {
        return a.y > b.y;
    }
    return a.x >= b.x;
}

bool eq64(uvec2 a, uvec2 b)
{
    return a.x == b.x && a.y == b.y;
}


uvec2 shiftR64(uvec2 a, uint bits)
{
    uvec2 result = a;
    uint rBits = uint(32) - bits;
    uint carry = a.y << rBits;
    result.x = (result.x >> bits) | carry;
    result.y = (result.y >> bits);
    return result;
}


uvec2 add64(uvec2 a, uvec2 b)
{
    uvec2 result = a + b;
    if(result.x < max(a.x, b.x))
    {
        result.y++;
    }
    return result;
}

// probably doesnt handle negative values
uvec2 sub64(uvec2 a, uvec2 b)
{
    uvec2 result = a - b;
    if(result.x > a.x)
    {
        result.y--;
    }
    return result;
}

uvec2 mul64(uvec2 a, uvec2 b)
{
    uvec2 result = uvec2(0);

    uint lowA = (a.x & uint(0xffff));
    uint lowB = (b.x & uint(0xffff));
    uint highA = a.x >> uint(16);
    uint highB = b.x >> uint(16);


    result.x = lowA * lowB;
    result.y = highA * highB;
    result.y += a.x * b.y;
    result.y += a.y * b.x;

    uint midA = lowA * highB;
    uint midB = highA * lowB;

    result = add64(result, uvec2(midA << 16, midA >> 16));
    result = add64(result, uvec2(midB << 16, midB >> 16));

    return result;
}


uvec4 add128(uvec4 a, uvec4 b)
{
    uvec4 result = a + b;
    if(result.x < max(a.x, b.x))
    {
        result.y++;
    }
    if(result.y < max(a.y, b.y))
    {
        result.z++;
    }
    if(result.z < max(a.z, b.z))
    {
        result.w++;
    }
    return result;
}


uvec4 mul128(uvec2 a, uvec2 b)
{
    uvec4 result = uvec4(0);

    uvec2 lowA =  uvec2(a.x, 0);
    uvec2 lowB =  uvec2(b.x, 0);
    uvec2 highA = uvec2(a.y, 0);
    uvec2 highB = uvec2(b.y, 0);

    uvec2 midA = mul64(lowA, highB);
    uvec2 midB = mul64(highA, lowB);

    result.xy = mul64(lowA, lowB);
    result.zw = mul64(highA, highB);

    result = add128(result, uvec4(uvec2(0, midA.x), uvec2(midA.y, 0)));
    result = add128(result, uvec4(uvec2(0, midB.x), uvec2(midB.y, 0)));

    return result;
}



#if 0
uvec2 div64(uvec2 a, uvec2 b)
{
    uvec2 result = uvec2(0);
    uvec2 minValue = uvec2(0);
    uvec2 maxValue = a;

    while(l64(add64(minValue, uvec2(1, 0)), maxValue))
    {
        uint lr = ((minValue.x & 1) + (maxValue.x & 1)) >> 1;
        uvec2 mid = add64(shiftR64(minValue, 1), shiftR64(maxValue, 1));
        mid = add64(mid, uvec2(lr, 0));

        uvec4 t1 = mul128(mid, b);
        // if it overflows any way.
        if((mid.y > 0 && b.y > 0)
            || t1.z > 0
            || t1.w > 0
            || g64(t1.xy, a))
        {
            maxValue = mid;
        }
        else
        {
            minValue = mid;
        }
    }
    return minValue;
}

#else

uvec2 div64(uvec2 a, uvec2 b)
{
    uvec2 result = uvec2(0);
    uvec2 minValue = uvec2(0);
    uvec2 maxValue = a;

    while(l64(add64(minValue, uvec2(1, 0)), maxValue))
    {
        uvec2 mid = shiftR64(add64(minValue, maxValue), 1);
        uvec2 t1 = mul64(mid, b);

        // if it overflows any way. Doing 128bit mul to check for carry bit
        uvec2 t2 = mul64(uvec2(mid.y, 0), uvec2(b.x, 0));
        uvec2 t3 = mul64(uvec2(b.y, 0), uvec2(mid.x, 0));
        uvec2 t4 = add64(uvec2(t1.y, 0), add64(t2, t3));

        if((mid.y > 0 && b.y > 0)
            || t4.y > 0
            || g64(t1, a))
        {
            maxValue = mid;
        }
        else
        {
            minValue = mid;
        }
    }
    return minValue;
}

#endif
