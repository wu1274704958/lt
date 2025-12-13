#include "static.h"
#include <memory>

void A::printValue() const
{
    if (val)
    {
        std::cout << "Value: " << *val << std::endl;
    }
    else
    {
        std::cout << "Value is null" << std::endl;
    }
}

void A::setValue(int v)
{
    if (val)
    {
        *val = v;
    }
    else
    {
        val = new int(v);
    }
}

namespace base {
    std::shared_ptr<A> globalA = std::make_shared<A>(99); // Initialize global pointer to nullptr
}

namespace func {
    void printValue() {
        base::globalA->printValue();
        std::cout << "addr = " << base::globalA.get() << std::endl; 
    }
}
