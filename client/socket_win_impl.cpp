#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "socket.h"


uint32_t sock::WSAdata::instance_count = 0;


sock::Socket sock::Socket::server(uint16_t port, uint32_t backlog)
{
	Socket self;

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(self.fd, (LPSOCKADDR)& sin, sizeof(sin)) == SOCKET_ERROR)
	{
		throw std::runtime_error("Failed to bind!");
	}

	//��ʼ����
	if (listen(self.fd, backlog) == SOCKET_ERROR)
	{
		throw std::runtime_error("Listen error !");
	}
	self.set_addr(sin);
	return self;
}

sock::Socket sock::Socket::client(const char* ip, uint16_t port) noexcept(false)
{
	Socket self;

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip);

	if (connect(self.fd, (struct sockaddr*) & serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		throw std::runtime_error("Failed to connect!");
	}
	self.set_addr(serAddr);
	return self;
}

sock::Socket sock::Socket::invalid() noexcept(true)
{
	return Socket(INVALID_SOCKET);
}

void sock::Socket::set_addr(sockaddr_in& addr)
{
	ip = inet_ntoa(addr.sin_addr);
	if (wws::big_endian())
		port = addr.sin_port;
	else
		port = wws::reverse_byte(addr.sin_port);
}