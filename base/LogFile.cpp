#include "LogFile.h"

#include <cstdio>
#include "Timestamp.h"

#include "Logger.h"

USE_NAMESPACE

LogFile::MyFile::MyFile(const std::string& name) : file_ (fopen(name.c_str(), "ae")),bytesWrite_(0) {}

LogFile::MyFile::~MyFile() {   fclose(file_);}

void LogFile::MyFile::write(const char* str, size_t len) {
    auto n = fwrite_unlocked(str, sizeof(char), len, file_);
    auto remain = len - n;
    while (remain) {
        const auto x = fwrite_unlocked(str + n, sizeof(char), len, file_);
        if (x == 0) {
            const int err = ferror(file_);
            if (err)
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
            break;
        }
        n += x;
        remain -= x;
    }
    bytesWrite_ += n;
}


LogFile::LogFile(const std::string& basename, off_t rollSize, int flushInterval, int checkEveryN):
    basename_(basename), count_(0), checkEveryN_(checkEveryN), flushInterval_(flushInterval), rollSize_(rollSize),
    logFile_(std::make_unique<MyFile>(getLogFileName(basename_))), startOfPeriod_(), lastRoll_(), lastFlush_() { }


void LogFile::flush() const {
    logFile_->flush();
}

void LogFile::write(const char* log, size_t len) {
    logFile_->write(log, len);

    if (logFile_->bytesWrite() > rollSize_)
        roll();
    else {
        ++count_;
        if (count_ >= checkEveryN_) {
            count_ = 0;
            time_t now = time(nullptr);
            time_t thisPeriod = now / RollPerSeconds_ * RollPerSeconds_;
            if (thisPeriod != startOfPeriod_)
                roll();
            else if (now - lastFlush_ > flushInterval_) {
                flush();
                lastFlush_ = now;
            }
        }
    }
}

bool LogFile::roll() {
    time_t now = 0;
    const std::string filename = getLogFileName(basename_);
    const time_t start = now / RollPerSeconds_ * RollPerSeconds_;

    if (now > lastRoll_) {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        // reset log file
        logFile_ = std::make_unique<MyFile>(filename);
        return true;
    }
    return false;
}

std::string LogFile::getLogFileName(const std::string& basename) {
    auto now = Timestamp::now();
    return basename + now.format(false) + ".log";
}


