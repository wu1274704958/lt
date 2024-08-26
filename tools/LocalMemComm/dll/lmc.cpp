#include "lmc.h"
#include "local_mem_comm.h"
#include "local_mem_adapter.h"
#include "local_mem_proto.h"
#include "test_component.h"

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

std::vector<CommTy*> comms;

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
		auto comm = new CommTy(mem_id, size);
		comms.push_back(comm);
		global_err_callback = callback;
		return (unsigned int)comms.size();
	}

	void LMC_send(unsigned int id,const char* msg)
	{
		if (id > 0 && id <= comms.size())
#if !INPUT_MSG_UNICODE
			comms[id - 1]->send(msg);
#else
		{
			const wchar_t* data = (const wchar_t*)msg;
			std::wstring str(data);
			std::string utf8_msg = cvt::unicode2utf8(str, comms[id - 1]);
			comms[id - 1]->send(utf8_msg);
		}
#endif
	}

	int LMC_tick(unsigned int id)
	{
		if (id > 0 && id <= comms.size())
			return comms[id - 1]->tick() ? 1 : 0;
		return 0;
	}

	int LMC_has_unsend(unsigned int id)
	{
		if (id > 0 && id <= comms.size())
			return comms[id - 1]->has_unsend() ? 1 : 0;
		return 0;
	}

	int LMC_pop_recv(unsigned int id,recv_callback callback)
	{
		if (id > 0 && id <= comms.size())
		{
			auto res = comms[id - 1]->pop_recv();
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

	void check_comms_all_release()
	{
		for (int i = 0;i < comms.size(); ++i)
		{
			if (comms[i] != nullptr)
				return;
		}
		if (!comms.empty())
			comms.clear();
	}

	void LMC_release(unsigned int id)
	{
		if (id > 0 && id <= comms.size())
		{
			delete comms[id - 1];
			comms[id - 1] = nullptr;
			check_comms_all_release();
		}
	}

	void LMC_EXPORTS LMC_reset(unsigned int id)
	{
		if (id > 0 && id <= comms.size())
		{
			comms[id - 1]->reset();
		}
	}

}

void ErrorHandler::error(void *ptr,const std::string& err)
{
	if (global_err_callback != nullptr)
	{
		for (int i = 0;i < comms.size();++i)
		{
			if (comms[i] == ptr)
			{
				global_err_callback((unsigned int)i+1, err.c_str());
				return;
			}
		}
	}
}