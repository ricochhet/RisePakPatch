#ifndef _PAKTOOL_UTILS_INCLUDED
#define _PAKTOOL_UTILS_INCLUDED

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace RisePakPatch {
    class Utils {
    public:
        static std::string       replaceAllInString(const std::string& baseString, const std::string& stringToReplace, const std::string& replacement);
        static std::vector<char> readAllBytes(const std::string& path);
        static std::string       toLower(const std::string& str);
        static std::string       toUpper(const std::string& str);
        static std::wstring      utf8ToUtf16(const std::string& utf8str);
        static void              writeAllBytes(const std::string& filename, const std::vector<char>& data);
    };
}

#endif  // _PAKTOOL_UTILS_INCLUDED