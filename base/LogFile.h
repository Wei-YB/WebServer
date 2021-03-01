#pragma once

#include <cstdio>   // file io
#include <memory>
#include <string>   // std::string
#include "Util.h"

START_NAMESPACE

class LogFile {
    
private:
    class MyFile {
    public:
        MyFile(const std::string& name);
        ~MyFile();

        void write(const char* str, size_t len);

        void flush() { ::fflush(file_); }

        off_t bytesWrite() const { return bytesWrite_; }

    private:
        FILE* file_;
        off_t bytesWrite_;
    };
public:
    // TODO
    LogFile(const std::string& basename, off_t rollSize, int flushInterval = 3, int checkEveryN = 1024);
    void flush() const;
    
    void write(const char* log, size_t len);

private:
    bool roll();
    static std::string getLogFileName(const std::string& basename);

    std::string basename_;

    int count_;
    const int checkEveryN_;
    const int flushInterval_;
    const off_t rollSize_;

    std::unique_ptr<MyFile> logFile_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;

    const int RollPerSeconds_ = 60 * 60 * 24;
};



END_NAMESPACE
