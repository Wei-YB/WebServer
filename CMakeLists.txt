# CMakeList.txt: Logging 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("Logging")

set(CMAKE_CXX_STANDARD 17)


set(Timestamp "Timestamp.h" "Timestamp.cpp")
set(LogStream "LogStream.h" "LogStream.cpp" )
set(Logger "Logger.h" "Logger.cpp"  ${LogStream} ${Timestamp})

message(${Logger})

set(Condition "Condition.cpp" "Condition.h")
set(Mutex "Mutex.h"  "Mutex.cpp")
set(CountDownLatch "CountDownLatch.h" "CountDownLatch.cpp" ${Mutex} ${Condition})

set(Thread "ThisThread.h" "ThisThread.cpp" "Thread.h" "Thread.cpp" )


# 将源代码添加到此项目的可执行文件。

link_libraries("pthread")
add_executable(Mutex_text "./test/mutex_text.cpp" ${Mutex} ${Condition} ${Timestamp})
add_executable (main  "main.cpp" "Utli.h" ${Logger})
add_executable(thread_test "./test/thread_text.cpp" "Utli.h" ${Thread} ${CountDownLatch} ${Logger})

# TODO: 如有需要，请添加测试并安装目标。
