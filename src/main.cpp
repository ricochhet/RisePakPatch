#include "lib/logger.h"
#include "lib/risepakpatch.h"

int main(int argc, char* argv[]) {
    RisePakPatch::Logger::Instance().setLogLevel(RisePakPatch::LogLevel::Error);
    RisePakPatch::Logger::Instance().log("Started (OK)", RisePakPatch::LogLevel::OK);

    if (argc != 3) {
        RisePakPatch::Logger::Instance().log("Usage: <arg1> <arg2>", RisePakPatch::LogLevel::Error);
        return 1;
    }

    const char* arg1 = argv[1];
    const char* arg2 = argv[2];

    RisePakPatch::processDirectory(arg1, arg2);

    return 0;
}