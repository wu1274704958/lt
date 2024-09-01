#include "lmc.h"
#include "local_mem_comm.h"
#include "local_mem_adapter.h"
#include "local_mem_proto.h"
#include "test_component.h"
#include <unordered_map>


#ifdef max
#undef max
#endif // max


#define INPUT_MSG_UNICODE 1

#if INPUT_MSG_UNICODE
#include <codecvt>
#include <corecrt_wstring.h>
#endif

using namespace eqd;



class ErrorHandler {
public:
	static void error(void* ptr,const std::string& err);
};

using CommTy = LocalMemComm<local_mem_proto<
	DefChecksum>,
	DefStringSerializer,
	DefStringSerializer,
	win_local_mem_adapter,
	ErrorHandler>;

using MapTy = std::unordered_map<uint32_t, CommTy*>;
using IteratorTy = MapTy::iterator;
MapTy comms;
int32_t Current = 0;

#if INPUT_MSG_UNICODE
namespace cvt {

	std::string unicode2utf8(const std::wstring& ws,void* cxt)
	{
		std::string ret;
		try {
			std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
			ret = wcv.to_bytes(ws);
		}
		catch (const std::exception& e) {
			ErrorHandler::error(cxt,e.what());
		}
		return ret;
	}

	std::wstring utf82unicode(const std::string& s, void* cxt)
	{
		std::wstring ret;
		try {
			std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
			ret = wcv.from_bytes(s);
		}
		catch (const std::exception& e) {
			ErrorHandler::error(cxt,e.what());
		}
		return ret;
	}
}
#endif


extern "C" {
	
	error_callback global_err_callback = nullptr;

	unsigned int LMC_init(const char* mem_id, unsigned int size, error_callback callback)
	{
		uint32_t id = Current + 1;
		if(comms.find(id) != comms.end())
		{
			return 0;
		}
		auto comm = new CommTy(mem_id, size);
		comms.insert(std::pair(id,comm));
		Current++;
		global_err_callback = callback;
		return (unsigned int)comms.size();
	}

	void LMC_send(unsigned int id,const char* msg)
	{
		IteratorTy it;
		if (it = comms.find(id);it != comms.end())
#if !INPUT_MSG_UNICODE
			it->second->send(msg);
#else
		{
			const wchar_t* data = (const wchar_t*)msg;
			std::wstring str(data);
			std::string utf8_msg = cvt::unicode2utf8(str, comms[id - 1]);
			it->second->send(utf8_msg);
		}
#endif
	}

	int LMC_tick(unsigned int id)
	{
		IteratorTy it;
		if (it = comms.find(id); it != comms.end())
			return it->second->tick() ? 1 : 0;
		return 0;
	}

	int LMC_has_unsend(unsigned int id)
	{
		IteratorTy it;
		if (it = comms.find(id); it != comms.end())
			return it->second->has_unsend() ? 1 : 0;
		return 0;
	}

	int LMC_pop_recv(unsigned int id,recv_callback callback)
	{
		IteratorTy it;
		if (it = comms.find(id); it != comms.end())
		{
			auto res = it->second->pop_recv();
			if (res)
			{
				if (callback != nullptr)
#if !INPUT_MSG_UNICODE
					callback(res.value().c_str());
#else
				{
					std::wstring uni_msg = cvt::utf82unicode(res.value(), comms[id - 1]);
					callback((const char*)uni_msg.data());
				}
#endif
				return 1;
			}
		}
		return 0;
	}

	void LMC_release(unsigned int id)
	{
		IteratorTy it;
		if (it = comms.find(id); it != comms.end())
		{
			delete it->second;
			comms.erase(it);
		}
	}

	void LMC_EXPORTS LMC_reset(unsigned int id)
	{
		IteratorTy it;
		if (it = comms.find(id); it != comms.end())
		{
			it->second->reset();
		}
	}

}

void ErrorHandler::error(void *ptr,const std::string& err)
{
	if (global_err_callback != nullptr)
	{
		for (auto it : comms)
		{
			if (it.second == ptr)
			{
				global_err_callback(it.first, err.c_str());
				return;
			}
		}
	}
}