// #include "../CountDownLatch.h"
#include "..//Mutex.h"
// #include "../Thread.h"
#include "../Timestamp.h"

#include <vector>
#include <cstdio>
#include <memory>
#include <cerrno>
#include <cstring>
#include <thread>

USE_NAMESPACE
using namespace std;

Mutex g_mutex;
vector<int> g_vec;
const int kCount = 10 * 1000 * 1000;

void threadFunc() {
    for (int i = 0; i < kCount; ++i) {
        LockGuard lock(g_mutex);
        g_vec.push_back(i);
    }
}

int foo() __attribute__((noinline));

int g_count = 0;

int foo() {
    LockGuard lock(g_mutex);
    if (!g_mutex.isLockedByThisThread()) {
        printf("FAIL\n");
        return -1;
    }

    ++g_count;
    return 0;
}

int main() {
    auto ret = foo();
    if (ret) {
        printf("error : %s", strerror(errno));
    }
    if (g_count != 1) {
        printf("MCHECK calls twice.\n");
        abort();
    }

    const int kMaxThreads = 8;
    g_vec.reserve(kMaxThreads * kCount);

    Timestamp start(Timestamp::now());
    for (int i = 0; i < kCount; ++i) {
        g_vec.push_back(i);
    }

    printf("single thread without lock %f\n", timeDifference(Timestamp::now(), start));

    start = Timestamp::now();
    threadFunc();
    printf("single thread with lock %f\n", timeDifference(Timestamp::now(), start));

    for (int nthreads = 1; nthreads < kMaxThreads; ++nthreads) {
        std::vector<std::unique_ptr<thread>> threads;
        g_vec.clear();
        start = Timestamp::now();
        for (int i = 0; i < nthreads; ++i) {
            threads.emplace_back(new thread(&threadFunc));
            // threads.back()->start();
        }
        for (int i = 0; i < nthreads; ++i) {
            threads[i]->join();
        }
        printf("%d thread(s) with lock %f\n", nthreads, timeDifference(Timestamp::now(), start));
    }
}
