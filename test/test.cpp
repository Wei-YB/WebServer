#include "../InetAddress.h"

#include <iostream>

using namespace webServer;
using namespace std;


int main() {
    InetAddress address("192.168.1.1", 80);
    cout << address.toString() << endl;
}