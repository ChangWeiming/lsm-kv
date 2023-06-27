#include <string>

std::string GenerateTableName(long long level, long long t) {
    return std::string("0_" + std::to_string(t) + ".lsm");
}