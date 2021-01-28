
#include <iostream>

#include "Logger.h"
using namespace std;
USE_NAMESPACE


int main() {
    LOG_INFO << "main thread start";
    int i = 100;
    while (--i ) {
        LOG_INFO << "current i is " << i;
    }
    LOG_INFO << "main thread end";
    
    return 0;
}