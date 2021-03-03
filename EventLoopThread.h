#pragma once

#include "Thread.h"

START_NAMESPACE

class EventLoop;

class EventLoopThread {
public:
    EventLoopThread(const std::string& name);
    ~EventLoopThread();


    EventLoop& getLoop();
    EventLoop& run();
private:

    EventLoop& loop_;
    Thread thread_;
};

END_NAMESPACE