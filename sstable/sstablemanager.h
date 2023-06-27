#include <vector>
#include <memory>
#include <map>
#include <set>

#include "sstable/sstable.h"
#include "sstable/memtable.h"

class SSTableManager {
public:
    static SSTableManager* GetInstance();
    void Memtable2SSTable(std::shared_ptr<std::map<std::string, Value>> m);
    Value Get(const std::string k, bool &isFind);
private:
    //TODO: LRU to replace map in order to save memory
    //level, timestamp -> SSTable
    std::map< std::pair<long long, long long>, std::shared_ptr<SSTable> > cache; 
    //s to represent SSTables on the disk, (level, timestamp)
    std::set< std::pair<long long, long long> > s;

    long long levelNextTimestamp(long long l);
    void compaction();
    bool compactionLevel(int l);
};