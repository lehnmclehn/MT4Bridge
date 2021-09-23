cmake_minimum_required(VERSION 3.20)

project(MT4Bridge CXX)

set(CMAKE_CXX_STANDARD 14)


add_executable(MT4BridgeTest MT4BridgeTest.cpp)
target_include_directories(MT4BridgeTest PUBLIC C:/install/include)
target_include_directories(MT4BridgeTest PUBLIC "C:/Program Files/OpenSSL-Win32/include/")
target_link_libraries(MT4BridgeTest PUBLIC C:/install/lib/librabbitmq.4.dll.a)


add_library(MT4Bridge SHARED MT4BridgeTest.cpp MT4BridgeLibrary.cpp)
target_include_directories(MT4Bridge PUBLIC C:/install/include)
target_include_directories(MT4Bridge PUBLIC "C:/Program Files/OpenSSL-Win32/include/")

target_link_libraries(MT4Bridge PUBLIC C:/install/lib/librabbitmq.4.dll.a)
#target_link_libraries(MT4Bridge PUBLIC C:/install/lib/liblibrabbitmq.4.a)

install(TARGETS MT4Bridge DESTINATION bin)
