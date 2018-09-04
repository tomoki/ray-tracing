cmake_minimum_required (VERSION 3.9)
project (my-raytracing)

find_package(Threads)
add_executable(executable src/main.cpp)
target_link_libraries(executable ${CMAKE_THREAD_LIBS_INIT})
