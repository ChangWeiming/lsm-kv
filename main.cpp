#include <iostream>
#include <chrono>
#include <string>
#include <cassert>

#include "lsm/lsm.h"

std::string IncrBy(int i) {
    std::string s = "aaaaaaaaaa";
    int l = s.length() - 1;
    while(i && l >= 0) {
        s[l] += i % 26;
        i /= 26;
        l--;
    }
    return s;
}

int main() {
    LSM lsm;
    //for(char c = 'a'; c <= 'z'; c++) {
        //lsm.Insert(std::string(1, c), std::string(1, c));
    //}

    //for(char c = 'a'; c <= 'z'; c++) {
        //std::string v = lsm.Get(std::string(1,c));
        //std::cout << "Get " << c << " found:" << v << std::endl;
    //}
    //std::cout << "Get aaa:" << lsm.Get("aaa") << std::endl;
    //lsm.Delete("a");
    //lsm.Delete("c");
    //lsm.Delete("e");

    //for(char c = 'a'; c <= 'z'; c++) {
        //std::string v = lsm.Get(std::string(1,c));
        //std::cout << "Get " << c << " found:" << v << std::endl;
    //}

    //lsm.Update("w", "smc");
    //lsm.Update("x", "smc");
    //lsm.Update("y", "smc");
    //lsm.Update("z", "smc");
    //for(char c = 'a'; c <= 'z'; c++) {
        //std::string v = lsm.Get(std::string(1,c));
        //std::cout << "Get " << c << " found:" << v << std::endl;
    //}
    auto start = std::chrono::steady_clock::now();
    const int MAX_ITER = 1000000;
    for(int i = 0;i < MAX_ITER; i++) {
        std::string s = IncrBy(i);
        lsm.Insert(s, s);
    }
    auto end= std::chrono::steady_clock::now();
    std::cout << "Elapsed time in milliseconds: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms" << std::endl;

    auto start2= std::chrono::steady_clock::now();

    for(int i = 0;i < MAX_ITER; i++) {
        std::string s = IncrBy(i);
        assert(lsm.Get(s) == s);
    }
    auto end2= std::chrono::steady_clock::now();
    std::cout << "Elapsed time in milliseconds: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count()
        << " ms" << std::endl;
}