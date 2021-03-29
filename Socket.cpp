#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/eventfd.h>

#include "Socket.h"

#include <unistd.h>

#include "Logger.h"


USE_NAMESPACE


int webServer::Socket(int domain, int type, int protocol) {
    auto fd = ::socket(domain, type, protocol);
    if (fd < 0)
        LOG_SYSFATAL << "socket fault";
    return fd;
}

int webServer::NonblockInetSocket() {
    return Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
}

void webServer::Bind(int fd, InetAddress& addr) {
    if (::bind(fd, addr.address(), sizeof(sockaddr_in)) < 0) {
        LOG_SYSFATAL << "bind fault";
    }
}

void webServer::Listen(int fd, int backlog) {
    if (::listen(fd, backlog) < 0)
        LOG_SYSFATAL << "listen fault";
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

int webServer::Accept(int fd, InetAddress& address) {
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
    if ( (currentErrno == EWOULDBLOCK) ||( currentErrno == EAGAIN)) {
        LOG_TRACE << "accept will be blocked";
        return 0;
    }
    LOG_SYSFATAL << "accept fault";
    return -1;
}

void webServer::ShutdownWrite(int fd) {
    auto ret = ::shutdown(fd, SHUT_WR);
    if(ret < 0) {
        LOG_SYSERR << "socket shutdown write";
    }
}

void webServer::setNoDelay(int fd, bool on) {
    int optval = on;
    ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

int webServer::eventFd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG_SYSFATAL << "Failed in eventfd";
    }
    return evtfd;
}

void webServer::Close(int fd) {
    ::close(fd);
}




