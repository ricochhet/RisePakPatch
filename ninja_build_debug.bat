@echo off

set BUILD_DIR=ninja_build_debug

rem Create build directory if it does not exist
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

rem Change directory to build directory
cd %BUILD_DIR%

rem Generate build files using CMake
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -G "Ninja" ..

rem Build application using generated build files
ninja

rem Pause so user can see output
pause