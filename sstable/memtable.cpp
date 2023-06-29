#include <string>
#include <cassert>
#include <iostream>

#include "sstable/memtable.h"
#include "sstable/sstablemanager.h"
#include "config/config.h"

MemTable::MemTable() {
    this->activeTable = std::make_shared<std::map<std::string, Value>>();
}

bool MemTable::Insert(const std::string &k, const Value &v) {

#ifdef DEBUGINFO
    std::cout << "Insert Value:" << v.Get() << ' ' << v.GetIsDeleted() << std::endl;
#endif


    if (this->activeTable->size() == Config::GetInstance()->GetMaxMemtable()) {
        toImmutableTable();
    }
    assert(this->activeTable != nullptr);
    this->activeTable->insert(std::pair<std::string, Value>(k, v));
    return true;
}

bool MemTable::Delete(const std::string &k) {
    return this->Insert(k, Value("", 1));
}

Value MemTable::Get(const std::string &k, bool &isFind) {
    auto it = this->activeTable->find(k);
    if (it == this->activeTable->end()) {
        isFind = false;
        return Value();
    } else {
        isFind = true;
        return it->second;
    }
}

bool MemTable::Update(const std::string &k, const Value &v) {
    auto it = this->activeTable->find(k);
    if (it == this->activeTable->end()) {
        return this->Insert(k, v);
    } else {
        (*(this->activeTable))[k] = v;
    }
    return true;
}

void MemTable::toImmutableTable() {

#ifdef DEBUGINFO

    for(auto it = this->activeTable->begin(); it != this->activeTable->end(); it++) {
        std::cout << "activeTable:" << it->first << ' ' << it->second.GetIsDeleted() << std::endl;
    }

#endif

    SSTableManager::GetInstance()->Memtable2SSTable(this->activeTable);
    activeTable.reset();
    this->activeTable = std::make_shared<std::map<std::string, Value>>();
}