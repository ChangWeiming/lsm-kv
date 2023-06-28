#include <iostream>

#include "lsm/lsm.h"

int main() {
    LSM lsm;
    lsm.Insert("123", "123");
    bool isFind = false;
    Value v = lsm.Get("123", isFind);
    if(isFind) {
        std::cout << "Get 123 found:" << v.Get() << std::endl;
    }
    v = lsm.Get("1234", isFind);
    if(!isFind) {
        std::cout << "Get 1234 not found:" << v.Get() << std::endl;
    }
}