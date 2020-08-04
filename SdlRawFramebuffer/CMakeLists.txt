cmake_minimum_required(VERSION 3.17)
project(SdlRawFramebuffer)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "/O2") # When using MSVC compiler

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


include_directories(.)
include_directories(../SDL2-devel-2.0.9-VC/SDL2-2.0.9/include)
link_directories(../SDL2-devel-2.0.9-VC/SDL2-2.0.9/lib)

add_executable(SdlRawFramebuffer
        _EntryPoint_.cpp
        BinHeap.cpp
        BinHeap.h
        ImmediateDraw.cpp
        ImmediateDraw.h
        ScanBufferDraw.cpp
        ScanBufferDraw.h
        ScanBufferFont.cpp
        ScanBufferFont.h
        Sort.cpp
        Sort.h)

target_link_libraries(SdlRawFramebuffer "${PROJECT_SOURCE_DIR}/../SDL2-devel-2.0.9-VC/SDL2-2.0.9/lib/x86/SDL2.lib")

