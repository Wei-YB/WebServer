#pragma once

#include <vector>
#include <string>

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

    void shrinkToFit();

private:

    void resize(size_t len);
    size_t readIndex_;
    size_t writeIndex_;
    std::vector<char> buffer_;
};