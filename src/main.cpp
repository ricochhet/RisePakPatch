#include <cctype>

#include "lib/logger.h"
#include "lib/risepakpatch.h"

int main(int argc, char* argv[]) {
    LOG_LEVEL(RisePakPatch::LogLevel::Error);
    LOG("Started (OK)", RisePakPatch::LogLevel::OK);

    const char* arg1 = argv[1];
    const char* arg2 = argv[2];
    const char* arg3 = argv[3];

    if (std::strcmp(arg1, "patch") == 0) {
        RisePakPatch::processDirectory(arg2, arg3, false);
    } else if (std::strcmp(arg1, "patch2") == 0) {
        RisePakPatch::processDirectory(arg2, arg3, true);
    } else if (std::strcmp(arg1, "extract") == 0) {
        RisePakPatch::extractDirectory(arg2, arg3, false);
    } else if (std::strcmp(arg1, "extract2") == 0) {
        RisePakPatch::extractDirectory(arg2, arg3, true);
    } else if (std::strcmp(arg1, "compress") == 0) {
        RisePakPatch::compressPakAndTable(arg2);
    } else if (std::strcmp(arg1, "decompress") == 0) {
        RisePakPatch::decompressPakAndTable(arg2);
    }

    return 0;
}