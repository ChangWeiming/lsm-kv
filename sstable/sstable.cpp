#include "sstable/sstable.h"

SSTable::SSTable(std::string _fp = "", long long _level = 0, long long _timestamp = 0) {
    filePath = _fp;
    level = _level;
    timestamp = _timestamp;
}

void SSTable::SaveDataFromMemTable(std::shared_ptr<MemTable> m,const std::string &filePath) {

}