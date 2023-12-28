#include "utils.h"

std::vector<char> RisePakPatch::Utils::readAllBytes(const std::string& path) {
    std::ifstream     inputFile(path, std::ios::binary);
    std::vector<char> bytes((std::istreambuf_iterator<char>(inputFile)), (std::istreambuf_iterator<char>()));
    return bytes;
}

std::string RisePakPatch::Utils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string RisePakPatch::Utils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string RisePakPatch::Utils::replaceAllInString(const std::string& baseString, const std::string& stringToReplace, const std::string& replacement) {
    std::string result         = baseString;
    size_t      stringPosition = 0;

    while ((stringPosition = result.find(stringToReplace, stringPosition)) != std::string::npos) {
        result.replace(stringPosition, stringToReplace.length(), replacement);
        stringPosition += replacement.length();
    }

    return result;
}

std::wstring RisePakPatch::Utils::utf8ToUtf16(const std::string& utf8str) {
    std::wstring utf16str;
    utf16str.reserve(utf8str.length());

    for (size_t i = 0; i < utf8str.length();) {
        wchar_t utf16char = 0;

        if ((utf8str[i] & 0x80) == 0) {
            utf16char = utf8str[i];
            i++;
        } else if ((utf8str[i] & 0xE0) == 0xC0) {
            utf16char = (static_cast<wchar_t>(utf8str[i] & 0x1F) << 6) | (utf8str[i + 1] & 0x3F);
            i += 2;
        } else if ((utf8str[i] & 0xF0) == 0xE0) {
            utf16char = (static_cast<wchar_t>(utf8str[i] & 0x0F) << 12) | (static_cast<wchar_t>(utf8str[i + 1] & 0x3F) << 6) | (utf8str[i + 2] & 0x3F);
            i += 3;
        } else {
            return L"";
        }

        utf16str.push_back(utf16char);
    }

    return utf16str;
}

void RisePakPatch::Utils::writeAllBytes(const std::string& filename, const std::vector<char>& data) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        return;
    }

    file.write(data.data(), data.size());
    file.close();
}