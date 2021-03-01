// WebServerTest.cpp: 定义应用程序的入口点。
//

#include "WebServerTest.h"

#include <strings.h>
#include <cstring>


#include "netinet/in.h"
#include "sys/socket.h"
#include <unistd.h>


#include "Channel.h"
#include "HTTPParse.h"
#include "Poller.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"


USE_NAMESPACE
using namespace std;

int main() {
    Logger::setLogLevel(Logger::LogLevel::TRACE);

    EventLoop mainLoop;
    Acceptor acceptor(mainLoop, 25465);

    acceptor.listen(5);

    acceptor.acceptCallback([&mainLoop, &acceptor](int conn)-> void {
        Connection newConn(mainLoop, conn, acceptor.hostAddress, acceptor.peerAddress);
        newConn.setMessageCallback([](std::shared_ptr<Connection> PtrConn, auto msg, auto len) {
            std::string str(msg, len);
            PtrConn->send(HTTPParse::parse(str));
        });
    });


    LOG_INFO << "server running";

    mainLoop.loop();
}
