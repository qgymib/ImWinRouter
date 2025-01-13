#include <windows.h>
#include <stdexcept>
#include "time.hpp"

static uint64_t hrtime_frequency_ = 0;

static BOOL CALLBACK s_time_init(PINIT_ONCE, PVOID, PVOID*)
{
    LARGE_INTEGER perf_frequency;
    if (QueryPerformanceFrequency(&perf_frequency) == 0)
    {
        abort();
    }

    hrtime_frequency_ = perf_frequency.QuadPart;
    return TRUE;
}

uint64_t iwr::hrtime()
{
    static INIT_ONCE token = INIT_ONCE_STATIC_INIT;
    InitOnceExecuteOnce(&token, s_time_init, nullptr, nullptr);

    LARGE_INTEGER counter;
    if (QueryPerformanceCounter(&counter) == 0)
    {
        throw std::runtime_error("QueryPerformanceCounter() failed");
    }

    double scaled_freq = (double)hrtime_frequency_ / 1000000000;
    double result = (double)counter.QuadPart / scaled_freq;
    return (uint64_t)result;
}

uint64_t iwr::lrtime()
{
    return iwr::hrtime() / 1000 / 1000;
}
