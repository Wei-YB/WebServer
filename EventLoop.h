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
    void stop();

    void queueInLoop(const Runnable& func) {
        LockGuard guard(queueLock_);
        runnableQueue_.push_back(func);
    }

    void runInLoop(const Runnable& func) {
        if (isInLoopThread())
            func();
        else
            queueInLoop(func);
    }

    void insertChannel(Channel&) const;
    void removeChannel(Channel&) const;
    void updateChannel(Channel&) const;

    bool hasChannel(Channel&) const;

    bool isInLoopThread() const;
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
};

END_NAMESPACE