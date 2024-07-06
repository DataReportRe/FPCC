cmake_minimum_required(VERSION 3.4)


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

add_definitions(${LLVM_DEFINITIONS})
add_definitions(-D_GNU_SOURCE)
add_definitions(-D__STDC_CONSTANT_MACROS)
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-D__STDC_LIMIT_MACROS)
include_directories($ENV{LLVM_INCLUDE_DIRS})
link_directories($ENV{LLVM_LIBRARY_DIRS})

add_subdirectory(FPCCSan)
