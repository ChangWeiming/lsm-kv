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

void SSTable::SaveMergedTable() {
    this->SaveFromMap(this->kv);
}

void SSTable::ReleaseKV() {
    kv.reset();
}

//Save map on the disk, and cache meta & index info
void SSTable::SaveFromMap(std::shared_ptr<std::map<std::string, Value>> m) {
    std::ofstream outFile(filePath, std::ios::out | std::ios::binary);

    //data preparation
    long long *dataStart = new long long[m->size()];
    long long nowStart = 0, i = 0;
    for (auto it = m->begin();it != m->end(); it++, i++) {
        dataStart[i] = nowStart;
        long long size = it->second.Get()->length();
        int isDeleted = it->second.GetIsDeleted();
        nowStart += size + sizeof(size) + sizeof(isDeleted);
        outFile.write(reinterpret_cast<char *>(&size), sizeof(size));
        outFile.write(it->second.Get()->c_str(), size);
        outFile.write(reinterpret_cast<char*>(&isDeleted), sizeof(isDeleted));
    }

    //key preparation
    //index
    long long indexLen = 0;
    i  = 0;
    index.Clear();

    for (auto it = m->begin();it != m->end(); it++, i++) {
        long long size = it->first.length();
        outFile.write(reinterpret_cast<char *> (&size), sizeof(size));
        outFile.write(reinterpret_cast<char *> (&dataStart[i]), sizeof(dataStart[i]));
        outFile.write(it->first.c_str(), it->first.length());
        indexLen += it->first.length() + sizeof(size) + sizeof(dataStart[i]); // len + 8 + 8
        index.AddIndexPair(it->first.c_str(), dataStart[i]);
    }
    delete dataStart;

    //meta info preparation
    long long metaKeyNumbers = m->size(), metaDataLen = nowStart, metaIndexLen = indexLen;
    outFile.write(reinterpret_cast<char *>(&metaKeyNumbers), sizeof(metaKeyNumbers));
    outFile.write(reinterpret_cast<char *>(&metaDataLen), sizeof(metaDataLen));
    outFile.write(reinterpret_cast<char *>(&metaIndexLen), sizeof(metaIndexLen));
    meta.keyNumbers = metaKeyNumbers;
    meta.dataLen = metaDataLen;
    meta.indexLen = metaDataLen;

    outFile.close();
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
        return;
    }

    inFile.seekg(-(sizeof(long long) * 3 + meta.indexLen), std::ios::end);

    //key length
    char* buffer = new char[meta.indexLen - meta.keyNumbers * sizeof(long long) * 2];
    long long kLen;
    inFile.read(buffer, sizeof(long long));
    std::memcpy(&kLen, buffer, sizeof(long long));

    //data start position
    long long startPos;
    inFile.read(buffer, sizeof(long long));
    std::memcpy(&startPos, buffer, sizeof(long long));

    //key
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
        return;
    }

    char* buffer = new char[this->meta.dataLen - meta.keyNumbers * sizeof(long long)];
    //buffer for long long (bufferL)
    char* bufferL = new char[10];
    long long dataLen;
    for(int i = 0;i < meta.keyNumbers; i++) {
        inFile.read(bufferL, sizeof(long long));
        std::memcpy(&dataLen, bufferL, sizeof(long long));

        inFile.read(buffer, dataLen);

        int isDeleted;
        inFile.read(bufferL, sizeof(int));
        std::memcpy(&isDeleted, bufferL, sizeof(isDeleted));

        (*kv)[index.GetByIndex(i).first] = Value(buffer, isDeleted);
    }
    isLoadData = true;
    delete buffer;
    delete bufferL;
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

Value SSTable::Get(const std::string &k, bool &isFind) {
    LoadMetaAndIndex();

    isFind = false;
    auto r = index.Get(k, isFind);
    if(isFind == false) {
        return Value();
    }

    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if(inFile.is_open() == false) {
        std::cerr << "Open file " << filePath << " error!\n";
        isFind = false;
        return Value();
    }

    //buffer for long long (bufferL)
    char* bufferL = new char[10];
    long long dataLen;

    inFile.read(bufferL, sizeof(long long));
    std::memcpy(&dataLen, bufferL, sizeof(long long));


    char* buffer = new char[dataLen+3];
    inFile.read(buffer, dataLen);

    int isDeleted;
    inFile.read(bufferL, sizeof(isDeleted));
    std::memcpy(&isDeleted, bufferL, sizeof(isDeleted));

    Value v(buffer, isDeleted);

    delete buffer;
    delete bufferL;

    return v;
}