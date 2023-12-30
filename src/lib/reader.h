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
        char readChar() {
            char value;
            m_Stream.read(&value, sizeof(value));
            return value;
        }
        void        seek(std::streampos position) { m_Stream.seekg(position); }
        void        seekFromBeginning(std::size_t position) { m_Stream.seekg(position, std::ios_base::beg); }
        void        seekFromEnd(std::size_t position) { m_Stream.seekg(position, std::ios_base::end); }
        void        seekFromCurrent(std::streampos position) { m_Stream.seekg(position, std::ios_base::cur); }
        int         position() { return m_Stream.tellg(); }
        std::size_t size() {
            std::streampos currentPos = m_Stream.tellg();
            m_Stream.seekg(0, std::ios::end);
            std::streampos fileSize = m_Stream.tellg();
            m_Stream.seekg(currentPos, std::ios::beg);
            return static_cast<std::size_t>(fileSize);
        }
        void close() { m_Stream.close(); }

    private:
        std::ifstream m_Stream;
    };
}

#endif  // _PAKTOOL_READER_INCLUDED