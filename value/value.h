#include <string>

class Value {
public:
    Value(const std::string _s = "", int isDeleted = 0) {
        s = _s;
        isDeleted = 0;
    }
    void Set(const std::string &_s) {
        s = _s;
    }
    void Delete() {
        isDeleted = 1;
    }
    std::string* Get() {
        return &s;
    }
    int GetIsDeleted() {
        return isDeleted;
    }
    
private:
    int isDeleted;
    std::string s;
};