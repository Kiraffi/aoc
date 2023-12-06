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


static const char* numbers[] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
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

static bool sTestNumber(const char* str, const char* numberStr)
{
    while(*str == *numberStr)
    {
        ++str;
        ++numberStr;
    }
    return *numberStr == '\0';
}

static int sTestNumbers(const char* str)
{
    for(int i = 1; i < 10; ++i)
    {
        if(sTestNumber(str, numbers[i]))
        {
            return i;
        }
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

static void sParse01A(const char* data, bool printOut)
{
    int sum = 0;
    __m128i zeroChar = _mm_set1_epi8('0');
    __m128i zero = _mm_set1_epi8(0);
    __m128i ten = _mm_set1_epi8(10);
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
            // char - '0'
            __m128i sub = _mm_sub_epi8(value, zeroChar);

            // 0 < x < 10
            __m128i lt10 = _mm_cmplt_epi8(sub, ten);
            __m128i gt0 = _mm_cmpgt_epi8(sub, zero);

            // And the masks together to find numbers between 1 and 9.
            __m128i numbsMask = _mm_and_si128(lt10, gt0);
            __m128i numbs = _mm_and_si128(sub, numbsMask);

            uint32_t mask = _mm_movemask_epi8(numbsMask);
            if(mask)
            {
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

    if(printOut)
        printf("Sum of lines 1A: %i\n", sum);
}

static void sParse01B(const char* data, bool printOut)
{

    int first = -1;
    int last = -1;
    int sum = 0;
    while(*data != '\0')
    {
        char c = *data;
        if(c == '\n')
        {
            assert(first > 0);
            assert(last > 0);
            sum += first * 10 + last;
            first = last = -1;
        }
        else if(isdigit(c))
        {
            last = c - '0';
            assert(last > 0);
            first = first == -1 ? last : first;
        }
        else
        {
            int numb = sTestNumbers(data);
            if(numb > 0)
            {
                last = numb;
                first = first == -1 ? last : first;
            }
        }
        data++;

    }
    if(printOut)
        printf("Sum of lines 1B: %i\n", sum);
}

#ifndef RUNNER
int main()
{
    sFindLineBreaks(data01A, sizeof(data01A));


    sParse01A(data01A, true);
    sParse01B(data01A, true);
    return 0;
}
#endif



void parse01()
{
    sFindLineBreaks(data01A, sizeof(data01A));
}

void run01A(bool printOut)
{
    sParse01A(data01A, printOut);
}
void run01B(bool printOut)
{
    sParse01B(data01A, printOut);
}

