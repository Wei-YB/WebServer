#pragma once


#include <functional>
#include "base/Util.h"

START_NAMESPACE

class EventLoop;

class Channel {
public:
    using EventCallback = std::function<void()>;

    Channel(EventLoop& loop, int fd);
    Channel(const Channel& channel) = default;

    ~Channel() = default;

    void handleEvent();

    void setReadCallback(const EventCallback& callback) { readCallback_ = callback; }
    void setWriteCallback(const EventCallback& callback) { writeCallback_ = callback; }
    void setErrorCallback(const EventCallback& callback) { errorCallback_ = callback; }
    void setCloseCallback(const EventCallback& callback) { closeCallback_ = callback; }

    [[nodiscard]]
    int fd() const { return fd_; }

    [[nodiscard]]
    int event() const { return event_; }

    int& actionEvents() { return actionEvents_; }

    [[nodiscard]]
    const int& actionEvents() const { return actionEvents_; }

    [[nodiscard]]
    bool isNoneEvent() const { return event_; }

    void remove();

    void enableReading();
    void enableWriting();
    void enableReadAndWrite();

    void disableReading();
    void disableWriting();
    void disableAll();

    [[nodiscard]]
    bool isReading() const;
    [[nodiscard]]
    bool isWriting() const;


    [[nodiscard]] // format: for logging
    std::string toString() const { return eventToString(fd_, event_); }

    static std::string eventToString(int fd, int event);

private:

    void update();

    EventLoop& loop_;

    const int fd_;

    int event_;
    int actionEvents_;

    bool eventHandling_;


    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
    
};

END_NAMESPACE
