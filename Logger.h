#pragma once

#include <iostream>
#include "Utli.h"

#include "Timestamp.h"
#include "LogStream.h"
#include <cstring>
START_NAMESPACE

class Logger {
public:

    // define of the log event level
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    // try to find the file name in compiler time
    class SourceFile {
    public:

        // the input filename contains absolute path, 
        // so we need to Intercepts the File Name that does not contain a path.
        template <size_t N>
        SourceFile(const char (&filename)[N]): data_(filename), size_(N - 1) {
            const char* slash = strrchr(data_, '/'); // find first slash from right side
            if (slash) {
                data_ = slash + 1; // now data point to the origin name

                // size_ = static_cast<int>(strlen(data_));   
                size_ = size_ - (data_ - filename); // update size
            }
        }

        // almost same as previous function, but need to calculate the size of string
        explicit  SourceFile(const char* filename):data_(filename) {
            const char* slash = strrchr(filename, '/');
            if(slash) {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        size_t size_;
    };


    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return realLogger_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);


    using OutputFunc = void(*)(const char*, size_t);
    using FlushFunc = void(*)();

    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

    // don't need
    // static void setTimeZone(const TimeZone& tz);

private:
    class MyLogger {
    public:
        MyLogger(LogLevel level, int savedErrno, const SourceFile& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    MyLogger realLogger_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel() {
    return g_logLevel;
}


#define LOG_TRACE if (NAMESPACE Logger::logLevel() <= NAMESPACE Logger::LogLevel::TRACE) \
  NAMESPACE Logger(__FILE__, __LINE__, NAMESPACE Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (NAMESPACE Logger::logLevel() <= NAMESPACE Logger::LogLevel::DEBUG) \
  NAMESPACE Logger(__FILE__, __LINE__, NAMESPACE Logger::DEBUG, __func__).stream()
#define LOG_INFO if (NAMESPACE Logger::logLevel() <= NAMESPACE Logger::LogLevel::INFO) \
  NAMESPACE Logger(__FILE__, __LINE__).stream()
#define LOG_WARN NAMESPACE Logger(__FILE__, __LINE__, NAMESPACE Logger::LogLevel::WARN).stream()
#define LOG_ERROR NAMESPACE Logger(__FILE__, __LINE__, NAMESPACE Logger::LogLevel::ERROR).stream()
#define LOG_FATAL NAMESPACE Logger(__FILE__, __LINE__, NAMESPACE Logger::LogLevel::FATAL).stream()
#define LOG_SYSERR NAMESPACE Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL NAMESPACE Logger(__FILE__, __LINE__, true).stream()


const char* strerror_tl(int savedErrno);

END_NAMESPACE
