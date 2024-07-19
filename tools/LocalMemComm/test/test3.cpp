#include "local_mem_comm.h"
#include "local_mem_adapter.h"
#include "local_mem_proto.h"
#include "test_component.h"
#include <iostream>

using namespace eqd;

class ErrorHandler {
public:
	static void error(const std::string& err)
	{
		printf("err = %s\n", err.c_str());
	}
};

int main()
{
	LocalMemComm<local_mem_proto<
		DefChecksum>,
		DefStringSerializer, 
		DefStringSerializer, 
		win_local_mem_adapter,
		ErrorHandler> comm ("TEST",4096);

	comm.send("world");
	comm.send("qqq");

	while (true)
	{
		if (comm.tick()) {
			auto msg = comm.pop_recv();
			if(msg)
			{ 
				std::cout << "recv = [" << msg.value() << ']' << std::endl;
				if (msg == "quit")
					break;
			}
		}
	}
	while(comm.has_unsend())
	{
		comm.tick();
	}
	return 0;
}