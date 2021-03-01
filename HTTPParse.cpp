#include "HTTPParse.h"

using std::string;

class HTMLText {
public:

    static string textFormat(const string& text) {
        string html;
        html +=
            "<html><head><title>a light web server </title>"
            "<link rel=\"icon\" href=\"data:; base64, =\">"
            "</head>\n"
            "<body><h1>this is " 
            "what you want</h1><p>" +
            text +
            "</p></body></html>\n";
        return html;
    }

    static string generate(int state, const string& text = {}) {
        switch (state) {
        case 200:
            return "HTTP/1.1 200 OK\n" + Connection() + ContentLength(text.size()) + "\n" + text;
        case 404:
            return "HTTP/1.1 404 Not Found\n";
            break;
        default:
            return "HTTP/1.1 400 Bad Request\n";
        }
    }

private:
    static string Connection(bool flag = true) {
        return "Connection: close\n";
    }

    static string ContentLength(size_t len) {
        return "Content-Length: " + std::to_string(len) + "\n";
    }
};

enum class RequestType {
    GET,
    POST,
    PUT,
    ERROR,
};

RequestType parseHead(const char* request, size_t len, int& index) {
    switch (request[index]) {
    case 'G':
        if (request[index + 1] == 'E' && request[index + 2] == 'T') {
            index += 4;
            return RequestType::GET;
        }
        return RequestType::ERROR;
        break;
    case 'P':
        if (request[index + 1] == 'O') {
            // POST
            if (request[index + 2] == 'S' && request[index + 3] == 'T') {
                index += 5;
                return RequestType::POST;
            }
            return RequestType::ERROR;
        }
        if (request[index + 1] == 'U') {
            //PUT 
            if (request[index + 2] == 'T') {
                index += 4;
                return RequestType::PUT;
            }
            return RequestType::ERROR;
        }
        break;
    default:
        return RequestType::ERROR;
    }
    return RequestType::ERROR;
}

static string notFound = HTMLText::generate(404);

static string defaultPage = HTMLText::generate(200, HTMLText::textFormat("Hello World!"));


string parseGet(const char* request, size_t len, int& index) {
    if (request[index] == '/')
        ++index;
    else
        return notFound;
    if(request[index] == ' ') {
        return defaultPage;
    }
    else {
        string text;
        for(; index < len; ++index) {
            if(index == ' ')
                break;
            text.push_back(request[index]);
        }
        return HTMLText::generate(200, HTMLText::textFormat(text));
    }
}



string HTTPParse::parse(const char* request, size_t len) {
    int index = 0;
    auto type = parseHead(request, len, index);
    if(type == RequestType::GET) {
        return parseGet(request, len, index);
    }
    return notFound;
}



std::string HTTPParse::parse(const std::string& request) {
    return parse(request.c_str(), request.size());
}

