#pragma once

#include "Util.h"

#include "Request.h"

START_NAMESPACE

class Buffer;

namespace http {


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

        Request& request() {
            return request_;
        }

    public:
        int parse(const char* begin, const char* end);
        bool parseHeadLine(const char* begin, const char* end);
        bool parseHeader(const char* begin, const char* end);
    private:
        ParseState state_ = ParseState::ExpectRequestLine;
        Request request_;
        int remainLength_ = -1;
    };
}
END_NAMESPACE