#include "Request.h"

using namespace webServer::http;
using std::string;
using std::map;

Request::Request() : version_(HttpVersion::Other), requestMethod_(RequestMethod::Other) {}


void Request::clear() {
    Request temp;
    this->swap(temp);
}

void Request::addHeader(const string& header, const string& value) {
    headers_[header] = value;
}

HttpVersion& Request::version() {
    return version_;
}

RequestMethod& Request::method() {
    return requestMethod_;
}

string& Request::URL() {
    return URL_;
}

string& Request::body() {
    return body_;
}

map<string, string>& Request::header() {
    return headers_;
}

bool Request::isHeader(const std::string& header) const {
    return headers_.find(header) != headers_.end();
}

string Request::getHeader(const string& header) const {
    auto it = headers_.find(header);
    if (it == headers_.end())
        return "";
    return it->second;
}

void Request::swap(Request& request) noexcept {
    headers_.swap(request.headers_);
    URL_.swap(request.URL_);
    body_.swap(request.body_);
    std::swap(requestMethod_, request.requestMethod_);
    std::swap(version_, request.version_);
}

void swap(Request& lhs, Request& rhs) noexcept {
    lhs.swap(rhs);
}
