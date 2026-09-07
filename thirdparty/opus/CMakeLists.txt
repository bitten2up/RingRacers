add_library(opus INTERFACE)
target_include_directories(opus INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(opus INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/libs/arm64-v8a/libopus.so")
add_library(Opus::opus ALIAS opus)