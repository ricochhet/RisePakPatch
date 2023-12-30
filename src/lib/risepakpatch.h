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
#include "reader.h"
#include "utils.h"
#include "writer.h"

namespace RisePakPatch {
    struct FileEntry {
        std::string fileName;
        uint32_t    fileNameLower;
        uint32_t    fileNameUpper;
        uint64_t    offset;
        uint64_t    uncompSize;
    };

    void processDirectory(const std::string& path, const std::string& outputFile, const bool& embedLookupTable);
    void extractDirectory(const std::string& inputFile, const std::string& outputDirectory, const bool& embedLookupTable);

    void writeLookupTableToFile(const std::string& lookupFile);
    void writeLookupTable(Writer& writer);

    void readLookupTableFromFile(const std::string& lookupFile);
    void readLookupTable(Reader& reader);
}

#endif  // PAKTOOL_INCLUDED