#ifndef _PAKTOOL_READER_INCLUDED
#define _PAKTOOL_READER_INCLUDED

#pragma once

#include <fstream>

namespace RisePakPatch {
    class Reader {
    public:
        Reader(std::string fileName) : m_Stream(fileName, std::ios::binary) {}
        ~Reader() { m_Stream.close(); }

        bool     isValid() const { return m_Stream.is_open(); }
        uint32_t readUInt32() {
            uint32_t value;
            m_Stream.read(reinterpret_cast<char*>(&value), sizeof(value));
            return value;
        }
        uint64_t readUInt64() {
            uint64_t value;
            m_Stream.read(reinterpret_cast<char*>(&value), sizeof(value));
            return value;
        }
        void read(char* data, std::streamsize count) { m_Stream.read(data, count); }
        void seek(std::streampos position) { m_Stream.seekg(position); }
        void skip(std::streampos position) { m_Stream.seekg(position, std::ios::cur); }
        int  position() { return m_Stream.tellg(); }

    private:
        std::ifstream m_Stream;
    };
}

#endif  // _PAKTOOL_READER_INCLUDED