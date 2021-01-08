#pragma once

#include <pthread.h>
#include "Utli.h"

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

    pthread_mutex_t* getPthreadMutex();

private:
    pthread_mutex_t mutex_;
    pid_t holder_;

    friend class Condition;
};

class LockGuard {
public:
    explicit LockGuard(Mutex& lock);

    ~LockGuard();

private:
    Mutex& lock_;
};


END_NAMESPACE
