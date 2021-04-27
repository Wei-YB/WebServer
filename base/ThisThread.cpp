#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <cstdio>

#include "ThisThread.h"

USE_NAMESPACE

thread_local int    ThisThread::t_cachedTid = 0;
thread_local char   ThisThread::t_tidString[32];
thread_local size_t ThisThread::t_tidStringSize;

static void cacheTid() {
    ThisThread::t_cachedTid     = ::syscall(SYS_gettid);
    ThisThread::t_tidStringSize = sprintf(ThisThread::t_tidString, "%5d", ThisThread::t_cachedTid);
}

int ThisThread::tid() {
    if (unlikely(t_cachedTid == 0))
        cacheTid();
    return t_cachedTid;
}

const char* ThisThread::tidString() {
    return t_tidString;
}

size_t ThisThread::tidStringLength() {
    return t_tidStringSize;
}

bool ThisThread::isMainThread() {
    return getpid() == tid();
}
