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

file(GLOB SOURCES "src/*.cpp" "src/include/*.h")

add_executable(RisePakPatch
    ${SOURCES}
    ${MURMURHASH3_SOURCES}
)