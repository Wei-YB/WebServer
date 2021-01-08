# CMakeList.txt: Logging 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("Logging")

set(CMAKE_CXX_STANDARD 17)

set(Logger "Logger.cpp" "Logger.h")
set(Timestamp "Timestamp.h" "Timestamp.cpp")
set(LogStream "LogStream.cpp" "LogStream.h")
# 将源代码添加到此项目的可执行文件。
add_executable (main  "main.cpp" "Utli.h" Logger Timestamp  LogStream "Mutex.h")

# TODO: 如有需要，请添加测试并安装目标。
