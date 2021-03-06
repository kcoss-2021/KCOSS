cmake_minimum_required(VERSION 3.14)
project(kmer_dump)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB SOURCE ./*.h ./*.c ./*.cpp)
add_executable(kmer_dump ${SOURCE})

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(kmer_dump Threads::Threads)

# __sync_bool_compare_and_swap_16
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mcx16")

