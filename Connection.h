#pragma once

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
    using MessageCallback =  std::function<void(std::shared_ptr<Connection>,std::deque<char>&)>;
public:
    Connection(EventLoop& loop, int fd, const InetAddress& localAddr, const InetAddress& peerAddr);

    ~Connection();

    void setMessageCallback(MessageCallback func){messageCallback_ = func;}

    void send(const std::string& str);

private:
    void handleRead();

    void handleWrite();

    void handleClose();
private:
    EventLoop& loop_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    Channel connChannel_;

    MessageCallback messageCallback_;


    std::deque<char> inputBuffer_;
    std::deque<char> outputBuffer_;
};

END_NAMESPACE