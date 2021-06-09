#include "TCPServer.h"

#include "base/Logger.h"
#include "Connection.h"

USE_NAMESPACE

TcpServer::TcpServer(uint16_t port, int workThread) :
    acceptor_(mainLoop_, port, [this](int             conn) { this->onAccept(conn); }),
    threadPool_(workThread, "ioThread", [](EventLoop* loop) { LOG_INFO << "thread init"; }) {}


void TcpServer::start() {
    LOG_INFO << "Tcp Server Start";
    threadPool_.start();
    // wtf
    acceptor_.listen(256);
    mainLoop_.loop();
    
}


void TcpServer::onAccept(int conn) {
    // get an event loop from thread pool
    auto* loop = threadPool_.getLoop();
    // create a connection and insert it into loop
    auto newConn = std::make_shared<Connection>(*loop, conn, acceptor_.hostAddress, acceptor_.peerAddress);

    // fixed, thread unsafe, messageCallback may called before correct set
    connections_[conn] = newConn;

    newConn->setConnectionCallback([this](auto conn) { this->connectionCallback_(conn);} );

    newConn->setMessageCallback([this](auto conn, auto buffer) { this->messageCallback_(conn, buffer); });

    newConn->setCloseCallback([this](std::shared_ptr<Connection> conn) {
        onClose(conn);
    });
    newConn->setWriteFinishCallback([this](std::shared_ptr<Connection> conn) {
        onWriteFinish(conn);
    });
    // newConn->establish();

    loop->runInLoop([newConn]() { newConn->establish(); });
}

void TcpServer::onClose(std::shared_ptr<Connection> conn) {
    const auto fd = conn->fd();
    LOG_TRACE << "remove connection:" << fd << " from connMaps";
    // TODO£º move connection erase in ioThread, not main Thread
    mainLoop_.queueInLoop([this,fd] {
        this->connections_.erase(fd);
    });
    if (closeCallback_)
        closeCallback_(conn);
}

void TcpServer::onWriteFinish(std::shared_ptr<Connection> conn) {
    if (writeFinishCallback_)
        writeFinishCallback_(conn);
}
