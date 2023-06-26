#include <string>

class Config {
public:
    static Config* GetInstance();
    int GetMaxMemtable();
    std::string GetDataPath();
private:
    Config();
    int maxMemtable;
    std::string dataPath;
};
