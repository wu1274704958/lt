#pragma once
#include "static.h"

extern "C" {

#ifdef EQD_MP_EXPORTS
#undef EQD_MP_EXPORTS
#define EQD_MP_EXPORTS __declspec(dllexport)
#else
#define EQD_MP_EXPORTS __declspec(dllimport)
#endif

	EQD_MP_EXPORTS void printValue();
}

template<typename T>
void printValueT(const T& value) {

	std::cout << "Calling from execute" << std::endl;
	base::globalA->printValue();
	std::cout << "Change value to 999 by execute" << std::endl;
	base::globalA->setValue(999);
	base::globalA->printValue();
	std::cout << "Value: " << value << base::globalA.get() << std::endl;
}

