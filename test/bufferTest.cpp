#include <cstring>
#include <deque>
#include <iostream>
#include <string>

constexpr int BlockSize = 2048;

struct Buffer {
    using Block = char[2048];
    std::deque<Block> buffer_;
    int startPos_;
    int endPos_;
    size_t size_;

    void consume(size_t size) {
        if (size >= size_) {
            size_ = startPos_ = endPos_ = 0;
            buffer_.clear();
            return;
        }
        while (size > BlockSize) {
            size -= BlockSize;
            buffer_.pop_front();
        }
        if (size >= BlockSize - startPos_) {
            buffer_.pop_front();
            startPos_ = size - (BlockSize - startPos_);
        }
        if (size < BlockSize - startPos_)
            startPos_ += size;
        size_ -= size;
    }

    void append(const char* str, int len) {
        while(len >= BlockSize - endPos_) {
            const auto leftSize = BlockSize - endPos_;
            auto& curBuf = buffer_.back();
            memcpy(curBuf + endPos_, str, leftSize);
            str += leftSize;
            len -= leftSize;
            char data[2048];
            buffer_.push_back(data);
            endPos_ = 0;
        }
        if(len > 0)
            memcpy(buffer_.back(),str,len);
        size_ += len;
    }

    void append(const std::string& str) {
        append(str.c_str(), str.size());
    }

    std::string toString() const {
        std::string result;
        if (size_ == 0)
            return "(null)";
        if (buffer_.size() == 1) {
            result.append(buffer_[0] + startPos_, endPos_ - startPos_);
            return result;
        }
        result.append(buffer_[0] + startPos_, BlockSize - startPos_);
        for (int i = 1; i < buffer_.size() - 1; ++i) {
            result.append(buffer_[i], BlockSize);
        }
        result.append(buffer_.back(), endPos_);
        return result;
    }
};

int main() {
    Buffer buffer;
    buffer.append("Hello World",11);
    std::cout << buffer.toString()<< std::endl;
    buffer.consume(2);
    std::cout<<buffer.toString()<<std::endl;
}