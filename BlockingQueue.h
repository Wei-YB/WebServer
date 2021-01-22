#pragma once

// push and take on different end
#include <queue>


#include "Utli.h"
#include "Mutex.h"
#include "Condition.h"

START_NAMESPACE

template <typename T>
class BlockingQueue {
public:
    BlockingQueue() : condition_(mutex_) {}

    void put(T&& val) {
        queue_.push(std::forward<T>(val));
        condition_.notify();
    }

    T take() {
        LockGuard lock(mutex_);
        while (queue_.empty())
            condition_.wait();
        auto ret = queue_.front();
        queue_.pop();
        return ret;
    }

    [[nodiscard]] size_t size() const {
        LockGuard lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<T> queue_;
    Mutex mutex_;
    Condition condition_;
};

END_NAMESPACE
