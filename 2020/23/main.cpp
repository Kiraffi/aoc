
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

std::string shuffleA(const std::vector<int>& input, int rounds)
{
    // copy
    auto vec = input;
    for(auto &v : vec)
    {
        v -= 1;
    }
    int currentCup = 0;

    int inHand[3] = {};

    for(int round = 0; round < rounds; ++round)
    {
        int picked = vec[currentCup];
        inHand[0] = vec[(currentCup + 1) % 9];
        inHand[1] = vec[(currentCup + 2) % 9];
        inHand[2] = vec[(currentCup + 3) % 9];
        int lower = (picked + 8) % 9; // -1;

        while(inHand[0] == lower || inHand[1] == lower || inHand[2] == lower)
        {
            lower = (lower + 8) % 9; // -1
        }
        for(int i = 0; i < 9; ++i)
        {
            int index = (i + currentCup + 4) % 9;
            int next = (currentCup + 1 + i) % 9;
            vec[next] = vec[index];
            if(vec[index] == lower)
            {
                vec[(next + 1) % 9] = inHand[0];
                vec[(next + 2) % 9] = inHand[1];
                vec[(next + 3) % 9] = inHand[2];
                break;
            }
        }
        currentCup = (currentCup + 1) % 9;
    }
    for(auto &v : vec)
    {
        v += 1;
        printf("%i", v);
    }
    printf("\n");
    return "";
}

struct Links
{
    Links *next = nullptr;
    int value = 0;
};


std::string shuffleB(const std::vector<int>& input, int rounds, int amount)
{
    // copy
    std::unordered_map<int, int> hash;
    std::vector<Links> list;
    list.resize(amount);
    for(int i = 0; i < amount; ++i)
    {
        if(i < input.size())
        {
            list[i].value = input[i] - 1;
            hash.insert({list[i].value, i});
            if(i > 0)
            {
                list[i - 1].next = &list[i];
            }
        }
        else
        {
            list[i - 1].next = &list[i];
            list[i].value = i;
            hash.insert({list[i].value, i});
        }
    }
    list[amount - 1].next = &list[0];

    Links* currentCup = &list[0];

    //for(int i = 0; i <= amount; ++i)
    //{
    //    int v = currentCup->value + 1;
    //    printf("%i ", v);
    //    currentCup = currentCup->next;
    //}
    //printf("\n");

    for(int round = 0; round < rounds; ++round)
    {
        Links* chain = currentCup->next;
        currentCup->next = chain->next->next->next;
        int nextone= currentCup->next->value;
        int findValue = (currentCup->value + amount - 1) % amount;

        while(findValue == chain->value
            || findValue == chain->next->value
            || findValue == chain->next->next->value)
        {
            findValue = (findValue + amount - 1) % amount;
        }

        Links* f = &list[hash[findValue]];
        while(f->value != findValue)
        {
            f = f->next;
        }
        chain->next->next->next = f->next;
        f->next = chain;

        currentCup = currentCup->next;
    }
    Links* one = nullptr;
    for(int i = 0; i < amount; ++i)
    {
        if(currentCup->value == 0)
        {
            one = currentCup;
        }
        currentCup->value += 1;
        int v = currentCup->value;
        //printf("%i ", v);
        currentCup = currentCup->next;
    }


    printf("\n");
    printf("%i, %i, %i\n", one->value, one->next->value, one->next->next->value);
    return "";
}


int main(int argc, char** argv)
{
    //std::vector<int> input = {3, 8, 9, 1, 2, 5, 4, 6, 7};
    std::vector<int> input = {6, 4, 3, 7, 1, 9, 2, 5, 8};


    shuffleA(input, 100);
    shuffleB(input, 10'000'000, 1'000'000); //'000'000, 1'000'000);


    return 0;
}