cmake_minimum_required(VERSION 3.22.1)

project(psoTsetlinMachine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Compiler-specific flags and definitions
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -mavx512f")
endif()

add_subdirectory(src/tsetlin)
add_subdirectory(src/pso)

include_directories(src/tsetlin)
include_directories(src/pso)

add_executable(psotest src/test/psoTest.cpp)
add_executable(xorDemo src/test/xor.cpp)
add_executable(siRNA src/test/siRNA.cpp)
add_executable(pso src/test/pso_siRNA.cpp)

target_link_libraries(psotest tmLib psoLib)
target_link_libraries(xorDemo tmLib )
target_link_libraries(siRNA tmLib )
target_link_libraries(pso tmLib psoLib)
