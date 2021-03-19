// WebServerTest.cpp: 定义应用程序的入口点。
//

#include "WebServerTest.h"

#include <strings.h>
#include <cstring>
#include <signal.h>

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


void threadInit(EventLoop* loop) {
    std::cout << "thread init" << std::endl;
}

int main() {
    //TODO use async log!
    //TODO EventLoopThreadPoll
    //
    AsyncLogging log("/home/csi/webServer", 1000 * 1000 * 500);
    log.start();
    asyncLog = &log;

    // fixme : in order to prevent SIGPIPE in write, just ignore SIGPIPE
    ::signal(SIGPIPE, SIG_IGN);
    

    Logger::setLogLevel(Logger::LogLevel::INFO);

    // EventLoop* ioLoop = nullptr;
    // Thread ioThread([&ioLoop]() {
    //     ioLoop = new EventLoop();
    //     ioLoop->loop();
    //     });
    // ioThread.start();
    EventLoopThreadPool threadPool(4, "ioThread", threadInit);

    threadPool.start();

    EventLoop mainLoop;
    Acceptor acceptor(mainLoop, 25465);

    // [fd -> std::shared_ptr<Connection>]
    // FIXME: thread unsafe
    // bug: need to erase connection in loop func, not all
    unordered_map<int, std::shared_ptr<Connection>> connMaps;

    acceptor.listen(256);

    acceptor.acceptCallback([&threadPool, &acceptor, &connMaps, &mainLoop](int conn)-> void {
        auto* loop    = threadPool.getLoop();
        auto  newConn = std::make_shared<Connection>(*loop, conn, acceptor.hostAddress, acceptor.peerAddress);
        // fixed, thread unsafe, messageCallback may called before correct set
        connMaps[conn] = newConn;
        newConn->setMessageCallback([](std::shared_ptr<Connection> ptrConn, Buffer& buf) {
            const auto str = buf.readAll();
            LOG_TRACE << "get message from connection:" << ptrConn->fd() << " \n    " << str << ")";
            const auto result = HTTPParse::parse(str);
            ptrConn->send(result);
        });

        newConn->setCloseCallback([&connMaps, &mainLoop](std::shared_ptr<Connection> ptrConn) {
            const auto fd = ptrConn->fd();
            LOG_TRACE << "remove connection:" << fd << " from connMaps";
            // connMaps.erase(ptrConn->fd());
            // fix race when erase elem in map
            mainLoop.queueInLoop([&fd, &connMaps]() {
                connMaps.erase(fd);
            });
        });
        newConn->setWriteFinishCallback([&connMaps](std::shared_ptr<Connection> ptrConn) {
            // LOG_TRACE << "remove connection:" << ptrConn->fd() << " from connMaps";
            // ptrConn->close();
        });
        newConn->connected();
    });
    


    LOG_INFO << "server running";
    Logger::setOutput(outPut);
    mainLoop.loop();
}
