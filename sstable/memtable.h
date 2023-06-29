#ifndef MEMTABLE_H
#define MEMTABLE_H
#include <string>
#include <map>
#include <mutex>
#include <memory>
#include <vector>

#include "value/value.h"
#include "config/config.h"

class MemTable {
public:
    MemTable();
    bool Insert(const std::string &k, const Value &v);
    bool Delete(const std::string &k);
    Value Get(const std::string &k, bool &isFind);
    bool Update(const std::string &k, const Value &v);
private:
    std::shared_ptr<std::map<std::string, Value>> activeTable;
    std::mutex lock;
    void toImmutableTable();
};
#endif