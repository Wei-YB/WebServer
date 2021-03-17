#pragma once

#include <vector>
#include <string>

#include "Util.h"

START_NAMESPACE

class Buffer {
public:

    Buffer();

    void consume(size_t len);

    void write(const char* str, size_t len);

    std::string read(size_t len);

    std::string readAll();

    const char* peek();

    [[nodiscard]]
    size_t size() const;

    [[nodiscard]]
    size_t writable() const;

    void shrinkToFit();

private:

    char* writePos();

    void ensureCapacity(size_t len);
    size_t readIndex_;
    size_t writeIndex_;
    std::vector<char> buffer_;
};

END_NAMESPACE