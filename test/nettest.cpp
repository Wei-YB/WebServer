#include "../Socket.h"

using namespace webServer;


int main() {
    auto listenfd = socket::socket(AF_INET, SOCK_STREAM, 0);
    socket::bind(listenfd, InetAddress::listenAddress(23456));
    socket::listen(listenfd, 1024);

    InetAddress peerAddr;
    socklen_t socklen = sizeof peerAddr;
    int fd =  -1;
    while (fd == -1) {
        fd = accept(listenfd, peerAddr.address(),&socklen);
    }
    InetAddress hostAddr;
    
    getsockname(fd, hostAddr.address(), &socklen);


    std::cout << "connection from: " << peerAddr.toString() << " and to: " << hostAddr.toString() << std::endl
    ;

    getchar();
    return 0;
}