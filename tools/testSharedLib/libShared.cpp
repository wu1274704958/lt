#include "shared.h"
#include "static.h"

void printValue()
{
    std::cout << "Calling from shared library" << std::endl;
    base::globalA->printValue();
    std::cout << "addr = " << base::globalA.get() << std::endl;

    std::cout << "Calling from static library" << std::endl;
    func::printValue();


    std::cout << "Change value to 100 by shared" << std::endl;
    base::globalA->setValue(100);

   std::cout << "Calling from shared library" << std::endl;
    base::globalA->printValue();
    std::cout << "addr = " << base::globalA.get() << std::endl;

    std::cout << "Calling from static library" << std::endl;
    func::printValue();
}