#include <gtest/gtest.h>

#include "../Timestamp.h"

using webServer::Timestamp;

int main() {
    testing::InitGoogleTest();
    auto ret =  RUN_ALL_TESTS();
    getchar();
}

TEST(Timestamp, IsConstructorRight) {
    Timestamp t1;
    Timestamp t2(10000);
    Timestamp t3(t2);
    EXPECT_TRUE(t2 == t3);
    EXPECT_TRUE(t1.microSecondsFromEpoch() == 0);
    EXPECT_TRUE(t2.microSecondsFromEpoch() == 10000);
    EXPECT_TRUE(t3.microSecondsFromEpoch() == 10000);
}

TEST(Timestamp, RightRelation) {
    Timestamp t1(10000);
    Timestamp t2(10001);
    EXPECT_TRUE(t1 < t2);
    EXPECT_TRUE(t2 > t1);
    EXPECT_FALSE(t2 < t1);
    EXPECT_FALSE(t1 > t2);
}

TEST(Timestamp, RightFormat) {
    Timestamp t1(1619364522568123ll);
    Timestamp t2(1619364522568000ll);
    const auto onlySec = t1.toFormattedString(false);
    const auto withMicroSec = t1.toFormattedString(true);
    EXPECT_EQ(onlySec, std::string("20210425 23:28:42"));
    EXPECT_EQ(withMicroSec, std::string("20210425 23:28:42.568123"));
    EXPECT_EQ(t2.toFormattedString(), std::string("20210425 23:28:42.568000"));
}

TEST(Timestamp, convert) {
    Timestamp t1(1619364522568123ll);
    time_t secs = t1.secondFromEpoch();
    const auto t2 = Timestamp::fromUnixTime(secs);
    const auto t3 = Timestamp::fromUnixTime(secs, 568123);
    const auto t4 = Timestamp::fromUnixTime(secs, 568122);
    EXPECT_EQ(t1.toFormattedString(false), t2.toFormattedString(false));
    EXPECT_EQ(t1, t3);
    EXPECT_NE(t1, t4);
}