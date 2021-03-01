#include "InetAddress.h"

#include "cstring"
#include "arpa/inet.h"

USE_NAMESPACE

static void setPort(sockaddr_in& addr, uint16_t port) {
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
}

InetAddress::InetAddress(uint32_t ip, uint16_t port): addr_() {
    setPort(addr_, port);
    addr_.sin_addr.s_addr = htonl(ip);
}


InetAddress::InetAddress(const std::string& ip, uint16_t port):addr_() {
    setPort(addr_, port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string InetAddress::toString() const {
    std::string result(inet_ntoa(addr_.sin_addr));
    result.append(":");
    result += std::to_string(ntohs(addr_.sin_port));
    return result;
}
