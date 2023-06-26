#include <vector>
#include <memory>
#include <map>

#include "sstable/sstable.h"
#include "sstable/memtable.h"

class SSTableManager {
public:
    static SSTableManager* GetInstance();
    void Memtable2SSTable(std::shared_ptr<std::map<std::string, Value>> m);
    Value Get(const std::string k, bool &isFind);
private:
    //level, timestamp -> SSTable
    std::map< std::pair<long long, long long>, SSTable > cache;
    void compaction();
};