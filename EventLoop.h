#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <functional>

#include "base/Mutex.h"
#include "base/Timestamp.h"

#include "Channel.h"


START_NAMESPACE

class Poller;
class Channel;


class EventLoop {
public:

    using Runnable = std::function<void()>;

    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&)            = delete;
    EventLoop(EventLoop&&)                 = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop& operator=(EventLoop&&)      = delete;


    void loop();

    [[nodiscard]]
    bool running() const { return running_; }

    void stop();

    void queueInLoop(const Runnable& func);

    // should not use the r-value ref;
    void runInLoop(const Runnable& func);

    void insertChannel(Channel&) const;
    void removeChannel(Channel&) const;
    void updateChannel(Channel&) const;

    bool hasChannel(Channel&) const;

    [[nodiscard]]
    bool isInLoopThread() const;

    void assertInLoopThread() const;


    // add wakeup feature to eventLoop, in order to wakeup epoll_wait
    void wakeup() const;

private:

    void handleEvent();

    void handleRunnable();

    void handleWakeUp() const;

private:
    bool running_;

    bool eventHandling_;
    bool queueRunning_;


    // used to wake up epoll_wait
    int     wakeupFd_;
    Channel wakeupChannel_;


    std::atomic<bool> stop_;
    // int currentThread_;
    const pid_t threadId_;
    Timestamp   pollReturnTime_;

    std::unique_ptr<Poller> poller_;

    Mutex                 queueLock_;
    std::vector<Runnable> runnableQueue_;

    std::vector<Channel*> activeChannels_;

    [[maybe_unused]]
    Channel*              currentActiveChannel_;
};

END_NAMESPACE
