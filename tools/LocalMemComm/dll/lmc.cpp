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
	static void error(const std::string& err);
};

LocalMemComm<local_mem_proto<
	DefChecksum>,
	DefStringSerializer,
	DefStringSerializer,
	win_local_mem_adapter,
	ErrorHandler>* comm;

#if INPUT_MSG_UNICODE
namespace cvt {

	std::string unicode2utf8(const std::wstring& ws)
	{
		std::string ret;
		try {
			std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
			ret = wcv.to_bytes(ws);
		}
		catch (const std::exception& e) {
			ErrorHandler::error(e.what());
		}
		return ret;
	}

	std::wstring utf82unicode(const std::string& s)
	{
		std::wstring ret;
		try {
			std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
			ret = wcv.from_bytes(s);
		}
		catch (const std::exception& e) {
			ErrorHandler::error(e.what());
		}
		return ret;
	}
}
#endif


extern "C" {
	
	error_callback global_err_callback = nullptr;

	void LMC_init(const char* mem_id, unsigned int size, error_callback callback)
	{
		if (comm == nullptr)
		{
			comm = new LocalMemComm<local_mem_proto<
				DefChecksum>,
				DefStringSerializer,
				DefStringSerializer,
				win_local_mem_adapter,
				ErrorHandler>(mem_id, size);
			global_err_callback = callback;
		}
	}

	void LMC_send(const char* msg)
	{
		if (comm != nullptr)
#if !INPUT_MSG_UNICODE
			comm->send(msg);
#else
		{
			const wchar_t* data = (const wchar_t*)msg;
			std::wstring str(data);
			std::string utf8_msg = cvt::unicode2utf8(str);
			comm->send(utf8_msg);
		}
#endif
	}

	int LMC_tick()
	{
		if (comm != nullptr)
			return comm->tick() ? 1 : 0;
		return 0;
	}

	int LMC_has_unsend()
	{
		if (comm != nullptr)
			return comm->has_unsend() ? 1 : 0;
		return 0;
	}

	int LMC_pop_recv(recv_callback callback)
	{
		if (comm != nullptr)
		{
			auto res = comm->pop_recv();
			if (res)
			{
				if (callback != nullptr)
#if !INPUT_MSG_UNICODE
					callback(res.value().c_str());
#else
				{
					std::wstring uni_msg = cvt::utf82unicode(res.value());
					callback((const char*)uni_msg.data());
				}
#endif
				return 1;
			}
		}
		return 0;
	}

	void LMC_release()
	{
		if (comm != nullptr)
		{
			delete comm;
			comm = nullptr;
		}
	}

}

void ErrorHandler::error(const std::string& err)
{
	if (global_err_callback != nullptr)
		global_err_callback(-1, err.c_str());
}