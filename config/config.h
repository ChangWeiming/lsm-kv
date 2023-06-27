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
