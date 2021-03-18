#include "EventLoopThreadPool.h"

#include "EventLoop.h"
#include "EventLoopThread.h"

USE_NAMESPACE

EventLoopThreadPool::EventLoopThreadPool(size_t size, const std::string& name, ThreadInitCallback func) :
    started_(false), loopSize_(size), next_(0), name_(name) {
    for (size_t i = 0; i < loopSize_; ++i) {
        const auto threadName = name + std::to_string(i);
        threads_.push_back(std::make_unique<EventLoopThread>(threadName, func) );
    }
}

void EventLoopThreadPool::start() {
    started_ = true;
    for(auto& thread : threads_) {
        loops_.emplace_back(thread->start());
    }
}


EventLoop* EventLoopThreadPool::getLoop() {
    return loops_[inc()];
}

const std::vector<EventLoop*>& EventLoopThreadPool::getAllLoop() const {
    return loops_;
}

