#pragma once

#include "Util.h"

START_NAMESPACE namespace ThisThread {
    extern thread_local int         t_cachedTid;
    extern thread_local char        t_tidString[32];
    extern thread_local size_t      t_tidStringSize;
    extern thread_local const char* t_threadName;
    int tid();

    const char* tidString();

    size_t tidStringLength();

    bool isMainThread();

    }
END_NAMESPACE