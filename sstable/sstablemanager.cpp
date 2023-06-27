#include <ctime>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

#include "sstable/sstablemanager.h"

SSTableManager* SSTableManager::GetInstance() {
    static SSTableManager* m = new SSTableManager();
    return m;
}

void SSTableManager::Memtable2SSTable(std::shared_ptr<std::map<std::string, Value>> m) {
    std::string s = "0_" + std::to_string(this->levelNextTimestamp(0)) + ".lsm";
    //data preparation
    std::ofstream outFile(Config::GetInstance()->GetDataPath() + "/" + s, std::ios::out | std::ios::binary);
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
    for (auto it = m->begin();it != m->end(); it++, i++) {
        long long size = it->first.length();
        outFile.write(reinterpret_cast<char *> (&size), sizeof(size));
        outFile.write(reinterpret_cast<char *> (&dataStart[i]), sizeof(dataStart[i]));
        outFile.write(it->first.c_str(), it->first.length());
        indexLen += it->first.length() + sizeof(size) + sizeof(dataStart[i]); // len + 8 + 8
    }
    delete dataStart;

    //meta info preparation
    long long metaKeyNumbers = m->size(), metaDataLen = nowStart, metaIndexLen = indexLen;
    outFile.write(reinterpret_cast<char *>(&metaKeyNumbers), sizeof(metaKeyNumbers));
    outFile.write(reinterpret_cast<char *>(&metaDataLen), sizeof(metaDataLen));
    outFile.write(reinterpret_cast<char *>(&metaIndexLen), sizeof(metaIndexLen));

    outFile.close();
}

long long SSTableManager::levelNextTimestamp(long long l) {
    for (auto it = s.rbegin();it != s.rend(); it++) {
        if(it->first == l) {
            return it->second + 1;
        }
    }
    return 0;
}

bool SSTableManager::compactionLevel(int l) {
    std::vector< std::pair<long long, long long> > v;
    for (auto it = s.begin();it != s.end(); it++) {
        if(it->first == l) v.push_back(*it);
        if(v.size() == Config::GetInstance()->GetMaxCompactionFile()) {
            break;
        }
    }

    if(v.size() < Config::GetInstance()->GetMaxCompactionFile()) {
        return false;
    }


    std::shared_ptr<SSTable> res = std::make_shared<SSTable>(l+1, levelNextTimestamp(l+1));
    for (auto it = v.begin(); it != v.end(); it++) {
        auto c = cache.find(*it);
        std::shared_ptr<SSTable> tmp;
        if(c == cache.end()) {
            tmp = std::make_shared<SSTable>(it->first, it->second);
            tmp->LoadData();
        } else {
            tmp = c->second;
        }
        res->MergeTable(tmp);
    }
}


void SSTableManager::compaction() {

}