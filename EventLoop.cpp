#include "EventLoop.h"


#include "Channel.h"
#include "Poller.h"
#include "ThisThread.h"

USE_NAMESPACE

constexpr int PollTimeMs = 10000;

// must create in loop thread
EventLoop::EventLoop(): running_(false),
                        stop_(false),
                        threadId_(ThisThread::tid()),
                        pollReturnTime_(),
                        poller_(std::make_unique<Poller>()), currentActiveChannel_(nullptr) { }


void EventLoop::loop() {
    assert(!stop_);
    if (running_ == false)
        running_ = true;
    else
        LOG_ERROR << "event loop already running";
    while (!stop_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(10000, activeChannels_);

        handleEvent();

        handleRunnable();

    }
    LOG_TRACE << "EventLoop " << this << " stop looping";
    running_ = false;
}


void EventLoop::stop() {
    running_ = false;
}

void EventLoop::insertChannel(Channel& channel) const {
    poller_->insert(channel);
}

void EventLoop::removeChannel(Channel& channel) const {
    poller_->remove(channel);
}

void EventLoop::updateChannel(Channel& channel) const {
    poller_->update(channel);
}

bool EventLoop::hasChannel(Channel& channel) const {
    return poller_->hasChannel(channel);
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == ThisThread::tid();
}

void EventLoop::assertInLoopThread() const {
    if (!isInLoopThread()) {
        LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " << ThisThread::tid();
    }
}


void EventLoop::handleEvent() {
    eventHandling_ = true;
    LOG_TRACE << "handle events";
    for (auto* channel : activeChannels_)
        channel->handleEvent();
    eventHandling_ = false;
}

void EventLoop::handleRunnable() {
    std::vector<Runnable> que;
    {
        LockGuard guard(queueLock_);
        que.swap(runnableQueue_);
    }
    queueRunning_ = true;
    for (auto& func : runnableQueue_)
        func();
    queueRunning_ = false;
}
