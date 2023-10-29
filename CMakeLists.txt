cmake_minimum_required(VERSION 3.16)
project(RisePakPatch)

set(CMAKE_CXX_STANDARD 23)

set(MURMURHASH3_SOURCES
    "${CMAKE_SOURCE_DIR}/dependencies/MurmurHash3/MurmurHash3.h"
    "${CMAKE_SOURCE_DIR}/dependencies/MurmurHash3/MurmurHash3.cpp"
)

include_directories(
    ${CMAKE_SOURCE_DIR}/dependencies/MurmurHash3
    ${CMAKE_SOURCE_DIR}/src
)

file(GLOB MAIN_SOURCES "src/*.cpp" "src/lib/*.h")
file(GLOB LIB_SOURCES "src/lib/*.cpp" "src/lib/*.h")

add_executable(RisePakPatch
    ${MAIN_SOURCES}
    ${LIB_SOURCES}
    ${MURMURHASH3_SOURCES}
)