cmake_minimum_required(VERSION 2.8)

project(TsetlinMachine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Compiler-specific flags and definitions
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -mavx512f")
endif()

add_subdirectory(src/tsetlin)
add_subdirectory(src/io)
add_subdirectory(src/pso)

include_directories(src/io)
include_directories(src/tsetlin)
include_directories(src/pso)

add_executable(xorDemo src/test/xor.cpp)
add_executable(siRNA src/test/siRNA.cpp)
add_executable(pso src/test/pso_siRNA.cpp)

target_link_libraries(xorDemo tmLib ioLib)
target_link_libraries(siRNA tmLib ioLib)
target_link_libraries(pso tmLib ioLib psoLib)
