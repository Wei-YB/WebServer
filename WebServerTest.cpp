// WebServerTest.cpp: 定义应用程序的入口点。
//

#include "WebServerTest.h"

#include <strings.h>
#include <cstring>


#include "HTTPParse.h"
#include "Poller.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

#include "AsyncLogging.h"

USE_NAMESPACE
using namespace std;

// EventLoop* ioLoop = nullptr;

AsyncLogging* asyncLog;

void outPut(const char* str, size_t len) {
    asyncLog->append(str, len);
}


int main() {
    //TODO use async log!
    //TODO EventLoopThreadPoll

    AsyncLogging log("/home/csi/webServer", 1000 * 1000 * 500);
    log.start();
    asyncLog = &log;

    

    Logger::setLogLevel(Logger::LogLevel::INFO);

    // EventLoop* ioLoop = nullptr;
    // Thread ioThread([&ioLoop]() {
    //     ioLoop = new EventLoop();
    //     ioLoop->loop();
    //     });
    // ioThread.start();
    EventLoopThreadPool threadPool(4, "ioThread");

    threadPool.start();

    EventLoop mainLoop;
    Acceptor acceptor(mainLoop, 25465);

    // [fd -> std::shared_ptr<Connection>]
    // TODO connMaps should be thread local
    unordered_map<int, std::shared_ptr<Connection>> connMaps;

    acceptor.listen(5);

    acceptor.acceptCallback([&threadPool, &acceptor, &connMaps](int conn)-> void {
        auto* loop = threadPool.getLoop();
        connMaps[conn] = std::make_shared<Connection>(*loop, conn, acceptor.hostAddress, acceptor.peerAddress);
        auto newConn = connMaps[conn];
        newConn->setMessageCallback([](std::shared_ptr<Connection> ptrConn, Buffer& buf) {
            const auto str = buf.readAll();
            LOG_TRACE << "get message from connection:" << ptrConn->fd() << " \n    " << str << ")";
            ptrConn->send(HTTPParse::parse(str));
        });

        newConn->setCloseCallback([&connMaps](std::shared_ptr<Connection> ptrConn) {
            LOG_TRACE << "remove connection:" << ptrConn->fd() << " from connMaps";
            connMaps.erase(ptrConn->fd());
        });
        newConn->setWriteFinishCallback([&connMaps](std::shared_ptr<Connection> ptrConn) {
            LOG_TRACE << "remove connection:" << ptrConn->fd() << " from connMaps";
            connMaps.erase(ptrConn->fd());
        });
    });
    


    LOG_INFO << "server running";
    // Logger::setOutput(outPut);
    mainLoop.loop();
}
