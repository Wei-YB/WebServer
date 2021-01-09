#include "Mutex.h"

#include <cassert>

// ThisThread::tid
#include "ThisThread.h"

USE_NAMESPACE

Mutex::Mutex() : mutex_(), holder_(0) {
    pthread_mutex_init(&mutex_, nullptr);
}

Mutex::~Mutex() {
   // assert(holder_ == 0);
    pthread_mutex_destroy(&mutex_);
}

bool Mutex::isLockedByThisThread() const {
    return holder_ == ThisThread::tid();
}

void Mutex::assertLocked() const {
    assert(isLockedByThisThread());
}

void Mutex::lock() {
    pthread_mutex_lock(&mutex_);
  //  holder_ = ThisThread::tid();
}

void Mutex::unlock() {
    pthread_mutex_unlock(&mutex_);
  //  holder_ = 0;
}

pthread_mutex_t* Mutex::getPthreadMutex() {
    return &mutex_;
}

LockGuard::LockGuard(Mutex& lock) : lock_(lock) {
    lock.lock();
}

LockGuard::~LockGuard() {
    lock_.unlock();
}
