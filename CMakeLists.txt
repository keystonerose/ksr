cmake_minimum_required(VERSION 3.6)
project(ksr)

set(3RDPARTY_DIR "3rdparty")

include(cpp17.cmake)
include_directories(include)
include_directories(SYSTEM ${3RDPARTY_DIR})

add_subdirectory(test)
add_executable(ksr_test ${KSR_TEST_SRCS})
target_compile_definitions(ksr_test PRIVATE KSR_THROW_ON_ASSERT)
