#pragma once

#include "Request.h"
#include "../base/Util.h"


START_NAMESPACE
class Buffer;
END_NAMESPACE

namespace
NAMESPACE http {

enum class ParseState {
    ExpectRequestLine,
    ExpectHeader,
    ExpectBody,
    GotAll,
};


class Parser {
public:
    bool parseRequest(Buffer* buffer);
    bool gotAll() const;

    void reset();

    const Request& request() const {
        return request_;
    }

    ::webServer::http::Request& request() {
        return request_;
    }

private:
    int  parse(const char* begin, const char* end);
    bool parseHeadLine(const char* begin, const char* end);
    bool parseHeader(const char* begin, const char* end);
private:
    ParseState state_ = ParseState::ExpectRequestLine;
    Request    request_;
    int        remainLength_ = -1;
};
}
