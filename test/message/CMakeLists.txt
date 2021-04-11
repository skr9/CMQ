cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)
project(msg_test C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(Threads REQUIRED)
message(STATUS "thread library found.....")

find_package(Qt5 COMPONENTS Core REQUIRED)
message(STATUS "Qt5 found------")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

IF (WIN32)
    message(STATUS "current platform is windows....")
    include_directories(C:/Qt/Qt5.14.2/5.14.2/mingw73_64/include)
    include_directories(C:/Qt/Qt5.14.2/5.14.2/mingw73_64/include/QtCore)
    link_directories(C:/Qt/Qt5.14.2/5.14.2/mingw73_64/lib)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
ELSEIF (UNIX AND NOT APPLE)
    message(STATUS "current platform is linux.....")
    include_directories(/usr/local/qt/include)
    link_directories(/usr/local/qt/lib)
    link_directories(/usr/local/lib)
ENDIF ()

include_directories(../../include/message)
include_directories(../../include/common)
include_directories(/usr/local/include/gtest)

add_definitions("-Wall -g")

enable_testing()

aux_source_directory(../../src/message MSG_FILES)
add_executable(msg_test cmqmessage_test.cpp ${MSG_FILES})

IF (WIN32)
    message(STATUS "libQt5Core.a loaded.....")
    target_link_libraries(msg_test libQt5Core.a)
ELSE ()
    target_link_libraries(msg_test libQt5Core.so gtest_main gtest)
ENDIF ()

include(GoogleTest)
gtest_discover_tests(msg_test)
   


