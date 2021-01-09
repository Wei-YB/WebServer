
#include "Condition.h"

#include <cstdint>
#include <cerrno>


USE_NAMESPACE

Condition::Condition(Mutex& lock): mutex_(lock) {
    pthread_cond_init(&cond_, nullptr);
}

Condition::~Condition() {
    pthread_cond_destroy(&cond_);
}

void Condition::wait() {
    // countDownLatch need to use wait without lock
    LockGuard lock(mutex_);
    waitWithMutexLocked();
}

bool Condition::waitForSeconds(double seconds) {
    timespec time{};
    clock_gettime(CLOCK_REALTIME, &time);
    constexpr int64_t NanoSecondsPerSecond = 1000000000;
    const auto nanoseconds = static_cast<int64_t>(seconds * NanoSecondsPerSecond);
    time.tv_sec += static_cast<time_t>((time.tv_nsec + nanoseconds) / NanoSecondsPerSecond);
    time.tv_nsec = static_cast<long>((time.tv_nsec + nanoseconds) % NanoSecondsPerSecond);

    LockGuard lock(mutex_);
    return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getPthreadMutex(), &time);
}

void Condition::notify() {
    pthread_cond_signal(&cond_);
}

void Condition::notifyAll() {
    pthread_cond_broadcast(&cond_);
}

void Condition::waitWithMutexLocked() {
    pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
}




