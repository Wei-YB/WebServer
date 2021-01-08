#include "LogStream.h"
#include <algorithm>
#include <cstdlib>

USE_NAMESPACE



LogStream::reference LogStream::operator<<(bool cond) {
    buffer_.append(cond ? "1" : "0", 1);
    return *this;
}

LogStream::reference LogStream::operator<<(short val) {
    this->operator<<(static_cast<int>(val));
    return *this;
}

LogStream::reference LogStream::operator<<(unsigned short val) {
    this->operator<<(static_cast<unsigned int>(val));
    return *this;
}

LogStream::reference LogStream::operator<<(int val) {
    formatInteger(val);
    return *this;
}

LogStream::reference LogStream::operator<<(unsigned int val) {
    formatInteger(val);
    return *this;
}

LogStream::reference LogStream::operator<<(long val) {
    this->operator<<(static_cast<int>(val));
    return *this;
}

LogStream::reference LogStream::operator<<(unsigned long val) {
    this->operator<<(static_cast<unsigned int>(val));
    return *this;
}

LogStream::reference LogStream::operator<<(long long val) {
    formatInteger(val);
    return *this;
}

LogStream::reference LogStream::operator<<(unsigned long long val) {
    formatInteger(val);
    return *this;
}

LogStream::reference LogStream::operator<<(float val) {
    return this->operator<<(static_cast<double>(val));
}

LogStream::reference LogStream::operator<<(double val) {
    if (buffer_.avail() >= MaxNumericSize) {
        const auto len = snprintf(buffer_.current(), buffer_.avail(), "%f", val);
        buffer_.add(len);
    }
    return *this;
}

LogStream::reference LogStream::operator<<(char val) {
    buffer_.append(&val, 1);
    return *this;
}

LogStream::reference LogStream::operator<<(const char* val) {
    if (val)
        buffer_.append(val, strlen(val));
    else
        buffer_.append("(null)", 6);
    return *this;
}

LogStream::reference LogStream::operator<<(const unsigned char* val) {
    return this->operator<<(reinterpret_cast<const char*>(val));
}

LogStream::reference LogStream::operator<<(const std::string& val) {
    buffer_.append(val.c_str(), val.size());
    return *this;
}

LogStream::reference LogStream::operator<<(const MyBuffer& v) {
    buffer_.append(v.data(), v.length());
    return *this;
}

void LogStream::staticCheck() {
    static_assert(MaxNumericSize - 10 > std::numeric_limits<double>::digits10,
        "kMaxNumericSize is large enough");
    static_assert(MaxNumericSize - 10 > std::numeric_limits<long double>::digits10,
        "kMaxNumericSize is large enough");
    static_assert(MaxNumericSize - 10 > std::numeric_limits<long>::digits10,
        "kMaxNumericSize is large enough");
    static_assert(MaxNumericSize - 10 > std::numeric_limits<long long>::digits10,
        "kMaxNumericSize is large enough");
}

template <typename T>
void LogStream::formatInteger(T val) {
    const std::string str = std::to_string(val);
    buffer_.append(str.c_str(), str.size());
}
