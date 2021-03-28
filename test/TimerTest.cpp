#include <cstdint>
#include <iostream>
#include <sys/timerfd.h>
#include <unistd.h>

int main() {

    const auto timer = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);

    itimerspec value{};
    value.it_value.tv_sec = 1;
    value.it_interval.tv_sec = 3;

    timerfd_settime(timer, 0, &value, nullptr);

    int count = 0;

    while (true) {
        uint64_t val;
        auto     ret = read(timer, &val, sizeof val);
        
        std::cout << "alarm: " << count++ << std::endl;
    }

}