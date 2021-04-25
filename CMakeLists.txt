# CMakeList.txt: WebServerTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required (VERSION 3.8)

project ("WebServerTest")

set(CMAKE_CXX_STANDARD 17)

aux_source_directory("./base" baseCodeFile )
aux_source_directory("./http" httpCodeFile)

add_library(base STATIC ${PROJECT_SOURCE_DIR}/${baseCodeFile} )
add_library(http STATIC ${PROJECT_SOURCE_DIR}/${httpCodeFile} )

target_include_directories(base PUBLIC ${PROJECT_SOURCE_DIR}/base)
target_include_directories(http PUBLIC ${PROJECT_SOURCE_DIR}/http)

set(Poller "Poller.h" "Poller.cpp" "Channel.h" "Channel.cpp")

set(Acceptor "Acceptor.h" "Acceptor.cpp")

set(EventLoop "EventLoop.h" "EventLoop.cpp")

set(InetAddress "InetAddress.cpp" "InetAddress.h")

set(Socket "Socket.h" "Socket.cpp")

set(Connection "Connection.h" "Connection.cpp" ${Socket} ${InetAddress})

set(Thread  ${EventLoop} "EventLoopThread.h" "EventLoopThread.cpp" "EventLoopThreadPool.h" "EventLoopThreadPool.cpp")

set(Buffer "Buffer.cpp" "Buffer.h")

set(Server "TCPServer.cpp" "TCPServer.h")

add_executable (main "WebServerTest.cpp" "WebServerTest.h" ${Poller} ${Connection} ${Acceptor} ${Thread} ${Buffer} ${Server})
target_link_libraries(main base http pthread)


add_executable(TimestampTest "./base/test/TimestampTest.cpp")
target_link_libraries(TimestampTest base gtest gtest_main pthread)
# TODO: 如有需要，请添加测试并安装目标。
