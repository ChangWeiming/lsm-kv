#include "lsm/lsm.h"

Value LSM::Get(const std::string &k, bool &isFind) {
    isFind = false;
    auto r = m->Get(k, isFind);
    if (isFind == false) {
        r = SSTableManager::GetInstance()->Get(k, isFind);
    }
    return r;
}

void LSM::Insert(const std::string &k, const std::string &v) {
    Value val(v, 0);
    m->Insert(k, val);
}

void LSM::Update(const std::string &k, const std::string &v) {
    Value val(v, 0);
    m->Update(k, val);
}

void LSM::Delete(const std::string &k) {
    m->Delete(k);
}

LSM::LSM() {
    m = std::make_shared<MemTable>();
}

LSM::~LSM() {
    m.reset();
}