#include "WebServerTest.h"

#include <strings.h>
#include <cstring>
#include <csignal>
#include <unistd.h>


#include "http/Parser.h"
#include "http/Response.h"
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

using http::Parser;
using http::Response;

static Response badReq;
static Response notFound;
static string mainPage;
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

    badReq.status() = 400;
    notFound.status() = 404;

    server.setConnectionCallback([](std::shared_ptr<Connection> conn) {
        if (conn->established()) {
            conn->setContext(http::Parser{});
        }
    });

    mainPage = "<html><head><title>a light web server </title>"
        "<link rel=\"icon\" href=\"data:; base64, =\">"
        "</head>\n"
        "<body><h1>this is "
        "what you want</h1><p>"
        "Hello World"
        "</p></body></html>\n";

    Response response;
    response.status() = 200;
    response.body() = mainPage;
    response.closeConnection() = false;

    const auto resultStr = response.toString();

    // TODO 
    server.setMessageCallback([&resultStr](std::shared_ptr<Connection> ptrConn, Buffer& buf) {

        auto& parser = std::any_cast<http::Parser&>(ptrConn->getContext());
        auto ret = parser.parseRequest(&buf);
        if(ret) {
            if (parser.gotAll()) {
                auto& req = parser.request();
                ptrConn->send(resultStr);
            }
        }else { // parse fail : bad request
            ptrConn->send(badReq.toString());
        }
        //
        // const auto str = buf.readAll();
        // LOG_TRACE << "get message from connection:" << ptrConn->fd() << " \n    " << str << ")";
        // const auto result = ::parse(str);
        // ptrConn->send(result);
    });

    LOG_INFO << "server running";

    server.start();
    
}

// TODO: add direct io to buffer
// TODO: finish httpServer
// TODO: update the control method of epoll out
// TODO: add timer for keep alive
// TODO: maybe make Socket.h a class?