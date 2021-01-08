#pragma once

#include <cctype>
#include <cstdint>
#include <string>
#include <thread>

#include "Utli.h"

START_NAMESPACE namespace  ThisThread {
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern  __thread size_t t_tidStringSize;
    extern  __thread const char* t_threadName;
    

    void cacheTid();

    int tid() {
        return t_cachedTid;
    }

    const char* tidString() {
        return t_tidString;
    }

    int tidStringLength();

    const char* name();

    bool isMainThread();

    void sleepUsec(int64_t usec);

//    std::string stackTrace(bool demangle);

    }

END_NAMESPACE
