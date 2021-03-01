#include "Channel.h"

#include <sys/epoll.h>

#include "Logger.h"
#include "EventLoop.h"
USE_NAMESPACE

constexpr int NoneEvent = 0;
constexpr int ReadEvent = EPOLLIN | EPOLLPRI;
constexpr int WriteEvent = EPOLLOUT;

Channel::Channel(EventLoop& loop, int fd) : loop_(loop), fd_(fd), event_(0), actionEvents_(0), eventHandling_(false) {}

void Channel::enableReading() {
    event_ |= ReadEvent;
    update();
}

void Channel::enableWriting() {
    event_ |= WriteEvent;
    update();
}

void Channel::enableReadAndWrite() {
    event_ |= WriteEvent;
    event_ |= ReadEvent;
    update();
}


void Channel::disableReading() {
    event_ &= ~ReadEvent;
    update();
}

void Channel::disableWriting() {
    event_ &= ~WriteEvent;
    update();
}

void Channel::disableAll() {
    event_ = NoneEvent;
    update();
}

bool Channel::isReading() const {
    return actionEvents_ & ReadEvent;
}

bool Channel::isWriting() const {
    return actionEvents_ & WriteEvent;
}

void Channel::remove() {
    loop_.removeChannel(*this);
}


void Channel::handleEvent() {
    eventHandling_ = true;
    LOG_TRACE << eventToString(fd_, actionEvents_);

    if (actionEvents_ & EPOLLERR)
        if (errorCallback_)
            errorCallback_();
    if (actionEvents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        if (readCallback_)
            readCallback_();
    if (actionEvents_ & EPOLLOUT)
        if (writeCallback_)
            writeCallback_();
    eventHandling_ = false;
}

std::string Channel::eventToString(int fd, int event) {
    std::string str = "fd = " + std::to_string(fd) + ", event = ";
    if (event & EPOLLIN)
        str += "EPOLLIN ";
    if (event & EPOLLOUT)
        str += "EPOLLOUT ";
    if (event & EPOLLERR)
        str += "EPOLLERR ";
    if (event & EPOLLPRI)
        str += "EPOLLPRI ";
    if (event & EPOLLHUP)
        str += "EPOLLHUP ";
    if (event & EPOLLRDHUP)
        str += "EPOLLRDHUP ";
    return str;
}

void Channel::update() {
    if (loop_.hasChannel(*this)) {
        loop_.updateChannel(*this);
    }
    else
        loop_.insertChannel(*this);
}
