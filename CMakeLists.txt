cmake_minimum_required(VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

add_compile_options(-fcoroutines)

project(cpp_snippet LANGUAGES CXX)

add_subdirectory(event_loop)
add_subdirectory(tests)