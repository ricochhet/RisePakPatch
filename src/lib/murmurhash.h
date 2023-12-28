#ifndef _PAKTOOL_MURMURHASH_INCLUDED
#define _PAKTOOL_MURMURHASH_INCLUDED

#pragma once

#include <MurmurHash3.h>

#include <fstream>

#include "utils.h"

namespace RisePakPatch {
    uint32_t murmurhash3_wstr(const std::string& u8_key, const uint32_t seed);
}

#endif  // _PAKTOOL_MURMURHASH_INCLUDED