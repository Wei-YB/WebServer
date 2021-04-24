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
}

Connection::~Connection() {
    LOG_INFO << "connection: " << connChannel_.fd() <<" from:"<<peerAddr_.toString()
    <<" to: "<<localAddr_.toString()<< " closed";
    // loop_.runInLoop([this]() {this->connChannel_.remove(); });
}

void Connection::send(const std::string& str) {
    outputBuffer_.write(str.c_str(), str.size());
    // memcpy(outputBuffer, str.c_str(), str.size());
    connChannel_.enableWriting();
}

void Connection::handleRead() {
    // read all available data from socket
    // change the buf to inputBuffer_
    char buf[4096];

    while (true) {
        // todo: use iovec to read data direct into buffer 
        const auto ret = read(connChannel_.fd(), buf, sizeof buf);
        if (ret > 0) {
            inputBuffer_.write(buf, ret);
            LOG_TRACE << "read message from connection: " << connChannel_.fd() << " and length is " << ret;
            // use outputBuffer instead of buf
            messageCallback_(shared_from_this(), inputBuffer_);
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
            close();
            break;
        }
    }
}

void Connection::handleWrite() {
    // fix use outputBuffer_ instead of buffer
    // EPOLL edge trigger so need to write until EAGAIN or outputBuffer empty
    for (;;) {
        const auto writtenSize = write(connChannel_.fd(), outputBuffer_.peek(), outputBuffer_.size());
        if (writtenSize > 0) {
            outputBuffer_.consume(writtenSize);
        }
        if (outputBuffer_.empty()) {
            connChannel_.disableWriting();
            LOG_TRACE << " all write buffer sent";
            if (writeFinishCallback_)
                loop_.queueInLoop([this]() {
                    this->writeFinishCallback_(shared_from_this());
                });
            break;
        }
        if (writtenSize == -1) {            // handle error
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                LOG_TRACE << "keep write will be block, waiting for next write event";
            }
            else if (errno == EINTR) {
                continue;
            }
            else {
                close();
                LOG_ERROR << "error handleWrite in connection: " << connChannel_.fd();
            }
        }
    }
}

void Connection::handleError() {
    close();
}


void Connection::connected() {

    connChannel_.enableReading();
    state_ = ConnState::ESTABLISHED;

}

void Connection::close() {
    connChannel_.disableAll();
    connChannel_.remove();
    shutdown(connChannel_.fd(), SHUT_RDWR);
    state_ = ConnState::CLOSED;
    loop_.queueInLoop([this]() {
        closeCallback_(shared_from_this());
    });
}
