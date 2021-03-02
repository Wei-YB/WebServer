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
    CLOSED
};

class Connection : public std::enable_shared_from_this<Connection> {
    using MessageCallback =  std::function<void(std::shared_ptr<Connection>,char*,size_t)>;
    using WriteFinishCallback = std::function<void (std::shared_ptr<Connection>)>;
    using CloseCallback = std::function<void(std::shared_ptr<Connection>)>;
public:
    Connection(EventLoop& loop, int fd, const InetAddress& localAddr, const InetAddress& peerAddr);

    ~Connection();

    bool established() const {
        return state_ == ConnState::ESTABLISHED;
    }

    void setMessageCallback(const MessageCallback& func){messageCallback_ = func;}

    void setCloseCallback(const CloseCallback& func) { closeCallback_ = func; }

    void setWriteFinishCallback(const WriteFinishCallback& func) {
        writeFinishCallback_ = func;
    }

    void send(const std::string& str);

    int fd() const {
        return connChannel_.fd();
    }

// TODO: Not implemented
//     void shutdown();

private:
    void handleRead();

    void handleWrite();

    // TODO handleError 
    // void handleError();

    // close connection
    void close();
private:
    EventLoop& loop_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    Channel connChannel_;

    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
    WriteFinishCallback writeFinishCallback_;


    ConnState state_;

    char outputBuffer[2048];
    int outputPos;
};

END_NAMESPACE