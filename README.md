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
    - Formatting is done via `clang-format -i src/lib/*.cpp`
## Usage
- `RisePakPatch <patch/extract> <input> <output>`
    - Patch and Extract by default will create a separate `*.data` file. This is a lookup table created to ensure extracting of data can keep file paths intact.
    - `patch2` is a function that will embed the lookup table at the end of the file. The file contains a modified structure that contains the lookup table data as well as the size of the lookup table. 
    - `extract2` is a function that will read an embedded lookup table based on the tables size (uint32).