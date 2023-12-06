#include <assert.h> // assert
#include <bit> // std::count
#include <ctype.h> //isdigit
#include <immintrin.h> // SIMD
#include <stdint.h> // intptr
#include <stdio.h> // printf

#include "input.cpp"

static const char test01A[] = R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet
      )";
constexpr size_t test01ASize = sizeof(test01A);
static_assert((test01ASize % 16) == 0, "Need to be dividable by 16 for alignment");

static const char test02B[] = R"(two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen
  )";
constexpr size_t test02BSize = sizeof(test02B);
static_assert((test02BSize % 16) == 0, "Need to be dividable by 16 for alignment");


static uint64_t numbers[] = {
    *(uint64_t*)("zero\0\0\0\0"),
    *(uint64_t*)("one\0\0\0\0\0"),
    *(uint64_t*)("two\0\0\0\0\0"),
    *(uint64_t*)("three\0\0\0"),
    *(uint64_t*)("four\0\0\0\0"),
    *(uint64_t*)("five\0\0\0\0"),
    *(uint64_t*)("six\0\0\0\0\0"),
    *(uint64_t*)("seven\0\0\0"),
    *(uint64_t*)("eight\0\0\0"),
    *(uint64_t*)("nine\0\0\0\0")
};

static const uint64_t masks[] = {
    uint64_t(0xff'ff'ff'ff),
    uint64_t(0xff'ff'ff),
    uint64_t(0xff'ff'ff),
    uint64_t(0xff'ff'ff'ff'ff),
    uint64_t(0xff'ff'ff'ff),
    uint64_t(0xff'ff'ff'ff),
    uint64_t(0xff'ff'ff),
    uint64_t(0xff'ff'ff'ff'ff),
    uint64_t(0xff'ff'ff'ff'ff),
    uint64_t(0xff'ff'ff'ff),
};



static const char* sLineEnds[2000];
static int sLineNumbers = 0;


static void sFindLineBreaks(const char* str, int len)
{
    sLineNumbers = 0;
    assert((intptr_t(str) % 16) == 0);
    assert((len % 16) == 0);

    const __m128i* ptr = (__m128i *)(str);
    const __m128i* end = ptr + (len / 16);

    const __m128i findChar = _mm_set1_epi8('\n');
    const __m128i zero = _mm_set1_epi8(0);
    int pos = 0;
    while(ptr < end)
    {
        __m128i value = _mm_loadu_si128(ptr);
        __m128i found = _mm_cmpeq_epi8(value, findChar);
        uint32_t mask = _mm_movemask_epi8(found);
        int offset = 0;
        while(mask)
        {
            int zeros = std::countr_zero(mask);
            mask = mask >> zeros;
            offset += zeros;

            sLineEnds[sLineNumbers++] = str + offset + pos;

            mask >>= 1;
            offset++;
        }
        ptr++;
        pos += 16;
    }
}

static bool sTestNumber(const char* str, uint64_t comp, uint64_t mask)
{
    uint64_t value = *((const uint64_t*)str);
    uint64_t maskedValue = value & mask;
    return maskedValue == comp;
}

static int sTestNumbers(const char* str)
{
    for(int i = 1; i < 10; ++i)
    {
        if(sTestNumber(str, numbers[i], masks[i]))
            return i;
    }
    return 0;
}

static constexpr int32_t bitMaskValues[4] =
{
    0xff,
    0xffff,
    0xffffff,
    -1,
};

static __m128i bitMasks[16] =
{
    _mm_set_epi32(0x0, 0x0, 0x0, bitMaskValues[0]),
    _mm_set_epi32(0x0, 0x0, 0x0, bitMaskValues[1]),
    _mm_set_epi32(0x0, 0x0, 0x0, bitMaskValues[2]),
    _mm_set_epi32(0x0, 0x0, 0x0, bitMaskValues[3]),

    _mm_set_epi32(0x0, 0x0, bitMaskValues[0], bitMaskValues[3]),
    _mm_set_epi32(0x0, 0x0, bitMaskValues[1], bitMaskValues[3]),
    _mm_set_epi32(0x0, 0x0, bitMaskValues[2], bitMaskValues[3]),
    _mm_set_epi32(0x0, 0x0, bitMaskValues[3], bitMaskValues[3]),

    _mm_set_epi32(0x0, bitMaskValues[0], bitMaskValues[3], bitMaskValues[3]),
    _mm_set_epi32(0x0, bitMaskValues[1], bitMaskValues[3], bitMaskValues[3]),
    _mm_set_epi32(0x0, bitMaskValues[2], bitMaskValues[3], bitMaskValues[3]),
    _mm_set_epi32(0x0, bitMaskValues[3], bitMaskValues[3], bitMaskValues[3]),

    _mm_set_epi32(bitMaskValues[0], bitMaskValues[3], bitMaskValues[3], bitMaskValues[3]),
    _mm_set_epi32(bitMaskValues[1], bitMaskValues[3], bitMaskValues[3], bitMaskValues[3]),
    _mm_set_epi32(bitMaskValues[2], bitMaskValues[3], bitMaskValues[3], bitMaskValues[3]),
    _mm_set_epi32(bitMaskValues[3], bitMaskValues[3], bitMaskValues[3], bitMaskValues[3]),
};

static int sParse01A(const char* data)
{
    int sum = 0;
    __m128i zeroChar = _mm_set1_epi8('0');
    __m128i zero = _mm_set1_epi8('0');
    __m128i ten = _mm_set1_epi8('9' + 1);
    for(int i = 0; i < sLineNumbers; ++i)
    {
        int first = 0;
        int last = 0;
        const __m128i* ptr = (const __m128i *)(data);
        const __m128i* const end = (const __m128i * const)sLineEnds[i];
        intptr_t len = sLineEnds[i] - data;
        while(ptr < end)
        {
            __m128i value = _mm_loadu_si128(ptr);
            // mask characters that come after line break
            if(len < 16)
            {
                value = _mm_and_si128(value, bitMasks[len]);
            }

            // 0 < x < 10
            __m128i lt10 = _mm_cmplt_epi8(value, ten);
            __m128i gt0 = _mm_cmpgt_epi8(value, zero);

            // And the masks together to find numbers between 1 and 9.
            __m128i numbsMask = _mm_and_si128(lt10, gt0);

            uint32_t mask = _mm_movemask_epi8(numbsMask);
            if(mask)
            {
                // char - '0'
                __m128i sub = _mm_sub_epi8(value, zeroChar);
                __m128i numbs = _mm_and_si128(sub, numbsMask);

                alignas(16) int8_t values[16];
                _mm_storeu_si128((__m128i_u*) values, numbs);
                int offset = 0;
                while(mask)
                {
                    int zeros = std::countr_zero(mask);
                    mask = mask >> zeros;
                    offset += zeros;

                    last = (int)values[offset];
                    first = first == 0 ? last : first;

                    offset++;
                    mask >>= 1;
                }

            }
            len -= 16;
            ptr++;
        }
        data = sLineEnds[i] + 1;

        sum += first * 10 + last;
    }
    return sum;

}

static int sParse01B(const char* data)
{

    int sum = 0;

    for(int i = 0; i < sLineNumbers; ++i)
    {
        int first = 0;
        int last = 0;

        data = i > 0 ? sLineEnds[i - 1] : data;
        while(first == 0)
        {
            char c = *data;
            if (isdigit(c))
            {
                first = c - '0';
            }
            else
            {
                int numb = sTestNumbers(data);
                if (numb > 0)
                {
                    first = numb;
                }
            }
            data++;
        }
        data = sLineEnds[i];
        while(last == 0)
        {
            char c = *data;
            if(isdigit(c))
            {
                last = c - '0';
            }
            else
            {
                int numb = sTestNumbers(data);
                if (numb > 0)
                {
                    last = numb;
                }
            }
            data--;
        }

        sum += first * 10 + last;

    }
    return sum;
}

#ifndef RUNNER
int main()
{
    sFindLineBreaks(data01A, sizeof(data01A));

    printf("Sum of lines 1A: %i\n", sParse01A(data01A));
    printf("Sum of lines 1B: %i\n", sParse01B(data01A));
    return 0;
}
#endif



void parse01()
{
    sFindLineBreaks(data01A, sizeof(data01A));
}

int run01A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int aResult = sParse01A(data01A);
    if(printOut)
        charsAdded = sprintf(buffer, "Sum of lines 1A: %i", aResult);
    return charsAdded;
}

int run01B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int resultB = sParse01B(data01A);
    if(printOut)
        charsAdded = sprintf(buffer, "Sum of lines 1B: %i", resultB);

    return charsAdded;
}

