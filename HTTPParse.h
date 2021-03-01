#pragma once

#include <string>

class HTTPParse {
public:
    static std::string parse(const char* request, size_t len);
    static std::string parse(const std::string& request);
};