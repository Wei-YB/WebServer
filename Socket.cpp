#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "Socket.h"
#include "base/Logger.h"


USE_NAMESPACE


int socket::socket(int domain, int type, int protocol) {
    auto fd = ::socket(domain, type, protocol);
    if (fd < 0)
        LOG_SYSFATAL << "socket fault";
    return fd;
}

int socket::nonBlockInetSocket() {
    return socket::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
}

void socket::bind(int fd, const InetAddress& addr) {
    if (::bind(fd, addr.address(), sizeof(sockaddr_in)) < 0) {
        LOG_SYSFATAL << "bind fault";
    }
}

void socket::listen(int fd, int backlog) {

}

static void setNonBlock(int fd) {
    // nonblock
    auto flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
        LOG_SYSFATAL << "get file flag fail";
    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) < 0)
        LOG_SYSFATAL << "set nonblock fail";

    // close-on-exec
    flag = ::fcntl(fd, F_GETFD, 0);
    flag |= O_CLOEXEC;
    if (fcntl(fd, F_SETFD, flag) < 0)
        LOG_SYSFATAL << "set file flag fail";

    if (fcntl(fd, F_SETFD, O_CLOEXEC) < 0)
        LOG_SYSFATAL << "set close-on-exec fail";

}

int socket::Accept(int fd, InetAddress& address) {
    socklen_t len = sizeof(sockaddr_in);
    const auto conn = ::accept(fd, address.address(), &len);
    auto currentErrno = errno;
    if (conn > 0) {
        setNonBlock(conn);
        return conn;
    }
    if (currentErrno == EINTR) {
        return Accept(fd, address);
    }
    if ((currentErrno == EWOULDBLOCK) || (currentErrno == EAGAIN)) {
        LOG_TRACE << "accept will be blocked";
        return 0;
    }
    LOG_SYSFATAL << "accept fault";
    return -1;
}

void socket::ShutdownWrite(int fd) {
    auto ret = ::shutdown(fd, SHUT_WR);
    if (ret < 0) {
        LOG_SYSERR << "socket shutdown write";
    }
}


int socket::eventFd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG_SYSFATAL << "Failed in eventfd";
    }
    return evtfd;
}

void socket::Close(int fd) {
    ::close(fd);
}

TCPSocket::TCPSocket(int fd, bool nonBlock) : fd_(fd) {
    if (nonBlock) {
        setNonBlock();
    }
}

TCPSocket::TCPSocket(bool nonBlock) :
    TCPSocket(::socket(AF_INET, SOCK_STREAM, 0)) {
    if (nonBlock)
        setNonBlock();
}

void TCPSocket::setNonBlock() const {
    auto flag = fcntl(fd_, F_GETFL, 0);
    if (flag == -1)
        LOG_SYSFATAL << "get file flag fail";
    flag |= O_NONBLOCK;
    if (fcntl(fd_, F_SETFL, flag) < 0)
        LOG_SYSFATAL << "set nonblock fail";

    // close-on-exec
    flag = ::fcntl(fd_, F_GETFD, 0);
    flag |= O_CLOEXEC;
    if (fcntl(fd_, F_SETFD, flag) < 0)
        LOG_SYSFATAL << "set file flag fail";

    if (fcntl(fd_, F_SETFD, O_CLOEXEC) < 0)
        LOG_SYSFATAL << "set close-on-exec fail";
}

void TCPSocket::listen(int backlog) const {
    if (::listen(fd_, backlog) < 0)
        LOG_SYSFATAL << "listen fault";
}

void TCPSocket::bind(const InetAddress& addr) {
    host_ = addr;
    if (::bind(fd_, host_.address(), sizeof(sockaddr_in)) < 0) {
        LOG_SYSFATAL << "bind fault";
    }
}

void TCPSocket::connect(const InetAddress& peer) {
    if (::connect(fd_, peer_.address(), sizeof peer) < 0) {
        LOG_SYSFATAL << "connect fault";
    }
    peer_ = peer;
    initHost();
}

TCPSocket TCPSocket::accept() const {
    InetAddress peerAddr;
    socklen_t len = sizeof(sockaddr_in);
    const auto newConnFd = ::accept(fd_, peerAddr.address(), &len);
    const auto currentErrno = errno;
    if (newConnFd > 0) {
        return newConnection(newConnFd, peerAddr);
    }
    if (currentErrno == EAGAIN) {
        LOG_TRACE << "accept will be blocked";
    }
    else {
        LOG_SYSFATAL << "accept fault";
    }
    return badSocket();
}

void TCPSocket::shutdownWrite() const {
    if (shutdown(fd_, SHUT_WR) < 0) {
        LOG_SYSERR << "socket shutdown write";
    }
}

void TCPSocket::setNoDelay(bool on) const {
    int optval = on;
    ::setsockopt(fd_,
                 IPPROTO_TCP, TCP_NODELAY,
                 &optval, static_cast<socklen_t>(sizeof optval));
}

void TCPSocket::close() {
    ::close(fd_);
    closed_ = 1;
}

TCPSocket TCPSocket::newConnection(int fd, const InetAddress& peerAddr) {
    TCPSocket conn(fd, false);
    conn.peer_ = peerAddr;
    return conn;
}

void TCPSocket::initHost() {
    socklen_t len = sizeof(sockaddr_in);
    if (getsockname(fd_, host_.address(), &len) < 0) {
        LOG_SYSERR << "getsockname error";
    }
}
