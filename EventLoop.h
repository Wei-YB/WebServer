#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include <queue>

#include "Channel.h"
#include "Mutex.h"
#include "ThisThread.h"
#include "Timestamp.h"

START_NAMESPACE

class Poller;
class Channel;

class EventLoop {
public:

    using Runnable = std::function<void()>;

    EventLoop();
    void loop();

    bool running() const { return running_; }

    void stop();

    void queueInLoop(Runnable&& func) {
        LockGuard guard(queueLock_);
        runnableQueue_.push_back(std::move(func));
    }

    void runInLoop(Runnable&& func) {
        if (isInLoopThread())
            func();
        else
            queueInLoop(std::move(func));
    }

    void insertChannel(Channel&) const;
    void removeChannel(Channel&) const;
    void updateChannel(Channel&) const;

    bool hasChannel(Channel&) const;

    bool isInLoopThread() const;

    void assertInLoopThread() const;

private:

    void handleEvent();

    void handleRunnable();
private:
    bool running_;

    bool eventHandling_;
    bool queueRunning_;

    std::atomic<bool> stop_;
    int currentThread_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;

    std::unique_ptr<Poller> poller_;

    Mutex queueLock_;
    std::vector<Runnable> runnableQueue_;

    std::vector<Channel*> activeChannels_;
    Channel* currentActiveChannel_;

    void* data = nullptr;

};

END_NAMESPACE