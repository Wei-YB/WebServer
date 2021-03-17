#include "Buffer.h"

#include <cassert>
#include <cstring>

USE_NAMESPACE

Buffer::Buffer(): readIndex_(0), writeIndex_(0), buffer_(256) {}

void Buffer::consume(size_t len) {
    if (size() > len) {
        readIndex_ += len;
    }
    else {
        consumeAll();
    }
}

void Buffer::consumeAll() {
    readIndex_ = writeIndex_ = 0;
}

void Buffer::write(const char* str, size_t len) {
    ensureCapacity(len);
    memmove(writePos(), str, len);
    writeIndex_ += len;
}

std::string Buffer::read(size_t len) {

    auto msg = std::string(peek(), len > size() ? size() : len);
    consume(len);
    return msg;

    // if (size() > len) {
    //     readIndex_ += len;
    //     return std::string(peek(), len);
    // }
    // else {
    //     std::string ret(peek(), size());
    //     readIndex_ = writeIndex_ = 0;
    //     return ret;
    // }
}

std::string Buffer::readAll() {
    return this->read(this->size());
}

const char* Buffer::peek() {
    return buffer_.data() + readIndex_;
}

size_t Buffer::size() const {
    assert(writeIndex_ >= readIndex_);
    return writeIndex_ - readIndex_;
}

bool Buffer::empty() const {
    return size() == 0;
}

size_t Buffer::writable() const {
    return buffer_.size() - writeIndex_;
}

void Buffer::shrinkToFit() {
    buffer_.resize(writeIndex_);
    buffer_.shrink_to_fit();
}

char* Buffer::writePos() {
    return buffer_.data() + writeIndex_;
}


void Buffer::ensureCapacity(size_t len) {
    if (writable() < len) {
        buffer_.resize(buffer_.size() * 2);
    }
}
