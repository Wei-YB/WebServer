#pragma once

#include <pthread.h>
#include "Util.h"

START_NAMESPACE


class Mutex {
public:
    Mutex();

    Mutex(Mutex&&) = delete;
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;
    Mutex& operator =(Mutex&&) = delete;

    ~Mutex();

    bool isLockedByThisThread() const;

    void assertLocked() const;

    void lock();

    void unlock();

    bool tryLock();

    pthread_mutex_t* getPthreadMutex();

private:
    pthread_mutex_t mutex_;
    pid_t holder_;

    friend class Condition;
};

class LockGuard {
public:
    explicit LockGuard(Mutex& lock);
    LockGuard(const LockGuard&) = delete;
    LockGuard(LockGuard&&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
    LockGuard& operator=(LockGuard&&) = delete;


    ~LockGuard();

private:
    Mutex& lock_;
};


END_NAMESPACE
