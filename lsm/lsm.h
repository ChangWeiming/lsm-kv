#include <string>
#include <memory>

#include "value/value.h"
#include "sstable/memtable.h"
#include "sstable/sstablemanager.h"

class LSM {
public:
    Value Get(const std::string &k, bool &isFind);
    void Insert(const std::string &k, const std::string &v);
    void Delete(const std::string &k);
    void Update(const std::string &k, const std::string &v);

    LSM(/* args */);
    ~LSM();
private:
    std::shared_ptr<MemTable> m;
};

