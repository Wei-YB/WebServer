// Logging.cpp: 定义应用程序的入口点。
//

#include "Logger.h"

// this_thread :: thread_id
#include <thread>

START_NAMESPACE

__thread char t_errnobuf[512];
__thread char currentTime[32];
__thread time_t t_lastSecond;


const char* strerror_tl(int savedErrno) {
    strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
    return t_errnobuf;
}

Logger::LogLevel initLogLevel() {
    if (getenv("LOG_TRACE"))
        return Logger::LogLevel::TRACE;
    if (getenv("LOG_DEBUG"))
        return Logger::LogLevel::DEBUG;
    return Logger::LogLevel::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* logLevelName[static_cast<int>(Logger::LogLevel::NUM_LOG_LEVELS)] = {
    "TRACE ",
    "DEBUG ",
    "INFO ",
    "WARN ",
    "ERRNO",
    "FATAL ",
};

// helper class for write str to buffer
class T {  // NOLINT(clang-diagnostic-padded)
public:
    T(const char* str, unsigned len): str_(str), len_(len) {}
    const char* str_;
    const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v) {
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
    s.append(v.data_, v.size_);
    return s;
}

void defaultOutput(const char* msg, size_t len) {
    size_t n = fwrite(msg, 1, len, stdout);
    //FIXME check n
    (void)n;
}

void defaultFlush() {
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

END_NAMESPACE


USE_NAMESPACE

Logger::MyLogger::MyLogger(LogLevel level, int savedErrno, const SourceFile& file, int line):
    time_(Timestamp::now()), level_(level), line_(line), basename_(file) {
    formatTime();

    // TODO append current thread id to the buffer
    
    // std::thread::id tid = std::this_thread::get_id();
    // stream_ << static_cast<unsigned long int>(tid);

    stream_ << T(logLevelName[static_cast<int>(level)], 6);
    if (savedErrno) {
        stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}


void Logger::MyLogger::formatTime() {
    const auto times = time_.microSecondsFromEpoch();
    const auto seconds = times / MicroSecondsPerSecond;

    if (seconds != t_lastSecond) {
        // need to update the curTime
        const auto time = time_.toFormattedString(false);
        memcpy(currentTime, time.c_str(), 17);
    }

    char microSeconds[8];
    microSeconds[0] = '.';
    snprintf(microSeconds + 1, sizeof microSeconds - 1,
             "%06d", static_cast<int>(times % MicroSecondsPerSecond));
    // this part need to update
    
    stream_ << T(currentTime,17) << T(microSeconds,8);
}

void Logger::MyLogger::finish() {
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line):
    realLogger_(LogLevel::INFO, 0, file, line) {}

Logger::Logger(SourceFile file, int line, LogLevel level) :
    realLogger_(level, 0, file, line) {}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func) :
    realLogger_(level, 0, file, line) {
    realLogger_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort) :
    realLogger_(toAbort ? LogLevel::FATAL : LogLevel::ERROR, errno, file, line) {}

Logger::~Logger() {
    realLogger_.finish();
    const auto& buf = stream().buffer();
    g_output(buf.data(), buf.length());
    if (realLogger_.level_ == LogLevel::FATAL) {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(LogLevel level) {
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out) {
    g_output = out;
}

void Logger::setFlush(FlushFunc flush) {
    g_flush = flush;
}
