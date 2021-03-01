#pragma once

#include "InetAddress.h"
#include "Channel.h"


START_NAMESPACE

class EventLoop;

class Connection {
public:
    Connection(EventLoop& loop, int fd, const InetAddress& localAddr, const InetAddress& peerAddr);


private:
    int fd_;
    EventLoop& loop_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    Channel connChannel_;

    
};

END_NAMESPACE