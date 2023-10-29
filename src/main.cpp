#include "lib/logger.h"
#include "lib/risepakpatch.h"

int main(int argc, char* argv[]) {
    Logger::Instance().setLogLevel(LogLevel::Error);
    Logger::Instance().log("Started (OK)", LogLevel::OK);

    if (argc != 3) {
        Logger::Instance().log("Usage: <arg1> <arg2>", LogLevel::Error);
        return 1;
    }

    const char* arg1 = argv[1];
    const char* arg2 = argv[2];

    RisePakPatch::processDirectory(arg1, arg2);

    return 0;
}