#ifndef RISEPAKPATCH_MURMURHASH_INCLUDED
#define RISEPAKPATCH_MURMURHASH_INCLUDED

#pragma once

#include <MurmurHash3.h>
#include "utils.h"

#include <fstream>

namespace RisePakPatch {
    uint32_t murmurhash3_wstr(const std::string& u8_key, const uint32_t seed);
}

#endif  // RISEPAKPATCH_MURMURHASH_INCLUDED