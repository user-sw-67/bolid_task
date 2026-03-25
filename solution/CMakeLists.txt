cmake_minimum_required(VERSION 3.10)

project(Loader VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(SOURCES
    src/main.cpp
)

add_executable(loader ${SOURCES})

target_include_directories(loader PRIVATE include)

if(WIN32)
    target_link_libraries(loader ws2_32)
endif()


