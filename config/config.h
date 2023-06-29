#ifndef CONFIG_H
#define CONFIG_H

#include <string>
class Config {
public:
    static Config* GetInstance();
    int GetMaxMemtable();
    std::string GetDataPath();
    int GetMaxCompactionFile();
private:
    Config();
    int maxMemtable, maxCompactionFile;
    std::string dataPath;
};
#endif