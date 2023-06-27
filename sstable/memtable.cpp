#include <string>
#include <cassert>

#include "sstable/memtable.h"
#include "sstable/sstablemanager.h"
#include "config/config.h"

bool MemTable::Insert(const std::string &k, const Value &v) {
    if (this->activeTable->size() == GetConfigInstance()->GetMaxMemtable()) {
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
    SSTableManager::GetInstance()->MemTable2SSTable(this->activeTable);
}