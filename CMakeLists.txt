cmake_minimum_required(VERSION 3.19)
project(brainfuck_jit)

set(CMAKE_CXX_STANDARD 17)
set(ASMJIT_STATIC ON CACHE BOOL "Build static-linked asmjit" FORCE)

add_subdirectory(3rdparty/asmjit)
include_directories(3rdparty/asmjit/src)

add_executable(brainfuck_jit src/main.cpp)
target_link_libraries(brainfuck_jit asmjit)
