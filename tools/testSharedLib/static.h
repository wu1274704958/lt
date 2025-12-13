#pragma once
#include <iostream>
#include <memory>

class A{
    public:
    int* val = nullptr;
    A(int v) : val(new int(v)) {}
    ~A() {
        delete val;
    }
    void printValue() const;
    void setValue(int v);
};

namespace base{
    extern std::shared_ptr<A> globalA;    
}

namespace func{
void printValue();
}
