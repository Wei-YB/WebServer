#include <unistd.h>
#include <strings.h>
#include <cerrno>

#include "Acceptor.h"
#include "base/Logger.h"
#include "EventLoop.h"

USE_NAMESPACE

Acceptor::Acceptor(EventLoop&            loop,
                   uint16_t              port,
                   const AcceptCallback& callback) : socket_(TCPSocket(true)),
                                                     listenChannel_(loop, socket_.fd()),
                                                     loop_(loop),
                                                     acceptCallback_(callback),
                                                     address_(InetAddress::listenAddress(port)),
                                                     isListening_(false) {
    socket_.bind(address_);

    listenChannel_.enableReading();
    listenChannel_.setReadCallback([this]() { this->onAccept(); });

    LOG_TRACE << "Acceptor created, fd = " << socket_.fd();
}

Acceptor::~Acceptor() {
    listenChannel_.disableAll();
    listenChannel_.remove();
    socket_.close();
}


void Acceptor::listen(int backlog) const {
    if (isListening_) {
        LOG_FATAL << "socket already in listening";
    }
    isListening_ = true;
    socket_.listen(backlog);
}

void Acceptor::acceptCallback(const std::function<void(int)>& func) {
    acceptCallback_ = func;
}

void Acceptor::onAccept() const {
    while (true) {
        // InetAddress peerAddr{};
        auto conn = socket_.accept();
        if (conn.isValid()) {
            LOG_INFO << "new connection from " << conn.peerAddress().toString()
            << "to " << conn.hostAddress().toString() << " and fd = " << conn.fd();
            conn.setNonBlock();
            acceptCallback_(conn.fd());
        }
        else {
            break;
        }
    }
}
