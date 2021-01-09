#include "CountDownLatch.h"

USE_NAMESPACE

CountDownLatch::CountDownLatch(int count): condition_(mutex_), count_(count) { }

void CountDownLatch::wait() {
    LockGuard guard(mutex_);
    while (count_ > 0) {
        condition_.waitWithMutexLocked();
    }
}

void CountDownLatch::countDown() {
    LockGuard guard(mutex_);
    --count_;
    if (count_ == 0)
        condition_.notifyAll();
}

int CountDownLatch::getCount() const {
    LockGuard guard(mutex_);
    return count_;
}

