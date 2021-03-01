#include "Acceptor.h"
#include "base/Logger.h"
#include "Socket.h"
#include "EventLoop.h"


#include <unistd.h>
#include <strings.h>
#include <cerrno>
USE_NAMESPACE

Acceptor::Acceptor(EventLoop& loop, uint16_t port) : fd_(NonblockInetSocket()),
                                                     listenChannel_(loop, fd_),
                                                     loop_(loop),
                                                     acceptCallback_([](int fd) { shutdown(fd, 2); }),
                                                     address_(InetAddress::listenAddress(port)),
                                                     isListening_(false),
                                                     hostAddress("127.0.0.1", port), peerAddress("0.0.0.0", 0) {
    if (fd_ < 0) {
        LOG_SYSFATAL << "socket create error ";
    }

    // sockaddr_in addr{};
    // bzero(&addr, sizeof addr);
    // addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // addr.sin_port = htons(static_cast<uint16_t>(port));
    // addr.sin_family = AF_INET;

    Bind(fd_, address_);

    listenChannel_.enableReading();
    listenChannel_.setReadCallback([this]() {this->onAccept(); });

    LOG_TRACE << "Acceptor created, fd = " << fd_;
}

Acceptor::~Acceptor() {
    listenChannel_.disableAll();
    listenChannel_.remove();
    ::close(fd_);
}


void Acceptor::listen(int backlog) const {
    if (isListening_) {
        LOG_FATAL << "socket already in listening";
    }
    isListening_ = true;
    Listen(fd_, backlog);
}

void Acceptor::acceptCallback(const std::function<void(int)>& func) {
    acceptCallback_ = func;
}

void Acceptor::onAccept() const {
    while (true) {
        InetAddress peerAddress{};
        const auto conn = Accept(fd_, peerAddress);
        if (conn > 0) {
            LOG_INFO << "new connection from " << peerAddress.toString();
            LOG_TRACE << "new connection fd = " << conn;
            this->peerAddress = peerAddress;
            acceptCallback_(conn);
        }
        else {
            break;
        }
    }
}
