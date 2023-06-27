#include <vector>
#include <string>

#include "sstable/indexinfo.h"

void IndexInfo::AddIndexPair(std::string k, long long p) {
    idx.push_back(std::make_pair(k, p));
}

void IndexInfo::AddIndexPair(char* k, long long p) {
    std::string s = k;
    idx.push_back(std::make_pair(s, p));
}

std::pair<std::string, long long> IndexInfo::Get(const std::string &k,bool &isFind) {
    int l = 0, r = idx.size();
    //[l,r)
    while(l < r - 1) {
        int mid = (l + r) / 2;
        if (idx[mid].first < k) {
            l = mid + 1;
        } else if(idx[mid].first > k) {
            r = mid;
        } else if(idx[mid].first == k) {
            isFind = true;
            return idx[mid];
        }
    }

    if(idx[l].first == k) {
        isFind = true;
        return idx[l];
    }

    isFind = false;
    return std::pair<std::string, long long>();
}

bool IndexInfo::IterateAllKeys(std::pair<std::string, long long> &p) {
    if(it == idx.end()) {
        return false;
    }
    p = *it;
    it++;
    return true;
}

void IndexInfo::ResetIterate() {
    it = idx.begin();
}

std::pair<std::string, long long> IndexInfo::GetByIndex(size_t i) {
    return idx[i];
}