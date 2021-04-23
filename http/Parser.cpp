#include "Parser.h"
#include "../Buffer.h"

using webServer::Buffer;
using namespace webServer::http;


static const char*   peekNext(const char* begin, const char* end, char ch);
static RequestMethod parseMethod(const char* begin, const char* end);
static std::string   parseURL(const char* begin, const char* end);
static HttpVersion   parseVersion(const char* begin, const char* end);

static const char* findCRLF(const char* begin, const char* end) {
    const auto len = end - begin;
    for (int i = 0; i < len - 1; ++i) {
        if (begin[i] == '\r' && begin[i + 1] == '\n')
            return begin + i;
    }
    return nullptr;
}


bool Parser::parseRequest(Buffer* buffer) {
    auto ret = parse(buffer->peek(), buffer->peek() + buffer->size());
    if (ret < 0) {
        // parse fail
        return false;
    }
    // ret is the length of character consumed
    buffer->consume(ret);

    return true;
}

bool Parser::gotAll() const {
    return state_ == ParseState::GotAll;
}

void Parser::reset() {
    state_ = ParseState::ExpectRequestLine;
    request_.clear();
    remainLength_ = -1;
}

int Parser::parse(const char* begin, const char* end) {
    const auto* start = begin;
    if (state_ == ParseState::ExpectRequestLine) {
        // parse the head line
        const auto* endOfLine = findCRLF(start, end);
        if (!endOfLine) {} // didn't get the head line, just wait

        // parse the request line

        if (!parseHeadLine(start, endOfLine)) // parse failed
            return -1;

        // parse success, update pointer
        state_ = ParseState::ExpectHeader;
        start  = endOfLine + 2;
    }
    if (state_ == ParseState::ExpectHeader) {
        // still wait for a line to get
        // when find an empty CRLF means end of header
        while (true) {
            const auto* endOfLine = findCRLF(start, end);
            if (!endOfLine) {
                // waiting for data
                break;
            }
            if (endOfLine == start) {
                // end of header
                start += 2;
                state_ = ParseState::ExpectBody;
                break;
            }
            if (!parseHeader(start, endOfLine))
                return -1;
            start = endOfLine + 2;
        }
    }
    if (state_ == ParseState::ExpectBody) {
        if (request_.method() == RequestMethod::Get || request_.method() == RequestMethod::Head) {
            // no body in GET method, GotAll
            state_ = ParseState::GotAll;
        }
        else if (request_.method() != RequestMethod::Other) {
            // get body from message
            auto it = request_.header().find("Content-Length");
            if (it == request_.header().end())
                return -1;
            remainLength_ = std::stoi(it->second);
            auto len      = end - start;
            if (len < remainLength_) {
                // not full message
                request_.body() = std::string(start, end);
                start           = end;
                remainLength_ -= len;
            }
            else {
                request_.body() = std::string(start, remainLength_);
                start += remainLength_;
                remainLength_ = 0;
                state_        = ParseState::GotAll;
            }
        }
    }

    return start - begin;
}


/*    |method|' '|URL|' '|version|'0x0a'|'0x0d'|
 *  begin------------------------------end
 */
bool Parser::parseHeadLine(const char* begin, const char* end) {
    const auto* firstSpace = peekNext(begin, end, ' ');
    if (firstSpace == end)
        return false;
    const auto* secondSpace = peekNext(firstSpace + 1, end, ' ');
    if (secondSpace == end)
        return false;

    request_.method()  = parseMethod(begin, firstSpace);
    request_.URL()     = parseURL(firstSpace + 1, secondSpace);
    request_.version() = parseVersion(secondSpace + 1, end);
    return true;
}

/*  |header|"   "|':'|"   "|key|'0x0a'|'0x0d'|
 * begin-----------------------------end
 */
bool Parser::parseHeader(const char* begin, const char* end) {
    const auto* mid = peekNext(begin, end, ':');
    if (mid == end)
        return false;

    const auto* left = mid - 1;
    while (*left == ' ') --left;

    const auto* right = mid + 1;
    while (*right == ' ') ++right;

    request_.addHeader(
                       std::string(begin, left + 1),
                       std::string(right, end));
    // state_ = ParseState::ExpectHeader;
    return true;
}


bool strMatch(const char* begin, const char* end, int len, const char* pattern) {
    if (end - begin != len)
        return false;
    while (begin != end) {
        if (*begin != *pattern)
            return false;
        ++pattern;
        ++begin;
    }
    return true;
}

const char* peekNext(const char* begin, const char* end, char ch) {
    while (begin != end && *begin != ch)
        ++begin;
    return begin != end ? begin : end;
}

RequestMethod parseMethod(const char* begin, const char* end) {
    if (begin[0] == 'P') {
        if (strMatch(begin, end, 3, "PUT"))
            return RequestMethod::Put;
        if (strMatch(begin, end, 4, "POST"))
            return RequestMethod::Post;
    }
    else if (begin[0] == 'G') {
        if (strMatch(begin, end, 3, "GET"))
            return RequestMethod::Get;
    }
    else if (begin[0] == 'H') {
        if (strMatch(begin, end, 4, "HEAD"))
            return RequestMethod::Head;
    }
    return RequestMethod::Other;
}

std::string parseURL(const char* begin, const char* end) {
    return std::string(begin, end);
}

HttpVersion parseVersion(const char* begin, const char* end) {
    if (strMatch(begin, begin + 7, 7, "HTTP/1.") && end - begin == 8) {
        switch (begin[7]) {
            case '0':
                return HttpVersion::HTTP10;
            case '1':
                return HttpVersion::HTTP11;
            default:
                return HttpVersion::Other;
        }
    }
    return HttpVersion::Other;
}
