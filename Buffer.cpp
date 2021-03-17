#include "Buffer.h"

#include <cassert>
#include <cstring>

Buffer::Buffer(): readIndex_(0), writeIndex_(0), buffer_(256) {}

void Buffer::consume(size_t len) {
    if (size() > len) {
        readIndex_ += len;
    }
    else {
        readIndex_ = writeIndex_ = 0;
    }
}

void Buffer::write(const char* str, size_t len) {
    resize(len);
    memcpy(buffer_.data() + writeIndex_, str, sizeof(char) * len);
    writeIndex_ += len;
}

std::string Buffer::read(size_t len) {
    if (size() > len) {
        readIndex_ += len;
        return std::string(peek(), len);
    }
    else {
        std::string ret(peek(), len);
        readIndex_ = writeIndex_ = 0;
        return ret;
    }
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

void Buffer::shrinkToFit() {
    buffer_.resize(writeIndex_);
    buffer_.shrink_to_fit();
}


void Buffer::resize(size_t len) {
    const auto available = buffer_.size() - writeIndex_;
    if (available < len) {
        buffer_.resize(len * 2 + writeIndex_);
    }
}
