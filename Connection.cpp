#include "Connection.h"

USE_NAMESPACE

Connection::Connection(EventLoop& loop,
                       int fd,
                       const InetAddress& localAddr,
                       const InetAddress& peerAddr):loop_(loop),
localAddr_(localAddr),peerAddr_(peerAddr),connChannel_(loop_,fd) {
    connChannel_.setReadCallback([this]() { this->handleRead(); });
    connChannel_.setWriteCallback([this]() { this->handleWrite(); });
    connChannel_.enableReadAndWrite();
}

Connection::~Connection() {
    connChannel_.disableAll();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
    connChannel_.remove();
}

void Connection::send(const std::string& str) {
    outputBuffer_.insert(outputBuffer_.end(),str.begin(),str.end());
}

void Connection::handleRead() {

    messageCallback_(shared_from_this(),)
}
