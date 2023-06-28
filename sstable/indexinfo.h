#include <vector>
#include <string>

class IndexInfo {
public:
    void AddIndexPair(std::string k, long long p);
    void AddIndexPair(char* k, long long p);
    bool IterateAllKeys(std::pair<std::string, long long> &p);
    void ResetIterate();
    void Clear();
    std::pair<std::string, long long> GetByIndex(size_t i);
    std::pair<std::string, long long> Get(const std::string &k, bool &isFind);
private:
    std::vector< std::pair<std::string, long long> > idx;
    std::vector< std::pair<std::string, long long> >::iterator it;
};
