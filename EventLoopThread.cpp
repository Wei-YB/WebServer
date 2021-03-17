#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Poller.h"

USE_NAMESPACE

EventLoopThread::EventLoopThread(const std::string& name, const ThreadInitCallback& func) : loop_(nullptr),
                                                                                     thread_([this]() {
                                                                                         this->threadFunc();
                                                                                     }, name),
                                                                                     mutex_(),
                                                                                     cond_(mutex_),
                                                                                     callback_(func) { }

EventLoopThread::~EventLoopThread() {
    if (loop_ != nullptr)
    {
        loop_->stop();
        thread_.join();
    }
}

// before start the loop must be valuable
EventLoop* EventLoopThread::start() {
    thread_.start();
    EventLoop* loop = nullptr;
    {
        LockGuard lock(mutex_);
        while (!loop_) {
            cond_.wait();
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if(callback_) {
        callback_(&loop);
    }

    {
        LockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
    LockGuard lock(mutex_);
    loop_ = nullptr;
}
