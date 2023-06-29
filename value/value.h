#ifndef VALUE_H
#define VALUE_H
#include <string>

class Value {
public:
    Value(const std::string _s = "", int _isDeleted = 0) {
        s = _s;
        isDeleted = _isDeleted;
    }
    void Set(const std::string &_s) {
        s = _s;
    }
    void Delete() {
        isDeleted = 1;
    }
    std::string Get() const {
        return s;
    }
    int GetIsDeleted() const {
        return isDeleted;
    }
    
private:
    int isDeleted;
    std::string s;
};

#endif