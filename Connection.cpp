#include "Connection.h"
#include "Logger.h"
#include <unistd.h>

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
    connChannel_.disableAll();
    connChannel_.remove();
    shutdown(connChannel_.fd(),SHUT_RDWR);
}

void Connection::send(const std::string& str) {
    memcpy(outputBuffer, str.c_str(), str.size());
    outputPos = str.size();
    connChannel_.enableWriting();
}

void Connection::handleRead() {
    // read all available data from socket
    char buf[2048];

    while (const auto ret = read(connChannel_.fd(), buf, sizeof buf)) {
        if (ret > 0) {
            messageCallback_(shared_from_this(), buf, ret);
        }
        else if (errno == EINTR)
            continue;
        else if (errno == EAGAIN)
            break;
        else {
            LOG_ERROR << "read error on connection " << connChannel_.fd();
        }
    }
}

void Connection::handleWrite() {
    write(connChannel_.fd(),outputBuffer,outputPos);
    connChannel_.disableWriting();
    outputPos = 0;
}

