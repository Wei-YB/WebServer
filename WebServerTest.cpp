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
#include "EventLoop.h"


USE_NAMESPACE
using namespace std;

int main() {
    Logger::setLogLevel(Logger::LogLevel::TRACE);

    EventLoop mainLoop;
    Acceptor acceptor(mainLoop, 25465);

    acceptor.listen(5);


    vector<Channel*> activeChannel;

    acceptor.acceptCallback([](int conn)-> void {
        static char buf[2048];
        auto len = read(conn, &buf, sizeof buf);
        buf[len] = '\0';
        cout << "incoming http request is: " << buf << endl;
        auto text = HTTPParse::parse(buf, len);
        cout << text << endl;
        send(conn, text.c_str(), text.size(), 0);
        shutdown(conn, 2);
    });


    LOG_INFO << "server running";

    mainLoop.loop();
}
