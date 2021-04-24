#pragma once

#include "../base/Util.h"

#include <string>
#include <map>

namespace
NAMESPACE http {

class Response {
private:
    using string = std::string;
public:
    using StatusCode = int;
public:

    Response() : status_(400), closeConnection_(false) {}

    //     const Status& status() const { return const_cast<const Response*>(this)->status(); }
    StatusCode& status() { return status_; }

    void addHeader(const string& header, const string& value) {
        headers_[header] = value;
    }

    string& body() { return body_; }

    string toString() const;

    bool& closeConnection() { return closeConnection_; }

private:
    StatusCode                         status_;
    std::map<std::string, std::string> headers_;
    std::string                        body_;

    bool closeConnection_;

};

}
