#include <iostream>
#include <string>

#include "lsm/lsm.h"

int main() {
    LSM lsm;
    for(char c = 'a'; c <= 'z'; c++) {
        lsm.Insert(std::string(1, c), std::string(1, c));
    }

    for(char c = 'a'; c <= 'z'; c++) {
        std::string v = lsm.Get(std::string(1,c));
        std::cout << "Get " << c << " found:" << v << std::endl;
    }
    std::cout << "Get aaa:" << lsm.Get("aaa") << std::endl;
    lsm.Delete("a");
    lsm.Delete("c");
    lsm.Delete("e");

    for(char c = 'a'; c <= 'z'; c++) {
        std::string v = lsm.Get(std::string(1,c));
        std::cout << "Get " << c << " found:" << v << std::endl;
    }

    lsm.Update("w", "smc");
    lsm.Update("x", "smc");
    lsm.Update("y", "smc");
    lsm.Update("z", "smc");
    for(char c = 'a'; c <= 'z'; c++) {
        std::string v = lsm.Get(std::string(1,c));
        std::cout << "Get " << c << " found:" << v << std::endl;
    }
}