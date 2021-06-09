#include "EventLoop.h"

#include <unistd.h>

#include "base/Mutex.h"
#include "base/ThisThread.h"

#include "Channel.h"
#include "Poller.h"
#include "Socket.h"


USE_NAMESPACE

[[maybe_unused]]
constexpr int PollTimeMs = 10000;

// must create in loop thread
EventLoop::EventLoop() : running_(false),
                         eventHandling_(false),
                         queueRunning_(false),
                         wakeupFd_(socket::eventFd()),
                         wakeupChannel_(*this, wakeupFd_),
                         stop_(false),
                         threadId_(ThisThread::tid()),
                         poller_(std::make_unique<Poller>()), currentActiveChannel_(nullptr) {
    wakeupChannel_.setReadCallback([this]() {
        handleWakeUp();
    });
    wakeupChannel_.enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_.disableAll();
    socket::Close(wakeupFd_);
}


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

void EventLoop::queueInLoop(const Runnable& func) {
    {
        LockGuard guard(queueLock_);
        runnableQueue_.push_back(func);
    }
    if (!isInLoopThread() || eventHandling_)
        wakeup();
}

void EventLoop::runInLoop(const Runnable& func) {
    if (isInLoopThread())
        func();
    else
        queueInLoop(func);
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

void EventLoop::wakeup() const {
    uint64_t   val = 1;
    const auto ret = ::write(wakeupFd_, &val, sizeof val);
    if (ret != sizeof val) {
        LOG_SYSERR << "EventLoop::wakeup() writes " << ret << " bytes instead of 8";
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
    LOG_TRACE << "handle runnable";
    std::vector<Runnable> que;
    {
        LockGuard guard(queueLock_);
        que.swap(runnableQueue_);
    }
    queueRunning_ = true;
    // fixed: used wrong runnable queue
    for (auto& func : que)
        func();
    queueRunning_ = false;
}

void EventLoop::handleWakeUp() const {
    uint64_t val = 1;
    while (true) {
        const auto ret = ::read(wakeupFd_, &val, sizeof val);
        if (ret < 0) {
            if (errno == EAGAIN)
                break;
            else if (errno == EINTR)
                continue;
            else {
                LOG_SYSERR << "EventLoop::handleWakeUp() error";
            }
        }
        else if (ret != sizeof val) {
            LOG_ERROR << "EventLoop::handleWakeUp() reads " << ret << " bytes instead of 8";
        }
    }
}
