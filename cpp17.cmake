# The explicit compiler setup here isn't ideal for building on other systems, but for projects
# targeting C++17 (via Clang 5 and libstdc++ 7 on my machine, neither of which has yet seen an
# official release), the setup is of necessity a little unusual for now. Once C++17 toolchains are
# more widely available, the need for this specialised setup will be obviated.

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CLANG_FLAGS "-Wall -Wextra -Wno-missing-braces")
set(KSR_GCC_TOOLCHAIN "" CACHE STRING "GCC toolchain path override; uses the system GCC toolchain if empty")
if(NOT ("${KSR_GCC_TOOLCHAIN}" STREQUAL ""))
    set(CLANG_FLAGS "--gcc-toolchain=${KSR_GCC_TOOLCHAIN} ${CLANG_FLAGS}")
endif()

if(CLANG_TIDY_PATH AND MYRIAD_ENABLE_LINT)
    set_target_properties(myriad PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_PATH}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CLANG_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z ${CLANG_FLAGS}")
