#pragma once

#include <netinet/in.h>
#include <string>
#include "base/Util.h"

START_NAMESPACE

class InetAddress {

public:
    InetAddress() = default;

    InetAddress(uint32_t ip, uint16_t port);

    explicit InetAddress(const std::string& ip, uint16_t port);
    InetAddress(const InetAddress&) = default;
    InetAddress(InetAddress&&) noexcept = default;
    ~InetAddress() = default;

    InetAddress& operator=(const InetAddress&) = default;
    InetAddress& operator=(InetAddress&&) noexcept = default;

    sockaddr* address() { return reinterpret_cast<sockaddr*>(&addr_); }
    const sockaddr* address() const { return reinterpret_cast<const sockaddr*>(&addr_); }

    std::string toString() const;

    static InetAddress listenAddress(uint16_t port) {
        return InetAddress{INADDR_ANY, port};
    }

private:
    sockaddr_in addr_;
};

END_NAMESPACE