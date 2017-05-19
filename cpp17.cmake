# The explicit compiler setup here isn't ideal for building on other systems, but for projects
# targeting C++17 (via Clang 5 and libstdc++ 7 on my machine, neither of which has yet seen an
# official release), the setup is of necessaity a little unusual for now. Once C++17 toolchains are
# more widely available, the need for this specialised setup will be obviated.

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CLANG_FLAGS "--gcc-toolchain=/opt/gcc7 -Wall -Wextra -Wno-missing-braces")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CLANG_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z ${CLANG_FLAGS}")
