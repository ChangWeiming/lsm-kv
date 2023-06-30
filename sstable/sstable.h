#ifndef SSTABLE_H
#define SSTABLE_H
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
    SSTable(long long level, long long timestamp);
    void LoadMetaInfo();
    void LoadIndexInfo();
    void LoadMetaAndIndex();
    void LoadData();
    void SaveFromMap(std::shared_ptr<std::map<std::string, Value>> m);
    void MergeTable(std::shared_ptr<SSTable> s);
    void ReleaseKV();
    void SaveMergedTable();
    Value Get(const std::string &k, bool &isFind);
    std::shared_ptr< std::map<std::string, Value> > GetKV();
private:
    std::shared_ptr< std::map<std::string, Value> > kv;
    std::string filePath;
    MetaInfo meta;
    IndexInfo index;
    bool isLoadIndex, isLoadMeta, isLoadData;
    long long level, timestamp;
};

#endif