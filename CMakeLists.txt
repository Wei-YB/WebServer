# CMakeList.txt: WebServerTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

cmake_minimum_required (VERSION 3.2)

project ("WebServerTest")

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(base)
add_subdirectory(http)


set(Poller "Poller.h" "Poller.cpp" "Channel.h" "Channel.cpp")

set(Acceptor "Acceptor.h" "Acceptor.cpp")

set(EventLoop "EventLoop.h" "EventLoop.cpp")

set(InetAddress "InetAddress.cpp" "InetAddress.h")

set(Socket "Socket.h" "Socket.cpp")

set(Connection "Connection.h" "Connection.cpp" ${Socket} ${InetAddress})

set(Thread  ${EventLoop} "EventLoopThread.h" "EventLoopThread.cpp" "EventLoopThreadPool.h" "EventLoopThreadPool.cpp")

set(Buffer "Buffer.cpp" "Buffer.h")

set(Server "TCPServer.cpp" "TCPServer.h")

# add_executable (main "WebServerTest.cpp" "WebServerTest.h" ${Poller} ${Connection} ${Acceptor} ${Thread} ${Buffer} ${Server})
# target_link_libraries(main base http pthread)


add_executable(nettest "test/nettest.cpp" ${Socket} ${InetAddress})
target_link_libraries(nettest base pthread)

# add_executable(TimestampTest "base/test/test_Timestamp.cpp")
# add_executable(ThisThreadTest "./base/test/ThisThreadTest.cpp")
# target_link_libraries(TimestampTest base gtest gtest_main pthread)
# target_link_libraries(ThisThreadTest base gtest gtest_main pthread)
# TODO: 如有需要，请添加测试并安装目标。
