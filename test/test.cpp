#include <cstring>
#include <sys/epoll.h>
#include <iostream>
#include <strings.h>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace std;


int main() {
    auto listener = socket(AF_INET, SOCK_STREAM, 0);
    cout << "listener = " << listener << endl;
    sockaddr_in addr;
    bzero(&addr, sizeof addr);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(25465);

    auto ret = bind(listener, reinterpret_cast<const sockaddr*>(&addr), sizeof addr);
    if (ret < 0)
        cout << strerror(errno) << endl;
     if (listen(listener, 10) < 0) {
         cout << strerror(errno) << endl;
     }

    cout << "ready to accept" << endl;
    auto conn = accept(listener, nullptr, nullptr);
    cout << "new connection fd: " << conn << endl;
    auto poller = epoll_create(2);
    epoll_event epollEvent;
    bzero(&epollEvent, sizeof epollEvent);
    epollEvent.events |= EPOLLET;
    epollEvent.events |= EPOLLIN;

    epoll_ctl(poller, EPOLL_CTL_ADD, conn,&epollEvent);
    vector<epoll_event> events(10);

    sleep(10);

    auto num = epoll_wait(poller, events.data(), events.size(), 10);
    cout << "epoll return with num:" << num << endl;

    char buf[20];
    auto len = read(conn, buf, sizeof buf);
    cout << "read result = " << len << endl;

    sleep(10);
    shutdown(conn, SHUT_RDWR);
}