#include <risepakpatch.h>
#include <writer.h>

void RisePakPatch::processDirectory(const std::string& path, const std::string& outputFile) {
    std::string directory = std::filesystem::absolute(path).string();

    if (!std::filesystem::is_directory(directory)) {
        return;
    }

    if (std::filesystem::exists(outputFile)) {
        Logger::Instance().log("Deleting existing output file...", LogLevel::Info);
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

    Logger::Instance().log("Processing " + std::to_string(sortedFiles.size()) + " files", LogLevel::Info);
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
    }

    writer.seekFromBeginning(16);
    for (const FileEntry& item : list) {
        Logger::Instance().log(item.fileName + " " + std::to_string(item.fileNameUpper) + " " + std::to_string(item.fileNameLower), LogLevel::Info);
        writer.writeUInt32(item.fileNameLower);
        writer.writeUInt32(item.fileNameUpper);
        writer.writeUInt64(item.offset);
        writer.writeUInt64(item.uncompSize);
        writer.writeUInt64(item.uncompSize);
        writer.writeUInt64(0uL);
        writer.writeUInt32(0u);
        writer.writeUInt32(0u);
    }

    writer.close();
}