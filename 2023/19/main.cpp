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

alignas(32) static constexpr char test19A[] =
    R"(px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013}
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
    while(**data >= '0' && **data <= '9')
    {
        number = ((**data) - '0') + number * 10;
        ++*data;
    }
    while(**data == ' ') ++*data;
    return neg ? -number : number;
}

static int sGetOrAdd(std::unordered_map<std::string, int>& stringMap, const char** data)
{
    const char* tmp = *data;
    while(isalpha(*tmp)) tmp++;
    std::string s(*data, tmp);
    auto iter = stringMap.find(s);
    int index = -1;
    if(iter == stringMap.end())
    {
        index = (int)stringMap.size();
        stringMap.insert({s, index});
    }
    else
    {
        index = iter->second;
    }
    *data = tmp;
    return index;

}

enum OpChar : uint8_t
{
    None = 0,
    LessThan = 1,
    GreaterThan = 2,
    SChar = 4,
    MChar = 8,
    AChar = 16,
    XChar = 32,
};

struct Statement
{
    int16_t cmpValues[4];
    uint16_t jmps[4];
    OpChar opChars[4];
};

static void sParseData(std::unordered_map<std::string, int>& stringMap, Statement* statements, const char** ptr)
{
    const char* data = *ptr;

    while(*data != '\n')
    {
        int index = sGetOrAdd(stringMap, &data);
        data++;
        Statement& statement = statements[index];
        int i = 0;
        while(*data != '}')
        {
            if(*(data + 1) != '<' && *(data + 1) != '>')
            {
                statement.opChars[i] = None;
                statement.jmps[i] = sGetOrAdd(stringMap, &data);
            }
            else
            {
                switch (*data)
                {
                    case 's':
                        statement.opChars[i] = SChar;
                        break;
                    case 'm':
                        statement.opChars[i] = MChar;
                        break;
                    case 'a':
                        statement.opChars[i] = AChar;
                        break;
                    case 'x':
                        statement.opChars[i] = XChar;
                        break;
                }
                data++;
                if (*data == '<')
                    statement.opChars[i] = OpChar(statement.opChars[i] | LessThan);
                else
                    statement.opChars[i] = OpChar(statement.opChars[i] | GreaterThan);
                data++;
                statement.cmpValues[i] = (int16_t)sParserNumber(0, &data);
                data++;
                statement.jmps[i] = sGetOrAdd(stringMap, &data);
                ++i;
                ++data;
            }
        }
        data++;
        data++;
    }
    data++;

    *ptr = data;
}


static int64_t sParseA(const char* data)
{
    TIMEDSCOPE("19A Total");
    std::unordered_map<std::string, int> stringMap;
    Statement statements[1024] = {};

    sParseData(stringMap, statements, &data);

    int indexA = stringMap["A"];
    int indexR = stringMap["R"];
    int indexIn = stringMap["in"];
    int64_t accepted = 0;
    while(*data)
    {
        data += 3;
        int16_t x = (int16_t)sParserNumber(0, &data);
        data += 3;
        int16_t m = (int16_t)sParserNumber(0, &data);
        data += 3;
        int16_t a = (int16_t)sParserNumber(0, &data);
        data += 3;
        int16_t s = (int16_t)sParserNumber(0, &data);
        data += 2;
        int index = indexIn;
        while(index != indexR && index != indexA)
        {
            const Statement& state = statements[index];
            int i = 0;
            bool isTrue = false;
            while(!isTrue)
            {
                switch ((int) state.opChars[i])
                {
                    case None:
                        isTrue = true;
                        break;

                    case LessThan | XChar:
                        isTrue = x < state.cmpValues[i];
                        break;
                    case LessThan | MChar:
                        isTrue = m < state.cmpValues[i];
                        break;
                    case LessThan | AChar:
                        isTrue = a < state.cmpValues[i];
                        break;
                    case LessThan | SChar:
                        isTrue = s < state.cmpValues[i];
                        break;

                    case GreaterThan | XChar:
                        isTrue = x > state.cmpValues[i];
                        break;
                    case GreaterThan | MChar:
                        isTrue = m > state.cmpValues[i];
                        break;
                    case GreaterThan | AChar:
                        isTrue = a > state.cmpValues[i];
                        break;
                    case GreaterThan | SChar:
                        isTrue = s > state.cmpValues[i];
                        break;
                    default:
                        assert(false);
                }
                ++i;
            }
            --i;
            index = state.jmps[i];
        }
        if(index == indexA)
        {
            accepted += x + m + a + s;

        }
    }

    return accepted;
}

struct Range
{
    int min;
    int max;
};
struct State
{
    Range ranges[4];
    int currentIndex;
};

static void sHandleLessThan(std::vector<State> &states,
    int cmpValue,
    int jmpTrueIndex,
    State& state,
    int rangeIndex)
{
    if(state.ranges[rangeIndex].min < cmpValue)
    {
        State newState = state;
        newState.ranges[rangeIndex].max = sMin(newState.ranges[rangeIndex].max, cmpValue - 1);
        newState.currentIndex = jmpTrueIndex;
        states.push_back(newState);

        state.ranges[rangeIndex].min = cmpValue;
    }
}

static void sHandleGreaterThan(std::vector<State> &states,
    int cmpValue,
    int jmpTrueIndex,
    State& state,
    int rangeIndex)
{
    if(state.ranges[rangeIndex].max > cmpValue)
    {
        State newState = state;
        newState.ranges[rangeIndex].min = sMax(newState.ranges[rangeIndex].min, cmpValue + 1);
        newState.currentIndex = jmpTrueIndex;
        states.push_back(newState);

        state.ranges[rangeIndex].max = cmpValue;
    }
}


static int64_t sParseB(const char* data)
{
    TIMEDSCOPE("19B Total");
    std::unordered_map<std::string, int> stringMap;
    Statement statements[1024] = {};

    sParseData(stringMap, statements, &data);

    int indexA = stringMap["A"];
    int indexR = stringMap["R"];
    int indexIn = stringMap["in"];
    int64_t accepted = 0;

    std::vector<State> states;
    {
        State tmpState = {};
        tmpState.currentIndex = indexIn;
        tmpState.ranges[0] = {.min = 1, .max = 4000};
        tmpState.ranges[1] = {.min = 1, .max = 4000};
        tmpState.ranges[2] = {.min = 1, .max = 4000};
        tmpState.ranges[3] = {.min = 1, .max = 4000};
        states.push_back(tmpState);
    }

    while(!states.empty())
    {
        State state = states.back();
        states.pop_back();
        if(state.currentIndex == indexR)
        {
            continue;
        }

        if(state.currentIndex == indexA)
        {
            int64_t mul = state.ranges[0].max + 1 - state.ranges[0].min;
            mul *= state.ranges[1].max + 1 - state.ranges[1].min;
            mul *= state.ranges[2].max + 1 - state.ranges[2].min;
            mul *= state.ranges[3].max + 1 - state.ranges[3].min;
            accepted += mul;
            continue;
        }
        const Statement& statement = statements[state.currentIndex];
        int i = 0;
        bool last = false;
        while(!last)
        {
            switch ((int) statement.opChars[i])
            {
                case None:
                    state.currentIndex = statement.jmps[i];
                    states.push_back(state);
                    last = true;
                    break;

                case LessThan | XChar:
                    sHandleLessThan(states, statement.cmpValues[i], statement.jmps[i], state, 0);
                    last = state.ranges[0].min > state.ranges[0].max;
                    break;
                case LessThan | MChar:
                    sHandleLessThan(states, statement.cmpValues[i], statement.jmps[i], state, 1);
                    last = state.ranges[1].min > state.ranges[1].max;
                    break;
                case LessThan | AChar:
                    sHandleLessThan(states, statement.cmpValues[i], statement.jmps[i], state, 2);
                    last = state.ranges[2].min > state.ranges[2].max;
                    break;
                case LessThan | SChar:
                    sHandleLessThan(states, statement.cmpValues[i], statement.jmps[i], state, 3);
                    last = state.ranges[3].min > state.ranges[3].max;
                    break;

                case GreaterThan | XChar:
                    sHandleGreaterThan(states, statement.cmpValues[i], statement.jmps[i], state, 0);
                    last = state.ranges[0].min > state.ranges[0].max;
                    break;
                case GreaterThan | MChar:
                    sHandleGreaterThan(states, statement.cmpValues[i], statement.jmps[i], state, 1);
                    last = state.ranges[1].min > state.ranges[1].max;
                    break;
                case GreaterThan | AChar:
                    sHandleGreaterThan(states, statement.cmpValues[i], statement.jmps[i], state, 2);
                    last = state.ranges[2].min > state.ranges[2].max;
                    break;
                case GreaterThan | SChar:
                    sHandleGreaterThan(states, statement.cmpValues[i], statement.jmps[i], state, 3);
                    last = state.ranges[3].min > state.ranges[3].max;
                    break;
                default:
                    assert(false);
            }
            ++i;
        }
    }

    return accepted;
}

static int sPrintA(char* buffer, int64_t value)
{
    return sprintf(buffer, "19A: Accepted: %" PRIi64, value);
}

static int sPrintB(char* buffer, int64_t value)
{
    return sprintf(buffer, "19B: Accepted: %" PRIi64, value);
}

#ifndef RUNNER
int main()
{
    char printBuffer[128];

    sPrintA(printBuffer, sParseA(data19A));
    printf("%s\n", printBuffer);

    sPrintB(printBuffer, sParseB(data19A));
    printf("%s\n", printBuffer);
    return 0;
}
#endif

int run19A(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultA = sParseA(data19A);

    if(printOut)
        charsAdded = sPrintA(buffer, resultA);
    return charsAdded;
}

int run19B(bool printOut, char* buffer)
{
    int charsAdded = 0;
    int64_t resultB = sParseB(data19A);

    if(printOut)
        charsAdded = sPrintB(buffer, resultB);

    return charsAdded;
}

