#include <gtest/gtest.h>
#include "../Mutex.h"

using webServer::Mutex;
using webServer::LockGuard;

int main()
{
    testing::InitGoogleTest();
    auto ret = RUN_ALL_TESTS();
    getchar();
}


TEST(Mutex, mutex)
{
    Mutex mtx;
    mtx.lock();
    auto* nativeMtx = mtx.getPthreadMutex();

    EXPECT_EQ(EBUSY, pthread_mutex_trylock(nativeMtx));
    EXPECT_EQ(0, pthread_mutex_unlock(nativeMtx));
    EXPECT_EQ(0, pthread_mutex_lock(nativeMtx));

    mtx.unlock();
    EXPECT_EQ(0, pthread_mutex_trylock(nativeMtx));
    mtx.unlock();
}

TEST(LockGuard, lockGuard)
{
    Mutex mtx;
    auto* nativeMtx = mtx.getPthreadMutex();

    {
        LockGuard guard(mtx);
        EXPECT_EQ(EBUSY, pthread_mutex_trylock(nativeMtx));
    }

    EXPECT_EQ(0, pthread_mutex_lock(nativeMtx));
    mtx.unlock();
}