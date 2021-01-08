#include "Mutex.h"

#include <cassert>

// ThisThread::tid
#include "ThisThread.h"

USE_NAMESPACE

MutexLock::MutexLock() : mutex_(), holder_(0) {
    pthread_mutex_init(&mutex_, nullptr);
}

MutexLock::~MutexLock() {
    assert(holder_ == 0);
    pthread_mutex_destroy(&mutex_);
}

bool MutexLock::isLockedByThisThread() const {
    return holder_ == ThisThread::tid();
}

void MutexLock::assertLocked() const {
    assert(isLockedByThisThread());
}

void MutexLock::lock() {
    pthread_mutex_lock(&mutex_);
    holder_ = ThisThread::tid();
}

void MutexLock::unlock() {
    pthread_mutex_unlock(&mutex_);
    holder_ = 0;
}

pthread_mutex_t* MutexLock::getPthreadMutex() {
    return &mutex_;
}

MutexLockGuard::MutexLockGuard(MutexLock& lock) : lock_(lock) {
    lock.lock();
}

MutexLockGuard::~MutexLockGuard() {
    lock_.unlock();
}
