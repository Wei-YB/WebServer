#include "Timestamp.h"

#include <cinttypes>
#include <sys/time.h>

USE_NAMESPACE

// std::string Timestamp::toString() const {
//     char          buf[32]      = {0};
//     const int64_t seconds      = microSecond_ / MicroSecondsPerSecond;
//     const int64_t microseconds = microSecond_ % MicroSecondsPerSecond;
//
// #if __WORDSIZE == 64
//     snprintf(buf, 32, "%ld. %06ld", seconds, microseconds);
// #else
//     snprintf(buf, 32, "%lld. %06lld", seconds, microseconds);
// #endif
//
//     return buf;
// }

void Timestamp::format(char* buf, size_t len, bool showMicroseconds) const {
    auto seconds = static_cast<time_t>(microSecond_ / MicroSecondsPerSecond) + 8 * 3600;
    tm   tmTime{};
    gmtime_r(&seconds, &tmTime);
    if (showMicroseconds) {
        const auto microseconds = static_cast<int>(microSecond_ % MicroSecondsPerSecond);
        snprintf(buf, len, "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
                 tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec,
                 microseconds);
    }
    else {
        snprintf(buf, len, "%4d%02d%02d %02d:%02d:%02d",
                 tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
                 tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
    }
}

std::string Timestamp::format(bool showMicroseconds) const {
    char buf[32] = {0};
    format(buf, sizeof buf, showMicroseconds);
    return showMicroseconds ? std::string(buf, 24) : std::string(buf, 17);
}

bool Timestamp::valid() const {
    return microSecond_ > 0;
}


int64_t Timestamp::microSecondsFromEpoch() const {
    return microSecond_;
}

Timestamp::operator time_t() const {
    return microSecond_ / MicroSecondsPerSecond;
}

// time_t Timestamp::secondFromEpoch() const {
//     
// }

Timestamp Timestamp::now() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return Timestamp(tv.tv_sec * MicroSecondsPerSecond + tv.tv_usec);
}


Timestamp Timestamp::invalid() {
    return Timestamp();
}

bool webServer::operator<(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsFromEpoch() < rhs.microSecondsFromEpoch();
}

bool webServer::operator>(const Timestamp& lhs, const Timestamp& rhs) {
    return rhs < lhs;
}

bool webServer::operator==(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsFromEpoch() == rhs.microSecondsFromEpoch();
}

bool webServer::operator!=(const Timestamp& lhs, const Timestamp& rhs) {
    return !(lhs.microSecondsFromEpoch() == rhs.microSecondsFromEpoch());
}

double webServer::operator-(const Timestamp& lhs, const Timestamp& rhs) {
    return static_cast<double>(lhs.microSecond_ - rhs.microSecond_) / MicroSecondsPerSecond;
}

Timestamp webServer::operator+(const Timestamp& stamp, double seconds) {
    return Timestamp(stamp.microSecond_ + static_cast<int64_t>(seconds * MicroSecondsPerSecond));
}

Timestamp& webServer::operator+=(Timestamp& stamp, double seconds) {
    stamp.microSecond_ += static_cast<int64_t>(seconds * MicroSecondsPerSecond);
    return stamp;
}
