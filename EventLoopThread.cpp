#include "EventLoopThread.h"
#include "EventLoop.h"

USE_NAMESPACE

EventLoopThread::EventLoopThread(const std::string& name) : loop_(), thread_([this](){ this->loop_.loop(); }, name ) {
    
}


EventLoop& EventLoopThread::getLoop() {
    return loop_;
}

EventLoop& EventLoopThread::run() {
    thread_.start();
}

