#include "Connection.h"
#include "Logger.h"
#include <unistd.h>

#include "EventLoop.h"

USE_NAMESPACE

Connection::Connection(EventLoop& loop,
                       int fd,
                       const InetAddress& localAddr,
                       const InetAddress& peerAddr):loop_(loop),
localAddr_(localAddr),peerAddr_(peerAddr),connChannel_(loop_,fd) {
    connChannel_.setReadCallback([this]() { this->handleRead(); });
    connChannel_.setWriteCallback([this]() { this->handleWrite(); });
    connChannel_.enableReading();
}

Connection::~Connection() {
    LOG_INFO << "connection: " << connChannel_.fd() <<" from:"<<peerAddr_.toString()
    <<" to: "<<localAddr_.toString()<< " closed";
    loop_.runInLoop([this]() {this->connChannel_.remove(); });
}

void Connection::send(const std::string& str) {
    memcpy(outputBuffer, str.c_str(), str.size());
    outputPos = str.size();
    connChannel_.enableWriting();
}

void Connection::handleRead() {
    // read all available data from socket
    char buf[4096];

    while (true) {
        const auto ret = read(connChannel_.fd(), buf, sizeof buf);
        if (ret > 0) {
            LOG_TRACE << "read message from connection: " << connChannel_.fd() << " and length is " << ret;
            messageCallback_(shared_from_this(), buf, ret);
        }
        else if(ret == 0) {
            LOG_TRACE << "connection " << peerAddr_.toString() << " closed by peer";
            close();
            break;
        }
        else if (errno == EINTR)
            continue;
        else if (errno == EAGAIN) {
            LOG_INFO << "read would blocked on connection: " << connChannel_.fd();
            break;
        }
        else {
            LOG_ERROR << "read error on connection " << connChannel_.fd();
            break;
        }
    }
}

void Connection::handleWrite() {
    write(connChannel_.fd(),outputBuffer,outputPos);
    connChannel_.disableWriting();
    outputPos = 0;
    writeFinishCallback_(shared_from_this());
}


void Connection::close() {
    connChannel_.disableAll();
    shutdown(connChannel_.fd(), SHUT_RDWR);
    state_ = ConnState::CLOSED;
    closeCallback_(shared_from_this());
}
