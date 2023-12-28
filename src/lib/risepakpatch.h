#ifndef PAKTOOL_INCLUDED
#define PAKTOOL_INCLUDED

#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "logger.h"
#include "murmurhash.h"
#include "utils.h"

namespace RisePakPatch {
    struct FileEntry {
        std::string fileName;
        uint32_t    fileNameLower;
        uint32_t    fileNameUpper;
        uint64_t    offset;
        uint64_t    uncompSize;
    };

    void processDirectory(const std::string& path, const std::string& outputFile);
    void extractDirectory(const std::string& inputFile, const std::string& outputDirectory);

    void writeLookupTable(const std::string& lookupFile);
    void readLookupTable(const std::string& lookupFile);
}

#endif  // PAKTOOL_INCLUDED