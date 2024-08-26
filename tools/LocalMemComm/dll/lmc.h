#pragma once

extern "C" {

#ifdef LMC_EXPORTS
#undef LMC_EXPORTS
#define LMC_EXPORTS __declspec(dllexport)
#else
#define LMC_EXPORTS __declspec(dllimport)
#endif

	typedef void(*error_callback)(unsigned int,const char*);
	typedef void(*recv_callback)(const char*);

	unsigned int LMC_EXPORTS LMC_init(const char* mem_id,unsigned int size, error_callback callback);

	void LMC_EXPORTS LMC_send(unsigned int,const char* msg);

	int LMC_EXPORTS LMC_tick(unsigned int);

	int LMC_EXPORTS LMC_has_unsend(unsigned int);

	int LMC_EXPORTS LMC_pop_recv(unsigned int,recv_callback callback);

	void LMC_EXPORTS LMC_release(unsigned int);

	void LMC_EXPORTS LMC_reset(unsigned int);
}