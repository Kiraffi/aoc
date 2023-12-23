#pragma once

#if PROFILE
#include <chrono>
#include <stdio.h>
#include <thread>
struct ScopeProfile
{
    ScopeProfile(const char* scopeName)
    {
        name = scopeName;
        begin = std::chrono::high_resolution_clock::now();
    }

    ~ScopeProfile()
    {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        int64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
        float usTime = ns / 1000.0f;

        printf("%s: %fus\n", name, usTime);
    }
    std::chrono::high_resolution_clock::time_point begin;
    const char* name;
};
#define CONCAT1(a, b) a ## b
#define CONCAT2(a, b) CONCAT1(a, b)
#define TIMEDSCOPE(name) ScopeProfile CONCAT2(profile, __LINE__)(name)


#else
#define TIMEDSCOPE(name)

#endif

