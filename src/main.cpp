#include <cctype>

#include "lib/logger.h"
#include "lib/risepakpatch.h"

int main(int argc, char* argv[]) {
    RisePakPatch::Logger::Instance().setLogLevel(RisePakPatch::LogLevel::Error);
    RisePakPatch::Logger::Instance().log("Started (OK)", RisePakPatch::LogLevel::OK);

    if (argc != 4) {
        RisePakPatch::Logger::Instance().log("Usage: <arg1> <arg2>", RisePakPatch::LogLevel::Error);
        return 1;
    }

    const char* arg1 = argv[1];
    const char* arg2 = argv[2];
    const char* arg3 = argv[3];

    if (std::strcmp(arg1, "patch") == 0) {
        RisePakPatch::processDirectory(arg2, arg3);
    } else if (std::strcmp(arg1, "extract") == 0) {
        RisePakPatch::extractDirectory(arg2, arg3);
    }

    return 0;
}