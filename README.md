# RisePakPatch
(C++) Compress loose files into pak

This was ripped from my original project ([REModman](https://github.com/ricochhet/REModman.cpp)), into a standalone release. With some minor changes.

## Building

Download the following tools if you do not have them already installed.
- Clang: [download here](https://github.com/llvm/llvm-project/releases) 
    - (or) LLVM-MINGW: [download here](https://github.com/mstorsjo/llvm-mingw)
- Ninja: [download here](https://github.com/ninja-build/ninja/releases)
- Cmake: [download here](https://cmake.org/download/)
- Run `build.ps1` to build.
    - Debug/Release can be specified via `-BuildType <Debug/Release>` (default: Debug)
## Usage
- `RisePakPatch <input> <output>`