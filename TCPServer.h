#pragma once

#include "EventLoopThreadPool.h"
#include "Acceptor.h"
#include "Buffer.h"
#include "EventLoop.h"


START_NAMESPACE
class Connection;

class TcpServer {

public:

    using MessageCallback = std::function<void(std::shared_ptr<Connection>, Buffer&)>;
    using ConnectionCallback = std::function<void(std::shared_ptr<Connection>)>;
    using CloseCallback = std::function<void(std::shared_ptr<Connection>)>;
    using WriteFinishCallback = std::function<void(std::shared_ptr<Connection>)>;
    using ConnectionMaps = std::unordered_map<int, std::shared_ptr<Connection>>;


    TcpServer(int port, int workThread);


    void start();

private:

    void onAccept(int conn);
    void onClose(std::shared_ptr<Connection> conn);
    void onWriteFinish(std::shared_ptr<Connection> conn);

    EventLoop           mainLoop_;
    Acceptor            acceptor_;
    EventLoopThreadPool threadPool_;
    MessageCallback     messageCallback_;
    ConnectionCallback  connectionCallback_;
    CloseCallback       closeCallback_;
    WriteFinishCallback writeFinishCallback_;
    ConnectionMaps      connections_;


};

END_NAMESPACE