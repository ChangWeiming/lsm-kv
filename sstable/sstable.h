#include <string>
#include <memory>

#include "sstable/memtable.h"
#include "sstable/metainfo.h"
#include "sstable/indexinfo.h"

/*
      dataLen             8*3
    -----------------------------
    |  data  |  index  |  meta  |
    -----------------------------
    for data its format is:
    |  length(8 bytes)  |  value  |  is_deleted(4 bytes)  |
    for index its format is:
    |  length(8 bytes)  |  data_start(8 bytes)  |  key  |
*/
class SSTable {
public:
    SSTable(std::string _fp = "", long long level = 0, long long timestamp = 0);
    static void SaveDataFromMemTable(std::shared_ptr<MemTable> m,const std::string &filePath);
    void LoadMetaInfo();
    void LoadIndexInfo();
    void LoadMetaAndIndex();
    void Get(const std::string &k);
private:
    std::string filePath;
    MetaInfo meta;
    IndexInfo index;
    long long level, timestamp;
};