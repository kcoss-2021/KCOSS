cmake_minimum_required(VERSION 3.16)
project(histo)

set(CMAKE_CXX_STANDARD 14)

add_executable(histo main.cpp concurrentqueue.h ThreadPool_stable.h Query.h File.h ConcurrentBloomfilter.h Tool.h Histo.h Cmdline.h)

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(histo Threads::Threads)