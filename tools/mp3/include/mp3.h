#pragma once

extern "C" {

#ifdef EQD_MP_EXPORTS
#undef EQD_MP_EXPORTS
#define EQD_MP_EXPORTS __declspec(dllexport)
#else
#define EQD_MP_EXPORTS __declspec(dllimport)
#endif

}