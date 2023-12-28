#include "risepakpatch.h"

#include <unordered_map>

#include "reader.h"
#include "writer.h"

#define MAX_SINGLE_FILE_SIZE 1073741824

std::unordered_map<uint32_t, std::string> reverseLookupTable;
std::string                               reverseLookup(uint32_t hash) {
    auto it = reverseLookupTable.find(hash);
    if (it != reverseLookupTable.end()) {
        return it->second;
    }
    return "";
}

void RisePakPatch::processDirectory(const std::string& path, const std::string& outputFile, const bool& embedLookupTable) {
    std::string directory = std::filesystem::absolute(path).string();

    if (!std::filesystem::is_directory(directory)) {
        return;
    }

    if (std::filesystem::exists(outputFile)) {
        LOG("Deleting existing output file...", LogLevel::Info);
        std::filesystem::remove(outputFile);
    }

    std::vector<std::string> sortedFiles;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(directory) / "natives")) {
        if (std::filesystem::is_regular_file(entry)) {
            sortedFiles.emplace_back(entry.path().string());
        }
    }
    std::sort(sortedFiles.begin(), sortedFiles.end(), [](const std::string& a, const std::string& b) {
        if (std::filesystem::path(a).parent_path() == std::filesystem::path(b).parent_path()) {
            return std::filesystem::path(a).filename() < std::filesystem::path(b).filename();
        }
        return std::filesystem::path(a).parent_path() < std::filesystem::path(b).parent_path();
    });

    LOG("Processing " + std::to_string(sortedFiles.size()) + " files", LogLevel::Info);
    std::vector<FileEntry> list;
    Writer                 writer(outputFile);
    writer.writeUInt32(1095454795u);
    writer.writeUInt32(4u);
    writer.writeUInt32((uint32_t)sortedFiles.size());
    writer.writeUInt32(0u);
    writer.seek(48 * sortedFiles.size() + writer.position());

    for (const std::string& obj : sortedFiles) {
        FileEntry   fileEntry2;
        std::string text = Utils::replaceAllInString(obj, directory, "");
        text             = Utils::replaceAllInString(text, "\\", "/");

        if (text[0] == '/') {
            text.erase(0, 1);
        }

        uint32_t          hash   = murmurhash3_wstr(Utils::toLower(text).c_str(), UINT_MAX);
        uint32_t          hash2  = murmurhash3_wstr(Utils::toUpper(text).c_str(), UINT_MAX);
        std::vector<char> array2 = Utils::readAllBytes(obj);

        fileEntry2.fileName      = text;
        fileEntry2.offset        = (uint64_t)writer.position();
        fileEntry2.uncompSize    = (uint64_t)array2.size();
        fileEntry2.fileNameLower = hash;
        fileEntry2.fileNameUpper = hash2;

        list.emplace_back(fileEntry2);
        writer.write(array2.data(), array2.size());

        reverseLookupTable[hash]  = text;
        reverseLookupTable[hash2] = text;
    }

    writer.seekFromBeginning(16);
    for (const FileEntry& item : list) {
        LOG(item.fileName + " " + std::to_string(item.fileNameUpper) + " " + std::to_string(item.fileNameLower), LogLevel::Info);
        writer.writeUInt32(item.fileNameLower);
        writer.writeUInt32(item.fileNameUpper);
        writer.writeUInt64(item.offset);
        writer.writeUInt64(item.uncompSize);
        writer.writeUInt64(item.uncompSize);
        writer.writeUInt64(0uL);
        writer.writeUInt32(0u);
        writer.writeUInt32(0u);
    }

    writer.seekFromEnd(0);
    uint32_t lookupTableSize = 0;
    if (embedLookupTable) {
        for (const auto& entry : reverseLookupTable) {
            uint32_t firstSize  = sizeof(uint32_t);
            uint32_t secondSize = entry.second.size() + 1;
            lookupTableSize += (firstSize + secondSize);

            writer.write(reinterpret_cast<const char*>(&entry.first), firstSize);
            writer.write(entry.second.c_str(), secondSize);
        }

        writer.writeUInt32(lookupTableSize);
    } else {
        writeLookupTableToFile(outputFile + ".data");
    }

    if (writer.size() > MAX_SINGLE_FILE_SIZE) {
        LOG("File data exceeded 1GB.", LogLevel::Error);
        return;
    }

    writer.close();
}

void RisePakPatch::extractDirectory(const std::string& inputFile, const std::string& outputDirectory, const bool& embedLookupTable) {
    Reader reader(inputFile);
    reverseLookupTable.clear();

    if (embedLookupTable) {
        reader.seekFromEnd(-sizeof(uint32_t));
        size_t lookupTableSize = reader.readUInt32();
        reader.seekFromEnd(-lookupTableSize - sizeof(uint32_t));
        LOG(std::to_string(reader.position()), LogLevel::Warning);

        while (reader.position() < reader.size() - sizeof(uint32_t)) {
            uint32_t hash = reader.readUInt32();

            std::string fileName;
            char        c;
            while ((c = reader.readChar()) != '\0') {
                fileName.push_back(c);
            }

            reverseLookupTable[hash] = fileName;
        }
    } else {
        readLookupTableFromFile(inputFile + ".data");
    }

    reader.seek(0);

    if (!reader.isValid()) {
        LOG("Error opening input file: " + inputFile, LogLevel::Error);
        return;
    }

    uint32_t unk0 = reader.readUInt32();
    uint32_t unk1 = reader.readUInt32();
    uint32_t unk2 = reader.readUInt32();
    uint32_t unk3 = reader.readUInt32();

    if (unk0 != 1095454795u || unk1 != 4u) {
        LOG("Invalid file format", LogLevel::Error);
        return;
    }

    std::vector<FileEntry> fileList;
    for (uint32_t i = 0; i < unk2; ++i) {
        FileEntry fileEntry;
        fileEntry.fileNameLower = reader.readUInt32();
        fileEntry.fileNameUpper = reader.readUInt32();
        fileEntry.offset        = reader.readUInt64();
        fileEntry.uncompSize    = reader.readUInt64();
        reader.skip(8);
        reader.skip(8);
        reader.skip(4);
        reader.skip(4);

        fileList.push_back(fileEntry);
    }

    for (const FileEntry& fileEntry : fileList) {
        std::string filePath = outputDirectory + "/" + reverseLookup(fileEntry.fileNameLower);
        std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
        LOG(filePath, LogLevel::Info);

        std::vector<char> fileData(fileEntry.uncompSize);
        reader.seek(fileEntry.offset);
        reader.read(fileData.data(), fileData.size());
        if (fileData.size() > MAX_SINGLE_FILE_SIZE) {
            LOG("File data exceeded 1GB.", LogLevel::Error);
            break;
        }

        Utils::writeAllBytes(filePath, fileData);
    }
}

void RisePakPatch::writeLookupTableToFile(const std::string& lookupFile) {
    std::ofstream file(lookupFile, std::ios::binary);

    if (!file.is_open()) {
        LOG("Could not create lookup table.", LogLevel::Error);
        return;
    }

    for (const auto& entry : reverseLookupTable) {
        file.write(reinterpret_cast<const char*>(&entry.first), sizeof(uint32_t));
        file.write(entry.second.c_str(), entry.second.size() + 1);
    }

    file.close();
}

void RisePakPatch::readLookupTableFromFile(const std::string& lookupFile) {
    std::ifstream file(lookupFile, std::ios::binary);

    if (!file.is_open()) {
        LOG("Could not read lookup table.", LogLevel::Error);
        return;
    }

    while (!file.eof()) {
        uint32_t hash;
        file.read(reinterpret_cast<char*>(&hash), sizeof(uint32_t));

        if (!file.eof()) {
            std::string fileName;
            std::getline(file, fileName, '\0');

            reverseLookupTable[hash] = fileName;
        }
    }

    file.close();
}