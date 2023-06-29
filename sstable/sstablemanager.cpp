#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <stack>

#include "sstable/sstablemanager.h"
#include "utils/tablehelper.hpp"

SSTableManager* SSTableManager::GetInstance() {
    static SSTableManager* m = new SSTableManager();
    return m;
}

void SSTableManager::Memtable2SSTable(std::shared_ptr<std::map<std::string, Value>> m) {
    long long level = 0, timestamp = this->levelNextTimestamp(0);
    std::shared_ptr<SSTable> t = std::make_shared<SSTable>(level, timestamp);
    t->SaveFromMap(m);
    s.insert(std::make_pair(level, timestamp));
    compaction();
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

    long long timestamp = levelNextTimestamp(l+1);
    std::shared_ptr<SSTable> res = std::make_shared<SSTable>(l+1, timestamp);

#ifdef DEBUGINFO
    std::cout << "merging into level:" << l+1 << " timestamp:" << timestamp << std::endl;
#endif

    for (auto it = v.begin(); it != v.end(); it++) {
        std::shared_ptr<SSTable> tmp = std::make_shared<SSTable>(it->first, it->second);
        tmp->LoadData();
        res->MergeTable(tmp);
    }

    res->SaveMergedTable();

    for (auto it = v.begin(); it != v.end(); it++) {
        s.erase(*it);
#ifdef DEBUGINFO
        std::cout << "erase file:" << it->first << " " << it->second << ".lsm" << std::endl;
#endif
        std::string tmp = Config::GetInstance()->GetDataPath() + "/" + GenerateTableName(it->first, it->second);
        remove(tmp.c_str());
    }

    s.insert(std::make_pair(l+1, timestamp));

    return true;
}

void SSTableManager::compaction() {
    for(int i = 0;compactionLevel(i); i++);
}

Value SSTableManager::Get(const std::string &k, bool &isFind) {
    long long lastLevel = -1;
    std::stack<long long> st;
    //TODO: a more straightforward manner
    for(auto it = s.begin(); it != s.end(); it++) {
        if(it->first != lastLevel) {
            while(!st.empty()) {
                long long timestamp = st.top();st.pop();

#ifdef DEBUGINFO
                std::cout << "get level:" << lastLevel << " timestamp:" << timestamp << std::endl;
#endif
                std::shared_ptr<SSTable> s = std::make_shared<SSTable> (lastLevel, timestamp);
                isFind = false;
                auto r = s->Get(k, isFind);
                if (isFind) {
                    return r;
                }
            }
            st.push(it->second);
        } else {
            st.push(it->second);
        }
        lastLevel = it->first;
    }

    while(!st.empty()) {
        long long timestamp = st.top();st.pop();

#ifdef DEBUGINFO
        std::cout << "get level:" << lastLevel << " timestamp:" << timestamp << std::endl;
#endif
        std::shared_ptr<SSTable> s = std::make_shared<SSTable> (lastLevel, timestamp);
        isFind = false;
        auto r = s->Get(k, isFind);
        if (isFind) {
            return r;
        }
    }
    isFind = false;
    return Value();
}