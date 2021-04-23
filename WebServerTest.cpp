#include "WebServerTest.h"

#include <strings.h>
#include <cstring>
#include <csignal>
#include <unistd.h>


#include "HTTPParse.h"
#include "Connection.h"
#include "Logger.h"
#include "TCPServer.h"

USE_NAMESPACE
using namespace std;

// EventLoop* ioLoop = nullptr;

// AsyncLogging* asyncLog;

// void outPut(const char* str, size_t len) {
//     asyncLog->append(str, len);
// }


void threadInit(EventLoop* loop) {
    LOG_INFO << "io thread init";
    loop->assertInLoopThread();
}

int main() {
    //TODO use async log!
    //TODO EventLoopThreadPoll
    // bug: async log not work
    // AsyncLogging log("/home/csi/webServer", 1000 * 1000 * 500);
    // log.start();
    // asyncLog = &log;

    // fixed : in order to prevent SIGPIPE in write, just ignore SIGPIPE
    ::signal(SIGPIPE, SIG_IGN);
    

    Logger::setLogLevel(Logger::LogLevel::TRACE);

    TcpServer server(25465, 1);

    server.setMessageCallback([](std::shared_ptr<Connection> ptrConn, Buffer& buf) {
        const auto str = buf.readAll();
        LOG_TRACE << "get message from connection:" << ptrConn->fd() << " \n    " << str << ")";
        const auto result = HTTPParse::parse(str);
        ptrConn->send(result);
    });

    LOG_INFO << "server running";

    server.start();
    
}
