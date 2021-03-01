# CMakeList.txt: WebServerTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required (VERSION 3.8)

project ("WebServerTest")

aux_source_directory("./base" baseCodeFile )

add_library(base STATIC ${PROJECT_SOURCE_DIR}/${baseCodeFile} )

target_include_directories(base PUBLIC ${PROJECT_SOURCE_DIR}/base)

link_libraries(pthread)

set(Poller "Poller.h" "Poller.cpp" "Channel.h" "Channel.cpp")

set(Acceptor "Acceptor.h" "Acceptor.cpp")

set(EventLoop "EventLoop.h" "EventLoop.cpp")

set(InetAddress "InetAddress.cpp" "InetAddress.h")

set(Socket "Socket.h" "Socket.cpp")

set(Connection "Connection.h" "Connection.cpp")

link_libraries(base)

add_executable (main "WebServerTest.cpp" "WebServerTest.h" "HTTPParse.h" "HTTPParse.cpp" ${Poller} ${Acceptor} ${InetAddress} ${EventLoop} ${Socket} ${Connection})

add_executable(myTest "test/test.cpp" "InetAddress.h" "InetAddress.cpp")

add_executable(bufferTest "test/bufferTest.cpp")
# TODO: 如有需要，请添加测试并安装目标。
