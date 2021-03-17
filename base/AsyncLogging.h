#pragma once

#include <memory>
#include <vector>


#include "CountDownLatch.h"
#include "LogStream.h"
#include "Thread.h"

// AsyncLogging Class

START_NAMESPACE

class AsyncLogging {

public:
    AsyncLogging(const std::string& basename, off_t rollSize);

    void append(const char* str, size_t len);

    void start() {
        running_ = true;
        thread_.start();
        // make sure log thread start when start() returns
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    void threadFunc();

    bool running_;
    std::string basename_;

    const off_t rollSize_;


    using BufferPtr = std::unique_ptr<LogBuffer<LargeBufferSize>>;
    using BufferVec = std::vector<  BufferPtr>;

    Thread thread_;

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVec buffers_;
    CountDownLatch latch_;
    Mutex mutex_;
    Condition cond_;
        

};

END_NAMESPACE