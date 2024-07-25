#pragma once
#include <wchar.h>


extern "C" {

#ifdef EQD_MP_EXPORTS
#undef EQD_MP_EXPORTS
#define EQD_MP_EXPORTS __declspec(dllexport)
#else
#define EQD_MP_EXPORTS __declspec(dllimport)
#endif


	void EQD_MP_EXPORTS MP3_init(const wchar_t* rootDir);
	void EQD_MP_EXPORTS MP3_release();

	void EQD_MP_EXPORTS MP3_getFFTData(float* data, unsigned int sizeTag);
	void EQD_MP_EXPORTS MP3_tick();
	void EQD_MP_EXPORTS MP3_setMode(int mode);
	void EQD_MP_EXPORTS MP3_pump();	
	int EQD_MP_EXPORTS MP3_setNext(const wchar_t* name);

}