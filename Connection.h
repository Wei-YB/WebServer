#pragma once

#include "InetAddress.h"
#include "Channel.h"
#include "Buffer.h"
#include <memory>
#include <functional>

START_NAMESPACE

class EventLoop;


enum class ConnState {
    ESTABLISHED,
    CLOSED
};

class Connection : public std::enable_shared_from_this<Connection> {
    using MessageCallback = std::function<void(std::shared_ptr<Connection>, Buffer&)>;
    using WriteFinishCallback = std::function<void (std::shared_ptr<Connection>)>;
    using CloseCallback = std::function<void(std::shared_ptr<Connection>)>;
public:
    Connection(EventLoop& loop, int fd, const InetAddress& localAddr, const InetAddress& peerAddr);

    ~Connection();

    bool established() const {
        return state_ == ConnState::ESTABLISHED;
    }

    void setMessageCallback(const MessageCallback& func) { messageCallback_ = func; }

    void setCloseCallback(const CloseCallback& func) { closeCallback_ = func; }

    void setWriteFinishCallback(const WriteFinishCallback& func) {
        writeFinishCallback_ = func;
    }

    void send(const std::string& str);

    int fd() const {
        return connChannel_.fd();
    }

    // insert into poller and enable read
    void connected();

    // TODO: Not implemented
    //     void shutdown();

// close connection
    void close();

    

private:
    void handleRead();

    void handleWrite();

    // TODO handleError 
    void handleError();

    
private:
    EventLoop&  loop_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    Channel connChannel_;

    MessageCallback     messageCallback_;
    CloseCallback       closeCallback_;
    WriteFinishCallback writeFinishCallback_;


    ConnState state_;

    // data need to send to peer
    Buffer outputBuffer_;

    // data get from peer
    Buffer inputBuffer_;
};

END_NAMESPACE
