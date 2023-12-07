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

static int64_t sParse07A(const char* data, MapCharFn mapFn)
{
    std::vector<std::vector<Hand>> allHands;
    allHands.resize(HandType::HandTypeCount, {});

    while(*data)
    {
        int cards[16] = {};
        int64_t hand = 0;

        int kinds = 0;
        int same = 0;
        for(int i = 0; i < 5; ++i)
        {
            char c = *data++;
            c = mapFn(c) - '0';

            hand <<= 4;
            hand |= c;
            int& cardCount = cards[(int)c];
            kinds += (cardCount++ == 0) ? 1 : 0;

            same = same < cardCount ? cardCount : same;
        }
        int bid = sParserNumber(0, &data);
        sAddToHands(hand, bid, same, kinds, allHands);
        data++;
    }
    sSortAllHands(allHands);
    return sCalculateWinnings(allHands);
}

static int64_t sParse07B(const char* data, MapCharFn mapFn)
{
    std::vector<std::vector<Hand>> allHands;
    allHands.resize(HandType::HandTypeCount, {});

    while(*data)
    {
        int cards[16] = {};
        int64_t hand = 0;
        int kinds = 0;
        int same = 0;
        for(int i = 0; i < 5; ++i)
        {
            char c = *data++;
            c = mapFn(c) - '0';

            hand <<= 4;
            hand |= c;
            int& cardCount = cards[(int)c];
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
    printf("7A: winnings: %" PRIi64 "\n", sParse07A(data07A, sMapCardCharA));
    printf("7B: winnings: %" PRIi64 "\n", sParse07B(data07A, sMapCardCharB));
    return 0;
}
#endif

int run07A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t aResult = sParse07A(data07A, sMapCardCharA);

    if(printOut)
        charsAdded = sprintf(buffer, "7A: Potential records: %" PRIi64, aResult);
    return charsAdded;
}

int run07B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParse07B(data07A, sMapCardCharB);

    if(printOut)
        charsAdded = sprintf(buffer, "6B: Potential records: %" PRIi64, resultB);

    return charsAdded;
}

