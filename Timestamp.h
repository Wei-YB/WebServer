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

    int64_t microSecondsFromEpoch() const;
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
    return lhs.microSecondsFromEpoch() < rhs.microSecondsFromEpoch();
}

inline bool operator>(const Timestamp& lhs, const Timestamp& rhs) {
    return rhs < lhs;
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsFromEpoch() == rhs.microSecondsFromEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low) {
    int64_t diff = high.microSecondsFromEpoch() - low.microSecondsFromEpoch();
    return static_cast<double>(diff) / MicroSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
inline Timestamp addTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * MicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsFromEpoch() + delta);
}


END_NAMESPACE
