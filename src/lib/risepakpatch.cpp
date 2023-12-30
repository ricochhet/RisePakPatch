#include "risepakpatch.h"

#include <unordered_map>

#include "reader.h"
#include "writer.h"

#define MAX_SINGLE_FILE_SIZE 1073741824
#define RISE_PAK_FOOTER 1163086162u
#define RISE_PAK_FOOTER_SIZE sizeof(uint64_t)

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
        LOG("processDirectory(): Deleting existing output file...", LogLevel::Info);
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

    LOG("processDirectory(): Processing " + std::to_string(sortedFiles.size()) + " files", LogLevel::Info);
    std::vector<FileEntry> list;
    Writer                 writer(outputFile);
    if (!writer.isValid()) {
        LOG("processDirectory(): Error writing: " + outputFile, LogLevel::Error);
        return;
    }

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

        uint32_t hash  = murmurhash3_wstr(Utils::toLower(text).c_str(), UINT_MAX);
        uint32_t hash2 = murmurhash3_wstr(Utils::toUpper(text).c_str(), UINT_MAX);
        Reader   bytes(obj);
        if (bytes.size() > MAX_SINGLE_FILE_SIZE) {
            LOG("processDirectory(): File data exceeded 1GB in file entry bytes.", LogLevel::Error);
            return;
        }
        std::vector<char> array2(bytes.size());
        bytes.read(array2.data(), array2.size());

        fileEntry2.fileName      = text;
        fileEntry2.offset        = (uint64_t)writer.position();
        fileEntry2.uncompSize    = (uint64_t)array2.size();
        fileEntry2.fileNameLower = hash;
        fileEntry2.fileNameUpper = hash2;

        list.emplace_back(fileEntry2);
        if (array2.size() > MAX_SINGLE_FILE_SIZE) {
            LOG("processDirectory(): File data exceeded 1GB in file entry buffer.", LogLevel::Error);
            return;
        }
        LOG("processDirectory(): Writing file: " + text + " of size: " + std::to_string(array2.size()), LogLevel::Info);
        writer.write(array2.data(), array2.size());

        reverseLookupTable[hash]  = text;
        reverseLookupTable[hash2] = text;
    }

    writer.seekFromBeginning(16);
    for (const FileEntry& item : list) {
        LOG("processDirectory(): Writing entry: " + item.fileName + " " + std::to_string(item.fileNameUpper) + " " + std::to_string(item.fileNameLower), LogLevel::Info);
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
    if (embedLookupTable) {
        writeLookupTable(writer);
    } else {
        writeLookupTableToFile(outputFile + ".data");
    }

    if (writer.size() > MAX_SINGLE_FILE_SIZE) {
        LOG("processDirectory(): File data exceeded 1GB in pak writer.", LogLevel::Error);
        return;
    }

    writer.close();
    LOG("processDirectory(): Done.", LogLevel::Info);
}

void RisePakPatch::extractDirectory(const std::string& inputFile, const std::string& outputDirectory, const bool& embedLookupTable) {
    Reader reader(inputFile);
    if (!reader.isValid()) {
        LOG("extractDirectory(): Error reading: " + inputFile, LogLevel::Error);
        return;
    }

    reverseLookupTable.clear();

    if (embedLookupTable) {
        LOG("extractDirectory(): Reading embedded lookup table...", LogLevel::Info);
        readLookupTable(reader);
    } else {
        LOG("extractDirectory(): Reading file lookup table...", LogLevel::Info);
        readLookupTableFromFile(inputFile + ".data");
    }

    reader.seek(0);

    uint32_t unk0 = reader.readUInt32();
    uint32_t unk1 = reader.readUInt32();
    uint32_t unk2 = reader.readUInt32();
    uint32_t unk3 = reader.readUInt32();

    if (unk0 != 1095454795u || unk1 != 4u) {
        LOG("extractDirectory(): Invalid file format", LogLevel::Error);
        return;
    }

    std::vector<FileEntry> fileList;
    for (uint32_t i = 0; i < unk2; ++i) {
        FileEntry fileEntry;
        fileEntry.fileNameLower = reader.readUInt32();
        fileEntry.fileNameUpper = reader.readUInt32();
        fileEntry.offset        = reader.readUInt64();
        fileEntry.uncompSize    = reader.readUInt64();
        reader.seekFromCurrent(8);
        reader.seekFromCurrent(8);
        reader.seekFromCurrent(4);
        reader.seekFromCurrent(4);

        fileList.push_back(fileEntry);
        LOG("extractDirectory(): Found file entry: " + std::to_string(fileEntry.fileNameLower) + " of size: " + std::to_string(fileEntry.uncompSize), LogLevel::Info);
    }

    if (reverseLookupTable.size() <= 0) {
        LOG("extractDirectory(): Empty lookup table", LogLevel::Error);
        return;
    }

    for (const FileEntry& fileEntry : fileList) {
        std::string name = reverseLookup(fileEntry.fileNameLower);
        if (name.empty()) {
            LOG("extractDirectory(): Missing lookup table entry", LogLevel::Error);
            break;
        }

        std::string filePath = outputDirectory + "/" + name;
        std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
        LOG("extractDirectory(): Writing: " + filePath, LogLevel::Info);

        std::vector<char> fileData(fileEntry.uncompSize);
        reader.seek(fileEntry.offset);
        reader.read(fileData.data(), fileData.size());
        if (fileData.size() > MAX_SINGLE_FILE_SIZE) {
            LOG("extractDirectory(): File data exceeded 1GB in entry file data.", LogLevel::Error);
            break;
        }

        Writer fileEntryWriter(filePath);
        fileEntryWriter.write(fileData.data(), fileData.size());
        fileEntryWriter.close();
    }

    reader.close();
    LOG("extractDirectory(): Done.", LogLevel::Info);
}

void RisePakPatch::compressPakAndTable(const std::string& inputFile) {
    LOG("compressPakAndTable(): Compressing...", LogLevel::Info);
    Writer pak(inputFile, true);
    if (!pak.isValid()) {
        LOG("compressPakAndTable(): Error writing: " + inputFile, LogLevel::Error);
        return;
    }

    Reader table(inputFile + ".data");
    if (!table.isValid()) {
        LOG("compressPakAndTable(): Error reading: " + inputFile + ".data", LogLevel::Error);
        return;
    }

    readLookupTable(table);
    pak.seekFromEnd(0);
    writeLookupTable(pak);

    pak.close();
    table.close();
    if (std::filesystem::exists(inputFile + ".data")) {
        LOG("compressPakAndTable(): Deleting existing table data file...", LogLevel::Info);
        std::filesystem::remove(inputFile + ".data");
    }
    LOG("compressPakAndTable(): Done.", LogLevel::Info);
}

void RisePakPatch::decompressPakAndTable(const std::string& inputFile) {
    Reader pak(inputFile);
    if (!pak.isValid()) {
        LOG("decompressPakAndTable(): Error reading: " + inputFile, LogLevel::Error);
        return;
    }

    Writer table(inputFile + ".data");
    if (!table.isValid()) {
        LOG("decompressPakAndTable(): Error writing: " + inputFile + ".data", LogLevel::Error);
        return;
    }

    readLookupTable(pak);
    LOG("decompressPakAndTable(): Writing table data...", LogLevel::Info);
    writeLookupTable(table);
    table.close();

    pak.seekFromEnd(-sizeof(RISE_PAK_FOOTER_SIZE));
    size_t lookupTableSize = pak.readUInt32();
    size_t risePakFooter   = pak.readUInt32();
    if (risePakFooter != RISE_PAK_FOOTER) {
        LOG("decompressPakAndTable(): RISE_PAK_FOOTER not found", LogLevel::Error);
        return;
    }

    pak.seek(0);
    size_t       pakSize    = pak.size();
    size_t       tableSize  = lookupTableSize + sizeof(uint64_t);
    const size_t decompSize = pakSize - tableSize;
    LOG("decompressPakAndTable(): Decompressing...", LogLevel::Info);
    LOG("decompressPakAndTable(): decompSize: " + std::to_string(decompSize), LogLevel::Info);
    if (decompSize > MAX_SINGLE_FILE_SIZE) {
        LOG("decompressPakAndTable(): File data exceeded 1GB in decompSize.", LogLevel::Error);
        return;
    }

    std::vector<char> buffer(decompSize);
    LOG("decompressPakAndTable(): bufferSize: " + std::to_string(buffer.size()), LogLevel::Info);
    if (buffer.size() > MAX_SINGLE_FILE_SIZE) {
        LOG("decompressPakAndTable(): File data exceeded 1GB in buffer.", LogLevel::Error);
        return;
    }
    LOG("decompressPakAndTable(): Writing to buffer...", LogLevel::Info);
    pak.read(buffer.data(), buffer.size());
    pak.close();

    Writer decomp(inputFile);
    if (!decomp.isValid()) {
        LOG("decompressPakAndTable(): Error writing: " + inputFile, LogLevel::Error);
        return;
    }
    LOG("decompressPakAndTable(): Finalizing decompression...", LogLevel::Info);
    decomp.write(buffer.data(), buffer.size());
    if (decomp.size() > MAX_SINGLE_FILE_SIZE) {
        LOG("decompressPakAndTable(): File data exceeded 1GB in decomp writer.", LogLevel::Error);
        return;
    }

    decomp.close();
    LOG("decompressPakAndTable(): Done.", LogLevel::Info);
}

void RisePakPatch::writeLookupTableToFile(const std::string& lookupFile) {
    Writer writer(lookupFile);
    if (!writer.isValid()) {
        LOG("writeLookupTableToFile(): Error writing: " + lookupFile, LogLevel::Error);
        return;
    }
    writeLookupTable(writer);
    writer.close();
}

void RisePakPatch::writeLookupTable(Writer& writer) {
    LOG("writeLookupTable(): Writing lookup table...", LogLevel::Info);
    uint32_t lookupTableSize = 0;
    for (const auto& entry : reverseLookupTable) {
        uint32_t firstSize  = sizeof(uint32_t);
        uint32_t secondSize = entry.second.size() + 1;
        lookupTableSize += (firstSize + secondSize);

        writer.write(reinterpret_cast<const char*>(&entry.first), firstSize);
        writer.write(entry.second.c_str(), secondSize);
    }

    writer.writeUInt32(lookupTableSize);
    writer.writeUInt32(RISE_PAK_FOOTER);
    if (writer.size() > MAX_SINGLE_FILE_SIZE) {
        LOG("writeLookupTable(): File data exceeded 1GB in lookup table writer.", LogLevel::Error);
        return;
    }
    LOG("writeLookupTable(): Done.", LogLevel::Info);
}

void RisePakPatch::readLookupTableFromFile(const std::string& lookupFile) {
    Reader reader(lookupFile);
    if (!reader.isValid()) {
        LOG("readLookupTableFromFile(): Error reading: " + lookupFile, LogLevel::Error);
        return;
    }
    readLookupTable(reader);
    reader.close();
}

void RisePakPatch::readLookupTable(Reader& reader) {
    LOG("readLookupTable(): Reading lookup table...", LogLevel::Info);
    reader.seekFromEnd(-sizeof(RISE_PAK_FOOTER_SIZE));
    size_t lookupTableSize = reader.readUInt32();
    size_t risePakFooter   = reader.readUInt32();
    if (risePakFooter != RISE_PAK_FOOTER) {
        LOG("readLookupTable(): RISE_PAK_FOOTER not found.", LogLevel::Error);
        return;
    }
    reader.seekFromEnd(-lookupTableSize - sizeof(RISE_PAK_FOOTER_SIZE));

    while (reader.position() < reader.size() - sizeof(RISE_PAK_FOOTER_SIZE)) {
        uint32_t hash = reader.readUInt32();

        std::string fileName;
        char        c;
        while ((c = reader.readChar()) != '\0') {
            fileName.push_back(c);
        }

        LOG("readLookupTable(): Found entry: " + std::to_string(hash) + ", " + fileName, LogLevel::Info);
        reverseLookupTable[hash] = fileName;
    }
    LOG("readLookupTable(): Done.", LogLevel::Info);
}