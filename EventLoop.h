#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include <queue>


#include "Timestamp.h"

START_NAMESPACE

class Poller;
class Channel;

class EventLoop {
public:

    using Functor = std::function<void()>;

    EventLoop();
    void loop();
    void stop();

    void insertChannel(Channel&) const;
    void removeChannel(Channel&) const;
    void updateChannel(Channel&) const;

    bool hasChannel(Channel&) const;

private:
    bool running_;
    std::atomic<bool> stop_;
    int currentThread_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;

    std::unique_ptr<Poller> poller_;

    std::queue<Functor> funcQueue_;

    std::vector<Channel*> activeChannels_;
    Channel* currentActiveChannel_;
};

END_NAMESPACE