#pragma once

#include "Channel.h"
#include "InetAddress.h"

START_NAMESPACE

class EventLoop;

class Acceptor {
public:

    using AcceptCallback = std::function<void(int)>;

    Acceptor(EventLoop& loop, uint16_t port, const AcceptCallback& callback = [](int fd) { shutdown(fd, 2); });
    ~Acceptor();

    void listen(int backlog) const;

    void acceptCallback(const AcceptCallback&);

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