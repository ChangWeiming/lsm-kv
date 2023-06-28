#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
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
    cache[std::make_pair(level, timestamp)] = t;
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
    res->SaveMergedTable();

    for (auto it = v.begin(); it != v.end(); it++) {
        auto c = cache.find(*it);
        if(c != cache.end()) {
            c->second.reset();
            cache.erase(c);
        }
    }

    for (auto it = v.begin(); it != v.end(); it++) {
        s.erase(*it);
#ifdef DEBUGINFO
        std::cout << "erase file:" << level << " " << timestamp << ".lsm" << std::endl;
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
    for(auto it = s.begin(); it != s.end(); it++) {
        if(it->first != lastLevel) {
            while(!st.empty()) {
                long long timestamp = st.top();st.pop();
                auto c = cache.find(std::make_pair(lastLevel, timestamp));
                std::shared_ptr<SSTable> s;
                //cache miss
                if(c == cache.end()) {
                    s = std::make_shared<SSTable> (lastLevel, timestamp);
                } else {
                    s = c->second;
                }
                bool isFind = false;
                auto r = s->Get(k, isFind);
                if (isFind) {
                    return r;
                }
            }
        } else {
            lastLevel = it->first;
            st.push(it->second);
        }
    }

    while(!st.empty()) {
        long long timestamp = st.top();st.pop();
        auto c = cache.find(std::make_pair(lastLevel, timestamp));
        std::shared_ptr<SSTable> s;
        //cache miss
        if(c == cache.end()) {
            s = std::make_shared<SSTable> (lastLevel, timestamp);
        } else {
            s = c->second;
        }
        bool isFind = false;
        auto r = s->Get(k, isFind);
        if (isFind) {
            return r;
        }
    }
    isFind = false;
    return Value();
}