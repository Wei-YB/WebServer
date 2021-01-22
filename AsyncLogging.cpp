#include "AsyncLogging.h"
#include "LogFile.h"

USE_NAMESPACE

AsyncLogging::AsyncLogging(const std::string& basename, off_t rollSize) :
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_([this]() { this->threadFunc(); }, "log thread"),
    currentBuffer_(std::make_unique<BufferPtr::element_type>()),
    nextBuffer_(std::make_unique<BufferPtr::element_type>()),
    latch_(1)
    , cond_(mutex_) {}


void AsyncLogging::append(const char* str, size_t len) {
    LockGuard guard(mutex_);    // make append thread safe
    // if size enough just append
    if (currentBuffer_->avail() > len)
    {
        currentBuffer_->append(str, len);
    }
    else {  // current buffer not enough, need to find another buffer
        buffers_.push_back(std::move(currentBuffer_));
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_ = std::make_unique<BufferPtr::element_type>();
        currentBuffer_->append(str, len);
        // notify the back end, a buffer is ready to write
        cond_.notify();
    }
}

void AsyncLogging::threadFunc() {
    // first init the argument
    latch_.countDown();
    LogFile logFile(basename_,rollSize_);


    BufferPtr buffer1 = std::make_unique<BufferPtr::element_type>();
    BufferPtr buffer2 = std::make_unique<BufferPtr::element_type>();

    BufferVec backEndBuffers;
    backEndBuffers.reserve(4);

    while (running_) {

        // entry the critical section
        // change buffer with the front end
        {
            LockGuard guard(mutex_);
            if (buffers_.empty())
            {
                cond_.wait(); // need at least one full buffer
            }
            buffers_.emplace_back(std::move(currentBuffer_)); // write current buffer
            backEndBuffers.swap(buffers_);
            currentBuffer_ = std::move(buffer1);
            if (!nextBuffer_)
                nextBuffer_ = std::move(buffer2);
        }

        for (auto& ptr : backEndBuffers)
            logFile.write(ptr->data(), ptr->length());

        if (backEndBuffers.size() > 2) // drop excess buffer
            backEndBuffers.resize(2);

        // backEndBuffer.size() supposed to be 2
        if (!buffer1) {
            buffer1 = std::move(backEndBuffers.back());
            backEndBuffers.pop_back();
            buffer1->reset();
        }

        if (!buffer2) {
            buffer2 = std::move(backEndBuffers.back());
            backEndBuffers.pop_back();
            buffer2->reset();
        }

        backEndBuffers.clear();
        logFile.flush();
    }
    logFile.flush();
}

