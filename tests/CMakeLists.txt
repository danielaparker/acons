#
# acons tests suite CMake file
#

cmake_minimum_required (VERSION 2.8)

# load global config
include (../../../build/cmake/config.cmake)

project (Tests CXX)

# load per-platform configuration
include (../../../build/cmake/${CMAKE_SYSTEM_NAME}.cmake)

file(GLOB_RECURSE UnitTests_sources ../../src/*.cpp)

add_executable(acons_tests
    ${UnitTests_sources}
)

target_compile_definitions (acons_tests PUBLIC)

if (NO_DEPRECATED)
add_definitions(-DJSONCONS_NO_DEPRECATED)
endif()

target_include_directories (acons_tests PUBLIC ../../../include
                                        PUBLIC ../../../third_party
                                        PRIVATE ../../include)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  # special link option on Linux because llvm stl rely on GNU stl
  target_link_libraries (acons_tests -Wl,-lstdc++)
endif()
