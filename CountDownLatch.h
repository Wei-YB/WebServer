#pragma once

#include  "Utli.h"
#include "Mutex.h"
#include "Condition.h"
START_NAMESPACE

class CountDownLatch {
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable  unique_lock<Mutex>  mutex_;
    Condition condition_;
    int count_;
};

END_NAMESPACE