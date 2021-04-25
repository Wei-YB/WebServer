#pragma once
#include <any>
#include <memory>
#include <functional>

#include "InetAddress.h"
#include "Channel.h"
#include "Buffer.h"


START_NAMESPACE

class EventLoop;


enum class ConnectionState {
    establishing,
    established,
    closing,
    closed,
};


class Connection final : public std::enable_shared_from_this<Connection> {

    using ConnectionPtr = std::shared_ptr<Connection>;

    using MessageCallback = std::function<void(ConnectionPtr, Buffer&)>;

    using ConnectionCallback = std::function<void(ConnectionPtr)>;
    using WriteFinishCallback = ConnectionCallback;
    using CloseCallback = WriteFinishCallback;
public:
    Connection(EventLoop& loop, int fd, const InetAddress& localAddr, const InetAddress& peerAddr);

    ~Connection();

    const InetAddress& localAddress() const;
    const InetAddress& peerAddress() const;

    bool established() const;
    ConnectionState connectionState() const;

    void send(const std::string& message);

    void send(std::string&& message) {
        send(message);
    }

    void send(const char* message, size_t len) {
        outputBuffer_.write(message, len);
        channel_.enableWriting();
    }
    // TODO finish buffer with send
    // void send(Buffer&& message);
    // void send(const Buffer& message);

    void shutdown();
    void forceClose();
    void setTcpNoDelay(bool on) const;

    void enableRead();

    void disableRead();

    bool isRead() const;

    // fixed : the callback function maybe reused, don't use r-value ref
    void setConnectionCallback(const ConnectionCallback& func);

    void setMessageCallback(const MessageCallback& func);

    void setCloseCallback(const CloseCallback& func);

    void setWriteFinishCallback(const WriteFinishCallback& func);

    int fd() const {
        return channel_.fd();
    }

    std::any& getContext() { return context_; }

    template <typename T>
    void setContext(T&& value) {
        context_ = std::make_any<T>(std::forward<T>(value));
    }


    // only called when connection hand to ioLoop
    void establish();
    // only called when connection destroyed
    void close();  

private:
    void handleRead();
    void handleWrite();
    void handleClose();

    // TODO handleError 
    void handleError();


    // TODO
    // void sendInLoop(const char* message, size_t len);
    void shutdownInLoop();

    void forceCloseInLoop();

    void enableReadInLoop();
    void disableReadInLoop();

private:
    EventLoop&  loop_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    Channel channel_;
    
    MessageCallback     messageCallback_     = [](ConnectionPtr ptr, Buffer& buf) { buf.consumeAll(); };
    ConnectionCallback  connectionCallback_  = [](ConnectionPtr ptr) {};
    CloseCallback       closeCallback_       = [](ConnectionPtr ptr) { ptr->close(); };
    WriteFinishCallback writeFinishCallback_ = [](ConnectionPtr ) {};
    
    ConnectionState state_;

    Buffer outputBuffer_; // data need to send to peer
    Buffer inputBuffer_;  // data get from peer

    std::any context_;

    bool reading_;
};

END_NAMESPACE
