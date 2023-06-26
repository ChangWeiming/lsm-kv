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
    long long t = time(NULL);
    std::string s = "0_" + std::to_string(t) + ".lsm";
    //data preparation
    std::ofstream outFile(Config::GetInstance()->GetDataPath() + "/" + s, std::ios::out | std::ios::binary);
    long long *dataStart = new long long[m->size()];
    long long nowStart = 0, i = 0;
    for (auto it = m->begin();it != m->end(); it++, i++) {
        dataStart[i] = nowStart;
        long long size = it->first.size();
        nowStart += size;
        outFile.write(reinterpret_cast<char *>(&size), sizeof(size));
        outFile.write(it->second.Get()->c_str(), it->second.Get()->size());
    }

    //key preparation
    for (auto it = m->begin();it != m->end(); it++) {

        outFile.write(, 8);

        outFile.write(it->second.Get()->c_str(), it->second.Get()->size());
    }

    //meta info preparation

}
