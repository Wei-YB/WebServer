#include "Connection.h"
#include "Logger.h"
#include <unistd.h>

#include "Socket.h"
#include "EventLoop.h"

USE_NAMESPACE


static std::string toString(ConnectionState state) {
    switch (state) {
        case ConnectionState::established:
            return "established";
        case ConnectionState::establishing:
            return "establishing";
        case ConnectionState::closing:
            return "closing";
        case ConnectionState::closed:
            return "closed";
    }
}

Connection::Connection(EventLoop&         loop,
                       int                fd,
                       const InetAddress& localAddr,
                       const InetAddress& peerAddr): loop_(loop),
                                                     localAddr_(localAddr),
                                                     peerAddr_(peerAddr),
                                                     channel_(loop_, fd),
                                                     reading_(false),
                                                     state_(ConnectionState::establishing) {
    channel_.setReadCallback([this]() { this->handleRead(); });
    channel_.setWriteCallback([this]() { this->handleWrite(); });
    channel_.setCloseCallback([this]() { this->handleClose(); });
    channel_.setErrorCallback([this]() { this->handleError(); });
}

Connection::~Connection() {
    LOG_INFO << "connection: " << channel_.fd() <<" from:"<<peerAddr_.toString()
    <<" to: "<<localAddr_.toString()<< " closed";
    // loop_.runInLoop([this]() {this->channel_.remove(); });
}

const InetAddress& Connection::localAddress() const { return localAddr_; }
const InetAddress& Connection::peerAddress() const { return peerAddr_; }

bool Connection::established() const {
    return state_ == ConnectionState::established;
}

ConnectionState Connection::connectionState() const { return state_; }

void Connection::send(const std::string& message) {
    send(message.c_str(), message.size());  
}

void Connection::shutdown() {
    if (state_ == ConnectionState::established) {
        state_ = ConnectionState::closing;
        loop_.runInLoop([this]() { this->shutdownInLoop(); });
    }
}

void Connection::forceClose() {
    if (state_ == ConnectionState::established || state_ == ConnectionState::establishing) {
        state_    = ConnectionState::closing;
        // todo: optimize the pass of shared_ptr 
        auto conn = shared_from_this();
        loop_.queueInLoop([conn]() {
            conn->forceCloseInLoop();
        });
    }
}

void Connection::setTcpNoDelay(bool on) {
    setNoDelay(fd(), on);
}

void Connection::enableRead() {
    loop_.runInLoop([this]() {
        this->enableReadInLoop();
    });
}

void Connection::disableRead() {
    loop_.runInLoop([this]() {
        this->disableReadInLoop();
    });
}

bool Connection::isRead() const {
    return reading_;
}

void Connection::setConnectionCallback(const ConnectionCallback& func) {
    connectionCallback_ = func;
}

void Connection::setMessageCallback(const MessageCallback& func) {
    messageCallback_ = func;
}

void Connection::setCloseCallback(const CloseCallback& func) {
    closeCallback_ = func;
}

void Connection::setWriteFinishCallback(const WriteFinishCallback& func) {
    writeFinishCallback_ = func;
}

void Connection::handleRead() {
    // read all available data from socket
    // change the buf to inputBuffer_
    char buf[4096];

    while (true) {
        // todo: use iovec to read data direct into buffer 
        const auto ret = read(channel_.fd(), buf, sizeof buf);
        if (ret > 0) {
            inputBuffer_.write(buf, ret);
            LOG_TRACE << "read message from connection: " << channel_.fd() << " and length is " << ret;
            // use outputBuffer instead of buf
            messageCallback_(shared_from_this(), inputBuffer_);
        }
        else if(ret == 0) {
            LOG_TRACE << "connection " << peerAddr_.toString() << " closed by peer";
            handleClose();
            break;
        }
        else if (errno == EINTR)
            continue;
        else if (errno == EAGAIN) {
            LOG_TRACE << "read would blocked on connection: " << channel_.fd();
            break;
        }
        else {
            LOG_SYSERR << "read error on connection " << channel_.fd();
            handleError();
            break;
        }
    }
}

void Connection::handleWrite() {
    // fix use outputBuffer_ instead of buffer
    // EPOLL edge trigger so need to write until EAGAIN or outputBuffer empty
    for (;;) {
        // fixed : write an disconnect connection may cause sigpipe
        const auto writtenSize = write(channel_.fd(), outputBuffer_.peek(), outputBuffer_.size());
        if (writtenSize > 0) {
            outputBuffer_.consume(writtenSize);
        }
        if (outputBuffer_.empty()) {
            channel_.disableWriting();
            LOG_TRACE << " all write buffer sent";
            if (writeFinishCallback_)
                loop_.queueInLoop([this]() {
                    this->writeFinishCallback_(shared_from_this());
                });
            break;
        }
        if (writtenSize == -1) {            // handle error
            // fixed: EAGAIN == EWOULDBLOCK
            if (errno == EAGAIN) {
                LOG_TRACE << "keep write will be block, waiting for next write event";
                continue;
            }
            else if (errno == EINTR) {
                continue;
            }
            else {
                LOG_SYSERR << "error handleWrite in connection: " << channel_.fd();
                break;
            }
        }
    }
}

void Connection::handleClose() {
    loop_.assertInLoopThread();
    LOG_TRACE << "fd = " << fd() << "state = " << toString(state_);
    assert(state_ == ConnectionState::established || state_ == ConnectionState::closing);

    state_ = ConnectionState::closed;
    channel_.disableAll();

    connectionCallback_(shared_from_this());
    closeCallback_(shared_from_this());
}


// we should not close a connection in handle error
// the connection will be closed on handle read
void Connection::handleError() {
    int  err = 0;
    auto len = static_cast<socklen_t>(sizeof err);
    ::getsockopt(fd(), SOL_SOCKET, SO_ERROR, &err, &len);
    LOG_ERROR << "TcpConnection::handleError [" << peerAddr_.toString()
        << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Connection::shutdownInLoop() {
    loop_.assertInLoopThread();
    if (!channel_.isWriting()) {
        ShutdownWrite(this->fd());
    }
}

void Connection::forceCloseInLoop() {
    loop_.assertInLoopThread();
    if (state_ == ConnectionState::established || state_ == ConnectionState::closing)
        handleClose();
}

void Connection::enableReadInLoop() {
    
    if (!reading_ || !channel_.isReading())
        channel_.enableReading();
    reading_ = true;
}

void Connection::disableReadInLoop() {
    if (reading_ || channel_.isReading())
        channel_.disableReading();
    reading_ = false;
}


void Connection::established() {
    loop_.assertInLoopThread();
    assert(state_ == ConnectionState::establishing);
    LOG_DEBUG << "connection established, fd = " << fd();
    state_ = ConnectionState::established;
    channel_.enableReading();

    connectionCallback_(shared_from_this());
}

void Connection::closed() {
    loop_.assertInLoopThread();
    if(state_ == ConnectionState::established) {
        state_ = ConnectionState::closed;
        channel_.disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_.remove();
    ::close(fd());
}
