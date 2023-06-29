#ifndef LSM_H
#define LSM_H
#include <string>
#include <memory>

#include "value/value.h"
#include "sstable/memtable.h"
#include "sstable/sstablemanager.h"

class LSM {
public:
    std::string Get(const std::string &k);
    void Insert(const std::string &k, const std::string &v);
    void Delete(const std::string &k);
    void Update(const std::string &k, const std::string &v);

    LSM();
    ~LSM();
private:
    std::shared_ptr<MemTable> m;
};
#endif