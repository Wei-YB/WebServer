#pragma once


#include "Utli.h"
#include "Mutex.h"

START_NAMESPACE

class Condition {
public:
    Condition(Mutex& lock);

    Condition() = delete;

    Condition(const Condition&) = delete;
    Condition& operator=(const Condition&) = delete;

    Condition(Condition&&) = delete;
    Condition& operator=(Condition&&) = delete;


    ~Condition();

    void wait();

    bool waitForSeconds(double seconds);

    void notify();

    void notifyAll();

private:

    void waitWithMutexLocked();

    friend class CountDownLatch;

    Mutex& mutex_;
    pthread_cond_t cond_{};
};

END_NAMESPACE
