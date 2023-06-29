
#include "tablehelper.hpp"

std::string GenerateTableName(long long level = 0, long long t = 0) {
    return std::string(std::to_string(level) + "_" + std::to_string(t) + ".lsm");
}