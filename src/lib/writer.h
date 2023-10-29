#ifndef _RCHT_WRITER_INCLUDED
#define _RCHT_WRITER_INCLUDED

#pragma once

#include <fstream>

namespace RisePakPatch {
    class Writer {
    public:
        Writer(std::string fileName) { m_Stream.open(fileName, std::ios::binary); }
        ~Writer() { m_Stream.close(); }
        void writeUInt32(uint32_t value) { m_Stream.write(reinterpret_cast<const char*>(&value), sizeof(value)); }
        void writeUInt64(uint64_t value) { m_Stream.write(reinterpret_cast<const char*>(&value), sizeof(value)); }
        void write(const char* data, std::streamsize count) { m_Stream.write(data, count); }
        void seek(std::streampos position) { m_Stream.seekp(position); }
        void seekFromBeginning(std::size_t position) { m_Stream.seekp(position, std::ios_base::beg); }
        int  position() { return m_Stream.tellp(); }
        void close() { m_Stream.close(); }

    private:
        std::ofstream m_Stream;
    };
}

#endif  // _RCHT_WRITER_INCLUDED