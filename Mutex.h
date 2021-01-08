#pragma once

#include <pthread.h>
#include "Utli.h"

START_NAMESPACE


class MutexLock {
public:
    MutexLock();

    MutexLock(MutexLock&&) = delete;
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    MutexLock& operator =(MutexLock&&) = delete;

    ~MutexLock();

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

class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock& lock);

    ~MutexLockGuard();

private:
    MutexLock& lock_;
};


END_NAMESPACE
