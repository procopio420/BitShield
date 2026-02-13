cmake_minimum_required(VERSION 3.12)
project(BitShield VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library target
add_library(bitshield
    src/bitstream.cpp
    src/codecs/repetition.cpp
    src/codecs/hamming74.cpp
    src/channel.cpp
    src/io.cpp
    src/metrics.cpp
)

target_include_directories(bitshield
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# CLI executable
add_executable(bitshield_cli
    apps/bitshield/main.cpp
)

target_link_libraries(bitshield_cli
    PRIVATE
        bitshield
)

set_target_properties(bitshield_cli PROPERTIES
    OUTPUT_NAME bitshield
)

# Tests
enable_testing()

add_executable(bitshield_tests
    tests/test_main.cpp
    tests/test_repetition.cpp
    tests/test_hamming74.cpp
    tests/test_channel.cpp
)

target_link_libraries(bitshield_tests
    PRIVATE
        bitshield
)

target_include_directories(bitshield_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

add_test(NAME bitshield_tests COMMAND bitshield_tests)

# Installation (optional)
install(TARGETS bitshield bitshield_cli
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/bitshield
    DESTINATION include
)

