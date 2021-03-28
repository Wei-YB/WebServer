#include "TCPServer.h"

#include "Logger.h"
#include "Connection.h"

USE_NAMESPACE

TcpServer::TcpServer(int port, int workThread) :
    acceptor_(mainLoop_, port),
    threadPool_(workThread, "ioThread", [](EventLoop* loop) { LOG_INFO << "thread init"; }) {}


void TcpServer::start() {
    acceptor_.acceptCallback([this](int conn) {
        this->onAccept(conn);
    });

    mainLoop_.loop();
}


void TcpServer::onAccept(int conn) {
    auto* loop = threadPool_.getLoop();
    auto  newConn = std::make_shared<Connection>(*loop, conn, acceptor_.hostAddress, acceptor_.peerAddress);


    // fixed, thread unsafe, messageCallback may called before correct set
    connections_[conn] = newConn;
    newConn->setMessageCallback([this](auto conn, auto buffer) { this->messageCallback_(conn, buffer); });

    newConn->setCloseCallback([this](std::shared_ptr<Connection> conn) {
        onClose(conn);
    });
    newConn->setWriteFinishCallback([this](std::shared_ptr<Connection> conn) {
        onWriteFinish(conn);
    });
    newConn->connected();
}
