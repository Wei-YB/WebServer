#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "Util.h"


START_NAMESPACE

class EventLoop;

class EventLoopThread;

class EventLoopThreadPool {
public:

    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(size_t size = 4,const std::string& name = "",ThreadInitCallback func = ThreadInitCallback());

    void start();

    EventLoop* getLoop();

    [[nodiscard]]
    const std::vector<EventLoop*>& getAllLoop() const;

private:

    size_t inc() {
        ++next_;
        if (next_ == loopSize_)
            next_ = 0;
        return next_;
    }

    bool started_;

    const size_t loopSize_;
    size_t next_;
    std::string name_;

    ThreadInitCallback threadInitCallback_;

    std::vector<EventLoop*> loops_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
};

END_NAMESPACE