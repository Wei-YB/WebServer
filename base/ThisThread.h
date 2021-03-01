#pragma once

#include <cctype>
#include <cstdint>
#include <string>
#include <thread>

#include "Util.h"

START_NAMESPACE namespace ThisThread {
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread size_t t_tidStringSize;
    extern __thread const char* t_threadName;


    void cacheTid();

    int tid();

    const char* tidString();

    size_t tidStringLength();

    const char* name();

    bool isMainThread();

}
END_NAMESPACE