#include "murmurhash.h"

uint32_t RisePakPatch::murmurhash3_wstr(const std::string& u8_key, const uint32_t seed) {
    std::wstring u16_key = Utils::utf8ToUtf16(u8_key);

    uint32_t hash;
    MurmurHash3_x86_32(u16_key.data(), u16_key.length() * 2, seed, &hash);
    return hash;
}