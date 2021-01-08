#pragma once

// standard library
// assert
#include <cassert>
// memcpy, memset
#include <cstring>

#include <string>


// user define head
#include "Utli.h"


START_NAMESPACE

constexpr int SmallBufferSize = 4000;
constexpr int LargeBufferSize = 4000 * 1000;


template <size_t SIZE>
class Buffer {
public:
    using cookieFunc = void(*)(void);

    Buffer() : data_{}, current_(data_) {}

    Buffer(const Buffer& b) : data_{}, current_(data_ + b.length()) {
        memcpy(data_, b.data(), b.length());
    }
    
    Buffer& operator=(const Buffer& buffer) {
        if (this == &buffer)
            return *this;
        memcpy(data_, buffer.data_, buffer.length());
        current_ = data_ + buffer.length();
        return *this;
    }

    Buffer(Buffer&&) noexcept = delete;
    Buffer& operator=(Buffer&&) = delete;

    ~Buffer() = default;

    /**
     * \brief Append contents to the buffer
     * \param buf contents
     * \param len length of buf
     */
    void append(const char* buf, size_t len) {
        if (avail() > len) {
            memcpy(current_, buf, len);
            current_ += len;
        }
    }

    /**
     * \brief Get the origin data of this buffer
     * \return point to the begin of buffer
     */
    const char* data() const { return data_; }

    /**
     * \brief Length of the current buffer used
     * \return unsigned integer
     */
    size_t length() const { return current_ - data_; }

    /**
     * \brief Pointer of the available space, common used as
     * write to data_ directly
     * \return pointer
     */
    // ReSharper disable once CppMemberFunctionMayBeConst
    char* current() { return current_; }

    /**
     * \brief Get number of characters available
     * \return unsigned integral type
     */
    size_t avail() const { return end() - current_; }

    /**
     * \brief Set internal position pointer to relative position
     * \param len Offset value
     */
    void add(size_t len) { current_ += len; }

    /**
     * \brief Set internal position pointer to beginning of the buffer 
     */
    void reset() { current_ = data_; }

    /**
     * \brief clear the buffer, but not change the internal position pointer
     */
    void bzero() { memset(data_, 0, sizeof data_); }

    /**
     * \brief make a copy of this buffer
     * \return std::string contains same as this buffer
     */
    std::string toString() const { return std::string(data_, length()); }

private:
    const char* end() const { return data_ + sizeof data_; }


    // define of member
    char data_[SIZE];
    char* current_;
};

class LogStream {
public:

    using reference = LogStream&;
    using const_reference = const LogStream&;
    using pointer = LogStream*;
    using const_pointer = const LogStream*;
    
    using MyBuffer = Buffer<SmallBufferSize>;

    // formatting built in type 
    reference operator<<(bool);
    reference operator<<(short);
    reference operator<<(unsigned short);
    reference operator<<(int);
    reference operator<<(unsigned int);
    reference operator<<(long);
    reference operator<<(unsigned long);
    reference operator<<(long long);
    reference operator<<(unsigned long long);
    reference operator<<(float);
    reference operator<<(double);
    reference operator<<(char);
    reference operator<<(const char*);
    reference operator<<(const unsigned char*);
    //  reference operator<<(const void*);
        
    // formatting std::string
    reference& operator<<(const std::string& v);

    // buffer
    reference& operator<<(const MyBuffer& v);

    void append(const char* data, size_t len) { buffer_.append(data, len); }
    const MyBuffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    void staticCheck();

    template <typename T>
    void formatInteger(T);

    MyBuffer buffer_;
    static const int MaxNumericSize = 32;
};

END_NAMESPACE
