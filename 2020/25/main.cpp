
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
#include <iostream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

static const int64_t CardValue = 1614360;
static const int64_t DoorValue = 7734663;

static const int64_t TestCardValue = 5764801;
static const int64_t TestDoorValue = 17807724;

int64_t shuffle(int64_t a, int64_t value)
{
    return (a * value) % int64_t(20201227);
}

void partA(int64_t card, int64_t door)
{
    int64_t value = 1;
    int i = 0;
    while(value != card && value != door)
    {
        value = shuffle(value, 7);
        i += 1;
    }
    int64_t otherValue = value == card ? door : card;
    value = 1;
    for(int j = 0; j < i; ++j)
    {
        value = shuffle(value, otherValue);
    }

    printf("Part a: Flipped ones: %" PRIi64 "\n", value);
}

int main(int argc, char** argv)
{
    //partA(TestCardValue, TestDoorValue);
    partA(CardValue, DoorValue);

    return 0;
}