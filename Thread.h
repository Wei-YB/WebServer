#pragma once

#include "Utli.h"

// standard library
#include <functional>
#include <atomic>

// POSIX thread library
#include <pthread.h>

#include "CountDownLatch.h"

START_NAMESPACE

class Thread {
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc, const std::string& name = std::string());
    // FIXME: make it movable in C++11
    ~Thread();

    void start();
    int join(); // return pthread_join()

    bool started() const { return started_; }
    // pthread_t pthreadId() const { return pthreadId_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const;

    static int numCreated();

private:
    void setDefaultName();

    bool       started_;
    bool       joined_;
    pthread_t  pthreadId_;
    pid_t      tid_;
    ThreadFunc func_;
    std::string     name_;
    CountDownLatch latch_;

    static std::atomic_int32_t numCreated_;
};

END_NAMESPACE