#ifndef RISEPAKPATCH_UTILS_INCLUDED
#define RISEPAKPATCH_UTILS_INCLUDED

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace Utils {
    std::string       replaceAllInString(const std::string& baseString, const std::string& stringToReplace, const std::string& replacement);
    std::vector<char> readAllBytes(const std::string& path);
    std::string       toLower(const std::string& str);
    std::string       toUpper(const std::string& str);
    std::wstring      utf8ToUtf16(const std::string& utf8str);
}

#endif  // RISEPAKPATCH_UTILS_INCLUDED