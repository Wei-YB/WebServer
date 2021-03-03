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
#include "Thread.h"


USE_NAMESPACE
using namespace std;

EventLoop* ioLoop = nullptr;



int main() {
    Logger::setLogLevel(Logger::LogLevel::TRACE);

    EventLoop* ioLoop = nullptr;
    Thread ioThread([&ioLoop]() {
        ioLoop = new EventLoop();
        ioLoop->loop();
        });
    ioThread.start();

    EventLoop mainLoop;
    Acceptor acceptor(mainLoop, 25465);

    // [fd -> std::shared_ptr<Connection>]
    unordered_map<int, std::shared_ptr<Connection>> connMaps;

    acceptor.listen(5);

    acceptor.acceptCallback([&ioLoop, &acceptor, &connMaps](int conn)-> void {
        connMaps[conn] = std::make_shared<Connection>(*ioLoop, conn, acceptor.hostAddress, acceptor.peerAddress);
        auto newConn = connMaps[conn];
        newConn->setMessageCallback([](std::shared_ptr<Connection> ptrConn, auto msg, auto len) {
            const std::string str(msg, len);
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

    mainLoop.loop();
}
