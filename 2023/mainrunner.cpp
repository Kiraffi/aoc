#include <chrono>
#include <inttypes.h>
#include <stdio.h>
#include <thread>

static constexpr int sParseAmount = 1;

static char charBuffer[1024 * 1024 * 8] = {};
static int charBufferIndex = 0;

void parse01();
void parse05();
void parse08();

int run01A(bool printOut, char* buffer);
int run01B(bool printOut, char* buffer);
int run02A(bool printOut, char* buffer);
int run02B(bool printOut, char* buffer);
int run03A(bool printOut, char* buffer);
int run03B(bool printOut, char* buffer);
int run04A(bool printOut, char* buffer);
int run04B(bool printOut, char* buffer);
int run05A(bool printOut, char* buffer);
int run05B(bool printOut, char* buffer);
int run06A(bool printOut, char* buffer);
int run06B(bool printOut, char* buffer);
int run07A(bool printOut, char* buffer);
int run07B(bool printOut, char* buffer);
int run08A(bool printOut, char* buffer);
int run08B(bool printOut, char* buffer);
int run09A(bool printOut, char* buffer);
int run09B(bool printOut, char* buffer);
int run10A(bool printOut, char* buffer);
int run10B(bool printOut, char* buffer);
int run10C(bool printOut, char* buffer);
int run11A(bool printOut, char* buffer);
int run11B(bool printOut, char* buffer);

using RunFunc = int (*)(bool, char*);
using ParseFunc = void (*)();

void measureParse(int amount, const char* dayText, ParseFunc func)
{
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < amount; ++i)
    {
        func();
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    ns /= amount;
    float usTime = ns / 1000.0f;
    charBufferIndex += sprintf(charBuffer + charBufferIndex, "Parsing %s: %fus\n", dayText, usTime);

}

void measureRun(int amount, const char* dayText, RunFunc func)
{
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < amount - 1; ++i)
    {
        func(false, nullptr);
    }
    int added = func(true, charBuffer + charBufferIndex);
    charBufferIndex += added;
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    ns /= amount;
    float usTime = ns / 1000.0f;
    charBufferIndex += sprintf(charBuffer + charBufferIndex, " | Running %s: duration %fus\n", dayText, usTime);
}

static void sAddLineBreak()
{
    charBufferIndex += sprintf(charBuffer + charBufferIndex, "\n");
}

int runAll(bool, char*)
{
    measureParse(sParseAmount, "01", parse01);
    measureRun(sParseAmount, "01A", run01A);
    measureRun(sParseAmount, "01B", run01B);
    sAddLineBreak();
    measureRun(sParseAmount, "02A", run02A);
    measureRun(sParseAmount, "02B", run02B);
    sAddLineBreak();
    measureRun(sParseAmount, "03A", run03A);
    measureRun(sParseAmount, "03B", run03B);
    sAddLineBreak();
    measureRun(sParseAmount, "04A", run04A);
    measureRun(sParseAmount, "04B", run04B);
    sAddLineBreak();
    measureParse(sParseAmount, "05", parse05);
    measureRun(sParseAmount, "05A", run05A);
    measureRun(sParseAmount, "05B", run05B);
    sAddLineBreak();
    measureRun(sParseAmount, "06A", run06A);
    measureRun(sParseAmount, "06B", run06B);
    sAddLineBreak();
    measureRun(sParseAmount, "07A", run07A);
    measureRun(sParseAmount, "07B", run07B);
    sAddLineBreak();
    measureParse(sParseAmount, "08", parse08);
    measureRun(sParseAmount, "08A", run08A);
    measureRun(sParseAmount, "08B", run08B);
    sAddLineBreak();
    measureRun(sParseAmount, "09A", run09A);
    measureRun(sParseAmount, "09B", run09B);
    sAddLineBreak();
    measureRun(sParseAmount, "10A", run10A);
    //measureRun(sParseAmount, "10B", run10B);
    measureRun(sParseAmount, "10B - line inside out", run10C);
    sAddLineBreak();
    measureRun(sParseAmount, "11A", run11A);
    measureRun(sParseAmount, "11B", run11B);
    sAddLineBreak();
    return 0;
}

int main()
{
    measureRun(1, "all avg 1 times", runAll);
    printf("%s\n", charBuffer);
    return 0;
}