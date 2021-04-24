#pragma once


#include "../base/Util.h"

#include <map>
#include <string>
#include <iostream>


namespace webServer::http {


enum class RequestMethod {
    Get,
    Post,
    Head,
    Put,
    Other
};

inline std::string toString(RequestMethod method) {
    switch (method) {
        case RequestMethod::Get:
            return "GET";
            break;
        case RequestMethod::Post:
            return "POST";
            break;
        case RequestMethod::Head:
            return "HEAD";
            break;
        case RequestMethod::Put:
            return "PUT";
            break;
        case RequestMethod::Other:
            return "OTHER";
            break;
    }
}

inline std::ostream& operator<<(std::ostream& os, RequestMethod method) {
    os << toString(method);
    return os;
}

enum class HttpVersion {
    HTTP10,
    HTTP11,
    Other,
};

inline std::string toString(HttpVersion version) {
    switch (version) {
        case HttpVersion::HTTP10:
            return "HTTP1.0";
            break;
        case HttpVersion::HTTP11:
            return "HTTP1.1";
            break;
        case HttpVersion::Other:
            return "Unknown Version";
            break;
    }
}

inline std::ostream& operator<<(std::ostream& os, HttpVersion version) {
    os << toString(version);
    return os;
}

class Request {

public:
    using Headers = std::map<std::string, std::string>;

    Request();
    Request(const Request& request)            = default;
    Request(Request&& request)                 = default;
    Request& operator=(const Request& request) = default;
    Request& operator=(Request&& request)      = default;
    ~Request()                                 = default;

    void clear();

    void           addHeader(const std::string& header, const std::string& value);
    HttpVersion&   version();
    RequestMethod& method();
    std::string&   URL();
    std::string&   body();

    Headers& header();

    bool        isHeader(const std::string& header) const;
    std::string getHeader(const std::string& header) const;

    void        swap(Request& request) noexcept;
    friend void swap(Request& lhs, Request& rhs) noexcept;

private:
    HttpVersion   version_;
    RequestMethod requestMethod_;
    std::string   URL_;
    std::string   body_;
    Headers       headers_;
};
}