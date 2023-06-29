#include "config/config.h"

Config* Config::GetInstance() {
    static Config* config = new Config();
    return config;
}

Config::Config() {
    dataPath = "./";
    maxMemtable = 3;
    maxCompactionFile = 3;
}

std::string Config::GetDataPath() {
    return dataPath;
}

int Config::GetMaxMemtable() {
    return maxMemtable;
}

int Config::GetMaxCompactionFile() {
    return maxCompactionFile;
}