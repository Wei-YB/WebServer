#pragma once

#include "Thread.h"

START_NAMESPACE

class EventLoop;

class EventLoopThread {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const std::string& name = "", const ThreadInitCallback& func = ThreadInitCallback());
    ~EventLoopThread();

    EventLoop* start();
private:

    void threadFunc();

private:

    EventLoop* loop_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;
    ThreadInitCallback callback_;
};

END_NAMESPACE