file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_executable(event_loop_test src/event_loop_test.cpp)
target_include_directories(event_loop PUBLIC include)
target_link_libraries(event_loop_test PUBLIC event_loop)

add_executable(co_test src/co_test.cpp)