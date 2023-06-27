#include <iostream>
#include <cstring>
#include <fstream>

#include "sstable/sstable.h"
#include "utils/tablehelper.hpp"

SSTable::SSTable(long long _level = 0, long long _timestamp = 0) {
    level = _level;
    timestamp = _timestamp;
    filePath = Config::GetInstance()->GetDataPath() + GenerateTableName(level, timestamp);
}

void SSTable::FromMemTable(std::shared_ptr<MemTable> m) {

}

void SSTable::LoadMetaInfo() {
    if(isLoadMeta) return;
    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if(inFile.is_open() == false) {
        std::cerr << "Open file " << filePath << " error!\n";
        exit(-1);
    }

    inFile.seekg(-sizeof(long long) * 3, std::ios::end);

    char* buffer = new char[sizeof(long long)];
    inFile.read(buffer, sizeof(long long));
    std::memcpy(&meta.keyNumbers, buffer, sizeof(long long));

    inFile.read(buffer, sizeof(long long));
    std::memcpy(&meta.dataLen, buffer, sizeof(long long));

    inFile.read(buffer, sizeof(long long));
    std::memcpy(&meta.indexLen, buffer, sizeof(long long));

    delete buffer;

#ifdef DEBUGINFO
    std::cout << "read meta info of file: " << filePath << " and meta info is keyNumbers:" << meta.keyNumbers << " dataLen:" << meta.dataLen << " indexLen:" << meta.indexLen << std::endl;
#endif
    isLoadMeta = true;
}

void SSTable::LoadIndexInfo() {
    if(isLoadIndex) return;
    LoadMetaInfo();
    
    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if(inFile.is_open() == false) {
        std::cerr << "Open file " << filePath << " error!\n";
        exit(-1);
    }

    inFile.seekg(-(sizeof(long long) * 3 + meta.indexLen), std::ios::end);

    char* buffer = new char[meta.indexLen - meta.keyNumbers * sizeof(long long) * 2];
    long long kLen;
    inFile.read(buffer, sizeof(long long));
    std::memcpy(&kLen, buffer, sizeof(long long));

    long long startPos;
    inFile.read(buffer, sizeof(long long));
    std::memcpy(&startPos, buffer, sizeof(long long));

    inFile.read(buffer, kLen);
    index.AddIndexPair(buffer, startPos);

    delete buffer;
#ifdef DEBUGINFO
    std::cout << "read meta info of file: " << filePath << " and meta info is keyNumbers:" << meta.keyNumbers << " dataLen:" << meta.dataLen << " indexLen:" << meta.indexLen << std::endl;
#endif
    isLoadIndex = true;
}

void SSTable::LoadMetaAndIndex() {
    LoadIndexInfo();
    LoadMetaInfo();
}

void SSTable::LoadData() {
    if (isLoadData) return;
    LoadMetaAndIndex();

    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if(inFile.is_open() == false) {
        std::cerr << "Open file " << filePath << " error!\n";
        exit(-1);
    }

    char* buffer = new char[this->meta.dataLen - meta.keyNumbers * sizeof(long long)];
    long long dataLen;
    for(int i = 0;i < meta.keyNumbers; i++) {
        inFile.read(buffer, sizeof(long long));
        std::memcpy(&dataLen, buffer, sizeof(long long));


        inFile.read(buffer, dataLen);
        std::memcpy(&dataLen, buffer, dataLen);

        int isDeleted;
        inFile.read(buffer, dataLen);
        std::memcpy(&isDeleted, buffer, sizeof(isDeleted));

        (*kv)[index.GetByIndex(i).first] = Value(buffer, isDeleted);
    }
    isLoadData = true;
    delete buffer;
}

std::shared_ptr<std::map<std::string, Value>> SSTable::GetKV() {
    return this->kv;
}

void SSTable::MergeTable(std::shared_ptr<SSTable> s) {
    auto m = s->GetKV();
    for(auto it = m->begin();it != m->end(); it++) {
        (*kv)[it->first] = it->second;
    }
}