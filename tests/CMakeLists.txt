file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(tests ${srcs})
target_include_directories(event_loop PUBLIC include)
target_link_libraries(tests PUBLIC event_loop)