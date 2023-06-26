#include <vector>
#include <string>

class IndexInfo {
public:
    void AddIndexPair(std::pair<std::string, long long> p);
    std::pair<std::string, long long> Get(const std::string &k, bool &isFind);
private:
    std::vector< std::pair<std::string, long long> > idx;
};