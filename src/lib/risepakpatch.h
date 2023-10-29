#ifndef RISEPAKPATCH_INCLUDED
#define RISEPAKPATCH_INCLUDED

#pragma once

#include "logger.h"
#include "murmurhash.h"
#include "utils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace RisePakPatch {
    struct FileEntry {
        std::string fileName;
        uint32_t    fileNameLower;
        uint32_t    fileNameUpper;
        uint64_t    offset;
        uint64_t    uncompSize;
    };

    void processDirectory(const std::string& path, const std::string& outputFile);
}

#endif  // RISEPAKPATCH_INCLUDED