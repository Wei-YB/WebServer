#include "CountDownLatch.h"

USE_NAMESPACE

CountDownLatch::CountDownLatch(int count):count_(count) {
    
}

void CountDownLatch::wait() {
    while(count_ > 0) {
        condition_.wait(mutex_);
    }
}

void CountDownLatch::countDown() {
    UniqueMutexLockGuard guard(mutex_);
    --count_;
    if (count_ == 0)
        condition_.notify_all();
}

int CountDownLatch::getCount() const {
    UniqueMutexLockGuard guard(mutex_);
    return count_;
}

