#pragma once

#include "Utli.h"
#include <cstdint>
#include <string>

START_NAMESPACE

constexpr int MicroSecondsPerSecond = 1000 * 1000;

class Timestamp {
public:
    Timestamp() : microSecond(0) {}
    explicit Timestamp(int64_t microSecondFromEpoch) : microSecond(microSecondFromEpoch) {}
    Timestamp(const Timestamp& timestamp) : microSecond(timestamp.microSecond) {}

    std::string toString() const;

    std::string toFormattedString(bool showMicroseconds = true) const;

    bool valid() const;

    int64_t microSecondFromEpoch() const;
    time_t secondFromEpoch() const;

    static Timestamp now();
    static Timestamp invalid();

    static Timestamp fromUnixTime(time_t t) {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds) {
        return Timestamp(t * MicroSecondsPerSecond * t + microseconds);
    }

private:
    int64_t microSecond;
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondFromEpoch() < rhs.microSecondFromEpoch();
}

inline bool operator>(const Timestamp& lhs, const Timestamp& rhs) {
    return rhs < lhs;
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondFromEpoch() == rhs.microSecondFromEpoch();
}

END_NAMESPACE