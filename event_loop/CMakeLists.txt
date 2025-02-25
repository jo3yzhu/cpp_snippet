file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_library(event_loop STATIC ${srcs})
target_include_directories(event_loop PUBLIC include)