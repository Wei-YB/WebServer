#pragma once

#include <cstring>

#include "InetAddress.h"
#include "Channel.h"

#include <deque>
#include <memory>
#include <functional>

START_NAMESPACE

class EventLoop;


enum class ConnState  {
    ESTABLISHED,
    FIN_WAIT,
    CLOSE_WAIT,
};

class Connection : public std::enable_shared_from_this<Connection> {
    using MessageCallback =  std::function<void(std::shared_ptr<Connection>,char*,size_t)>;
public:
    Connection(EventLoop& loop, int fd, const InetAddress& localAddr, const InetAddress& peerAddr);

    ~Connection();

    void setMessageCallback(const MessageCallback& func){messageCallback_ = func;}

    void send(const std::string& str);

private:
    void handleRead();

    void handleWrite();

//     void close();
private:
    EventLoop& loop_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    Channel connChannel_;

    MessageCallback messageCallback_;

    char outputBuffer[2048];
    int outputPos;
};

END_NAMESPACE