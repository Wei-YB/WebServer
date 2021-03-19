#include "Thread.h"

#include <utility>
#include "Logger.h"
#include "ThisThread.h"
USE_NAMESPACE

class MainThreadInit {
public:
    MainThreadInit() {
        ThisThread::t_threadName = "main";
        ThisThread::tid();
    }
};

MainThreadInit init{};

class ThreadArgs {
public:
    ThreadArgs(const Thread::ThreadFunc& func, CountDownLatch* latch, const std::string& name, pid_t* tid) :
        func_(func), latchPtr_(latch), name_(name), tid_(tid){}

    void run() {
        *tid_ = ThisThread::tid();
        tid_ = nullptr;
        latchPtr_->countDown();
        latchPtr_ = nullptr;
        ThisThread::t_threadName = name_.c_str();
        try {
            func_();
            ThisThread::t_threadName = "finished";
        }
        catch (const std::exception& e) {
            ThisThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "exception: %s\n", e.what());
            abort();
        }
        catch (...) {
            ThisThread::t_threadName = "crashed";
            fprintf(stderr,"exception caught in Thread %s\n", name_.c_str());
            throw;
        }
    }

    Thread::ThreadFunc func_;
    CountDownLatch* latchPtr_;
    std::string name_;
    pid_t* tid_;

};

void* runThread(void* args) {
    auto* ptr = static_cast<ThreadArgs*>(args);
    ptr->run();
    delete ptr;
    return nullptr;
}

Thread::Thread(ThreadFunc func, const std::string& name) : func_(func),name_(name),latch_(1) {
    ++numCreated_;
    if (name.empty())
        setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_)
        pthread_detach(pthreadId_);
}

void Thread::start() {
    started_ = true;
    auto* args = new ThreadArgs(func_,&latch_,name_,&tid_);
    if(pthread_create(&pthreadId_, nullptr, runThread, static_cast<void*>(args))) {
        delete args;
        started_ = false;
        LOG_SYSERR << "Failed in pthread_create";
    }else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, nullptr);
}

const std::string& Thread::name() const {
    return name_;
}

int Thread::numCreated() {
    return numCreated_;
}

void Thread::setDefaultName() {
    name_ = "Thread " + std::to_string(numCreated_);
}

std::atomic_int32_t Thread::numCreated_;
