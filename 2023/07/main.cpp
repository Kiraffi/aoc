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

alignas(16) static constexpr char test07A[] =
    R"(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)";



using MapCharFn = char(*)(char c);

enum HandType: int
{
    Number,
    Pair,
    TwoPairs,
    ThreeOfAKind,
    FullHouse,
    FourOfAKind,
    FiveOfAKing,

    HandTypeCount
};

struct Hand
{
    int32_t cards;
    int32_t bid;
};




static int64_t sParserNumber(int64_t startNumber, const char** data)
{
    int64_t number = startNumber;
    while(**data == ' ') ++*data;
    while(**data >= '0')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return number;
}

static char sMapCardCharA(char c)
{
    switch (c)
    {
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;
        case 'T':
            c = '9' + 1;
            break;
        case 'J':
            c = '9' + 2;
            break;
        case 'Q':
            c = '9' + 3;
            break;
        case 'K':
            c = '9' + 4;
            break;
        case 'A':
            c = '9' + 5;
            break;
        default:
            assert(c && 0);
            break;
    }
    return c;
}


static char sMapCardCharB(char c)
{
    switch (c)
    {
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;
        case 'T':
            c = '9' + 1;
            break;
        case 'J':
            c = '1';
            break;
        case 'Q':
            c = '9' + 3;
            break;
        case 'K':
            c = '9' + 4;
            break;
        case 'A':
            c = '9' + 5;
            break;
        default:
            assert(c && 0);
            break;
    }
    return c;
}

static void sAddToHands(int hand, int bid, int same, int kinds, std::vector<std::vector<Hand>>& allHands)
{
    Hand handCard = {.cards = hand, .bid = bid };
    switch(same)
    {
        case 1:
            assert(kinds == 5);
            allHands[HandType::Number].push_back(handCard);
            break;
        case 2:
            assert(kinds >= 3 && kinds <= 4);
            allHands[kinds == 3 ? HandType::TwoPairs : HandType::Pair].push_back(handCard);
            break;
        case 3:
            assert(kinds >= 2 && kinds <= 3);
            allHands[kinds == 3 ? HandType::ThreeOfAKind : HandType::FullHouse].push_back(handCard);
            break;
        case 4:
            assert(kinds == 2);
            allHands[HandType::FourOfAKind].push_back(handCard);
            break;
        case 5:
            assert(kinds == 1);
            allHands[HandType::FiveOfAKing].push_back(handCard);
            break;
        default:
            assert(same);
            break;
    }
}

static void sSortAllHands(std::vector<std::vector<Hand>>& allHands)
{
    for(std::vector<Hand>& hands : allHands)
    {
        std::sort(hands.begin(), hands.end(), [](const Hand& a, const Hand& b)
        {
            return a.cards < b.cards;
        });
    }

}

static int64_t sCalculateWinnings(const std::vector<std::vector<Hand>>& allHands)
{
    int rank = 1;
    int64_t winnings = 0;
    for(const std::vector<Hand>& hands : allHands)
    {
        for(const Hand& hand : hands)
        {
            winnings += rank * hand.bid;
            ++rank;
        }
    }
    return winnings;

}

static int64_t sParse07A(const char* data)
{
    std::vector<std::vector<Hand>> allHands;
    allHands.resize(HandType::HandTypeCount, {});

    uint32_t cards[5] = {};

    while(*data)
    {
        int hand = 0;

        int kinds = 0;
        int same = 0;
        uint32_t checked = 0;
        for(int i = 0; i < 5; ++i)
        {
            char c = *data++;
            c = char(sMapCardCharA(c) - '1');

            hand <<= 4;
            hand |= c;

            cards[4 - i] = c * 0x11111;
        }
        for(int i = 0; i < 5; ++i)
        {
            if(checked  & (1 << (4 * i)))
                continue;
            uint32_t value = cards[i] ^ hand;
            value = ~value;
            value = value & (value >> 2);
            value = value & (value >> 1);
            value &= 0x11111;
            assert(value);
            checked |= value;
            kinds++;
            int currentSame = std::popcount(value);
            same = same < currentSame ? currentSame : same;

        }
        assert(checked == 0x11111);

        int bid = sParserNumber(0, &data);
        sAddToHands(hand, bid, same, kinds, allHands);
        data++;
    }
    sSortAllHands(allHands);
    return sCalculateWinnings(allHands);
}

static int64_t sParse07B(const char* data)
{
    std::vector<std::vector<Hand>> allHands;
    allHands.resize(HandType::HandTypeCount, {});

    while(*data)
    {
        char cards[16] = {};
        int64_t hand = 0;
        int kinds = 0;
        int same = 0;
        for(int i = 0; i < 5; ++i)
        {
            char c = *data++;
            c = sMapCardCharB(c) - '0';

            hand <<= 4;
            hand |= c;
            char& cardCount = cards[(int)c];
            cardCount++;
            if(c != 1)
            {
                kinds += cardCount == 1 ? 1 : 0;
                same = same < cardCount ? cardCount : same;
            }
        }
        // Jokers
        if(cards[1])
        {
            same += cards[1];
            if(kinds == 0)
                kinds = 1;
        }
        int bid = sParserNumber(0, &data);
        sAddToHands(hand, bid, same, kinds, allHands);
        data++;
    }
    sSortAllHands(allHands);
    return sCalculateWinnings(allHands);
}

#ifndef RUNNER
int main()
{
    printf("7A: winnings: %" PRIi64 "\n", sParse07A(data07A));
    printf("7B: winnings: %" PRIi64 "\n", sParse07B(data07A));
    return 0;
}
#endif

int run07A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse07A(data07A);

    if(printOut)
        charsAdded = sprintf(buffer, "7A: winnings: %" PRIi64, aResult);
    return charsAdded;
}

int run07B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse07B(data07A);

    if(printOut)
        charsAdded = sprintf(buffer, "7B: winnings: %" PRIi64, resultB);

    return charsAdded;
}

