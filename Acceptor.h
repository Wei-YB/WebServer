#pragma once

#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
START_NAMESPACE

class EventLoop;

class Acceptor {
public:
    // TODO: update AcceptCallback to use TCPSocket
    using AcceptCallback = std::function<void(int)>;

    Acceptor(EventLoop& loop, uint16_t port, const AcceptCallback& callback = [](int fd) { shutdown(fd, 2); });
    ~Acceptor();

    void listen(int backlog) const;

    void acceptCallback(const AcceptCallback&);

    void onAccept() const;
    
    const Channel& channel() const { return listenChannel_; }
    Channel& channel() { return listenChannel_; }

private:

    TCPSocket socket_;
    Channel listenChannel_;

    EventLoop& loop_;

    std::function<void(int)> acceptCallback_;

    InetAddress address_;

    mutable bool isListening_;
};

END_NAMESPACE