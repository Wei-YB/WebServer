#pragma once

#include "InetAddress.h"

START_NAMESPACE namespace socket {
int socket(int domain, int type, int protocol);

int nonBlockInetSocket();

void bind(int fd, const InetAddress& address);

void listen(int fd, int backlog);

int Accept(int fd, InetAddress& address);

void ShutdownWrite(int fd);

void setNoDelay(int fd, bool on);

int eventFd();

void Close(int fd);
}

class Buffer;

// must call close before destructor
class TCPSocket {
public:
    explicit TCPSocket(int fd, bool nonBlock = false);
    explicit TCPSocket(bool nonBlock);
    TCPSocket(TCPSocket&& sock) noexcept = default;
    TCPSocket(TCPSocket& sock) = default;
    TCPSocket& operator= (TCPSocket&& sock) noexcept = default;
    TCPSocket& operator= (TCPSocket& sock) = default;

    bool isValid() const { return fd_ > 0; }
    int fd() const { return fd_; };
    InetAddress& peerAddress() { return peer_; }
    const InetAddress& peerAddress() const { return peer_; }
    const InetAddress& hostAddress() const { return host_; }

    void setNonBlock() const;
    void listen(int backlog) const;
    void bind(const InetAddress& addr);
    void connect(const InetAddress& peer);
    TCPSocket accept() const;
    void shutdownWrite() const;
    void setNoDelay(bool on) const;

    void close();

    // TODO: try to use buffer to read and write
    // int read(Buffer& buffer);
    // int write(Buffer& buffer);


private:
    static TCPSocket newConnection(int fd, const InetAddress& peerAddr);

    static TCPSocket badSocket() {
        return TCPSocket(-1, false);
    }

    void initHost();

    char closed_;
    int fd_;
    InetAddress peer_;
    InetAddress host_;
};

END_NAMESPACE
