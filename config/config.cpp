#include "config/config.h"

Config* Config::GetInstance() {
    static Config* config = new Config();
    return config;
}

Config::Config() {

}

std::string Config::GetDataPath() {
    return dataPath;
}

int Config::GetMaxMemtable() {
    return maxMemtable;
}