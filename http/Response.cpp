#include "Response.h"

using std::string;
using std::to_string;
using namespace webServer::http;

static const char* toResponseStatus(int);
static string      toHeadLine(int status);


Response::string Response::toString() const {
    string respond = toHeadLine(status_);
    if (closeConnection_)
        respond += "Connection: close\r\n";
    else
        respond += "Connection: Keep-Alive\r\n";

    if (!body_.empty())
        respond += "Content-Length: " + to_string(body_.size()) + "\r\n";


    for (const auto& p : headers_) {
        const auto line = p.first + ": " + p.second + "\r\n";
        respond += line;
    }
    respond += "\r\n";
    respond += body_;
    return respond;
}


string toHeadLine(int status) {
    string headLine = "HTTP/1.1 ";
    headLine += to_string(status);
    headLine.push_back(' ');
    headLine += toResponseStatus(status);
    headLine.append("\r\n");
    return headLine;
}

const char* toResponseStatus(int status) {
    switch (status) {
        case 200:
            return "OK";
        case 204:
            return "No Content";
        case 301:
            return "Moved Permanently";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        default:
            return "Unknown";
    }
}
