#ifndef _PAKTOOL_WRITER_INCLUDED
#define _PAKTOOL_WRITER_INCLUDED

#pragma once

#include <fstream>

namespace RisePakPatch {
    class Writer {
    public:
        Writer(std::string fileName) { m_Stream.open(fileName, std::ios::binary); }
        ~Writer() { m_Stream.close(); }

        bool        isValid() const { return m_Stream.is_open(); }
        void        writeUInt32(uint32_t value) { m_Stream.write(reinterpret_cast<const char*>(&value), sizeof(value)); }
        void        writeUInt64(uint64_t value) { m_Stream.write(reinterpret_cast<const char*>(&value), sizeof(value)); }
        void        write(const char* data, std::streamsize count) { m_Stream.write(data, count); }
        void        writeChar(const char* data) { m_Stream.write(data, sizeof(data)); }
        void        seek(std::streampos position) { m_Stream.seekp(position); }
        void        seekFromBeginning(std::size_t position) { m_Stream.seekp(position, std::ios_base::beg); }
        void        seekFromEnd(std::size_t position) { m_Stream.seekp(position, std::ios_base::end); }
        void        skip(std::streampos position) { m_Stream.seekp(position, std::ios_base::cur); }
        int         position() { return m_Stream.tellp(); }
        std::size_t size() {
            std::streampos currentPos = m_Stream.tellp();
            m_Stream.seekp(0, std::ios::end);
            std::streampos fileSize = m_Stream.tellp();
            m_Stream.seekp(currentPos, std::ios::beg);
            return static_cast<std::size_t>(fileSize);
        }
        void close() { m_Stream.close(); }

    private:
        std::ofstream m_Stream;
    };
}

#endif  // _PAKTOOL_WRITER_INCLUDED