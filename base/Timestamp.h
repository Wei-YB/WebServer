#pragma once

#include "Util.h"
#include <cstdint>
#include <string>

START_NAMESPACE

constexpr int MicroSecondsPerSecond = 1000 * 1000;

class Timestamp {
public:

    Timestamp() : microSecond_(0) {}
    explicit Timestamp(int64_t microSecondFromEpoch) : microSecond_(microSecondFromEpoch) {}

    Timestamp(const Timestamp& timestamp)  = default;
    Timestamp(Timestamp&&)                 = default;
    Timestamp& operator=(const Timestamp&) = default;
    Timestamp& operator=(Timestamp&&)      = default;
    ~Timestamp()                           = default;

public:
    // std::string toString() const;

    void format(char* buf, size_t len, bool showMicroseconds) const;

    [[nodiscard]] std::string format(bool showMicroseconds = true) const;

    [[nodiscard]] bool valid() const;

    [[nodiscard]] int64_t microSecondsFromEpoch() const;

    // replace this func with an implicit type cast
    // time_t secondFromEpoch() const;
    explicit operator time_t() const;

    static Timestamp now();
    static Timestamp invalid();

    static Timestamp fromUnixTime(time_t t) {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds) {
        return Timestamp(t * MicroSecondsPerSecond + microseconds);
    }

private:
    static thread_local char buffer_[64];

    int64_t microSecond_;
public:
    friend double     operator-(const Timestamp&, const Timestamp&);
    friend Timestamp  operator+(const Timestamp&, double seconds);
    friend Timestamp& operator+=(Timestamp&, double seconds);
};

double operator-(const Timestamp&, const Timestamp&);

Timestamp operator+(const Timestamp&, double seconds);

Timestamp& operator+=(Timestamp&, double seconds);

bool operator<(const Timestamp& lhs, const Timestamp& rhs);

bool operator>(const Timestamp& lhs, const Timestamp& rhs);

bool operator==(const Timestamp& lhs, const Timestamp& rhs);

bool operator!=(const Timestamp& lhs, const Timestamp& rhs);

END_NAMESPACE
