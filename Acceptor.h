#pragma once

#include "Channel.h"
#include "InetAddress.h"

START_NAMESPACE

class EventLoop;

class Acceptor {
public:
    Acceptor(EventLoop& loop, uint16_t port);
    ~Acceptor();

    void listen(int backlog) const;

    void acceptCallback(const std::function<void(int)>&);

    void onAccept() const;
    
    const Channel& channel() const { return listenChannel_; }
    Channel& channel() { return listenChannel_; }

    mutable InetAddress peerAddress;
    InetAddress hostAddress;

private:
    int fd_;
    Channel listenChannel_;

    EventLoop& loop_;

    std::function<void(int)> acceptCallback_;

    InetAddress address_;

    

    mutable bool isListening_;
};

END_NAMESPACE