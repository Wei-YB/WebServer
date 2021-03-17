#include <cstring>
#include <deque>
#include <iostream>
#include <string>
#include "../Buffer.h"

int main() {
    Buffer buffer;
    buffer.write("Hello World",11);
    std::cout << buffer.readAll()<< std::endl;
    for(int i = 0; i < 20; ++i) {
        buffer.write("Test ", 5);
    }
    std::cout << buffer.read(10 * 5) << std::endl;
    std::cout << buffer.size()<<std::endl;

    for(int i = 0; i < 10000; ++i) {
        buffer.write("Test ", 5);
    }
    buffer.readAll();
    buffer.shrinkToFit();
    return 0;
}