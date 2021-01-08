#include "Timestamp.h"

#include <cinttypes>
#include <sys/time.h>

USE_NAMESPACE

std::string Timestamp::toString() const {
    char buf[32] = {0};
    const int64_t seconds = microSecond / MicroSecondsPerSecond;
    const int64_t microseconds = microSecond % MicroSecondsPerSecond;

#if __WORDSIZE == 64
    snprintf(buf, 32, "%ld. %06ld", seconds, microseconds);
#else
    snprintf(buf, 32, "%lld. %06lld", seconds, microseconds);
#endif
   
    return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
    auto seconds = static_cast<time_t>(microSecond / MicroSecondsPerSecond);
    seconds += 8 * 3600;
    tm tmTime{};
    gmtime_r(&seconds, &tmTime);

    if (showMicroseconds) {
        const auto microseconds = static_cast<int>(microSecond % MicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
                 tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec,
                 microseconds);
    }
    else {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
                 tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
    }
    return buf;
}

bool Timestamp::valid() const {
    return microSecond > 0;
}


int64_t Timestamp::microSecondFromEpoch() const {
    return microSecond;
}

time_t Timestamp::secondFromEpoch() const {
    return microSecond / MicroSecondsPerSecond;
}

Timestamp Timestamp::now() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return Timestamp(tv.tv_sec * MicroSecondsPerSecond + tv.tv_usec);
}


Timestamp Timestamp::invalid() {
    return Timestamp();
}
