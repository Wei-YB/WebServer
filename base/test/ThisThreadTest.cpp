#include "gtest/gtest.h"
#include "../ThisThread.h"

#include <thread>

using webServer::ThisThread::tid;
using webServer::ThisThread::tidString;
using webServer::ThisThread::tidStringLength;
using webServer::ThisThread::isMainThread;

volatile int t1;
volatile int t2;
volatile int mainThread;

int main() {
    testing::InitGoogleTest();
    auto ret = RUN_ALL_TESTS();
    getchar();
}

TEST(ThisThread, tid) {
    mainThread = tid();
    std::thread ([]() {t1 = tid();}).join();
    std::thread ([]() {t2 = tid();}).join();
    EXPECT_NE(t1, t2);
    EXPECT_EQ(mainThread, tid());
}

TEST(ThisThread, tidString) {
    const auto* const str = tidString();
    const auto        len = tidStringLength();
    EXPECT_EQ(strlen(str), len);
}


TEST(ThisThread, isMainThread) {
    EXPECT_TRUE(isMainThread());
}