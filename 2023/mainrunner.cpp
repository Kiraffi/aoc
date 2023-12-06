#include <chrono>
#include <inttypes.h>
#include <stdio.h>
#include <thread>

static constexpr int sParseAmount = 1;


void parse01();
void parse05();

void run01A(bool printOut);
void run01B(bool printOut);
void run02A(bool printOut);
void run02B(bool printOut);
void run03A(bool printOut);
void run03B(bool printOut);
void run04A(bool printOut);
void run04B(bool printOut);
void run05A(bool printOut);
void run05B(bool printOut);
void run06A(bool printOut);
void run06B(bool printOut);

using RunFunc = void (*)(bool);
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
    printf("Parsing %s: %fus\n", dayText, usTime);
}

void measureRun(int amount, const char* dayText, RunFunc func)
{
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < amount - 1; ++i)
    {
        func(false);
    }
    func(true);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
    ns /= amount;
    float usTime = ns / 1000.0f;
    printf("Running %s: duration %fus\n", dayText, usTime);
}

void runAll(bool unused)
{
    measureParse(sParseAmount, "01", parse01);
    measureRun(sParseAmount, "01A", run01A);
    measureRun(sParseAmount, "01B", run01B);
    printf("\n");
    measureRun(sParseAmount, "02A", run02A);
    measureRun(sParseAmount, "02B", run02B);
    printf("\n");
    measureRun(sParseAmount, "03A", run03A);
    measureRun(sParseAmount, "03B", run03B);
    printf("\n");
    measureRun(sParseAmount, "04A", run04A);
    measureRun(sParseAmount, "04B", run04B);
    printf("\n");
    measureParse(sParseAmount, "05", parse05);
    measureRun(sParseAmount, "05A", run05A);
    measureRun(sParseAmount, "05B", run05B);
    printf("\n");
    measureRun(sParseAmount, "06A", run06A);
    measureRun(sParseAmount, "06B", run06B);
    printf("\n");
}

int main()
{
    measureRun(1, "all", runAll);
    return 0;
}