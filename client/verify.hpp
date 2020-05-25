#pragma once

#include <fileop.hpp>
#include <json.hpp>
#include "socket.h"
#include <dbg.hpp>
#include "tools/convert.h"
#include <fstream>
#include "tools/thread_pool.h"
#include "json.hpp"
#include <cstdlib>
#include "constant.h"
#include <make_color.hpp>


std::optional<std::tuple<std::string, int>> config() noexcept(false);
bool send(sock::Socket& cli, std::string& data);
std::string recv(sock::Socket& cli);
sock::Socket link_server(std::string ip, int port) noexcept(false);
std::optional<std::string> downVerify(sock::Socket& client);
bool verify();


std::optional<std::tuple<std::string,int>> config() noexcept(false)
{
	char * env = std::getenv("AT_CONF_PATH");
	if(env == nullptr)
		throw std::runtime_error("AT_CONF_PATH not found!!");
	std::filesystem::path cnf(env);
	auto res = wws::read_from_file<1024>(cnf, std::ios::binary);
	if (!res)
		throw std::runtime_error("read file failed!");
	wws::Json ret;
	try {
		ret = std::move(wws::Json(res.value()));
		std::string ip = ret.get<std::string>("ip");
		int port = ret.get<int>("port");
		return std::optional( std::make_tuple(ip,port) );
	}
	catch (std::exception e)
	{
		throw e;
	}
	return {};
}

bool send(sock::Socket& cli, std::string& data)
{
	try {
		cli.send<char>(0x07);
		cli.send<int>(data.size());
		cli.send(data);
		cli.send<int>(0x0);
		cli.send<char>(0x09);
	}
	catch (std::exception e)
	{
		dbg(e.what());
		return false;
	}
	return true;
}

std::string recv(sock::Socket& cli)
{
	while (cli.recv<unsigned char>() != 0x07) {}

	int len = cli.recv<int>();
	std::string req = cli.recv(len);
	int CRC = cli.recv<int>();
	if (cli.recv<unsigned char>() != 0x09)
	{
		throw std::exception("bad pkg");
	}
	return req;
}

sock::Socket link_server(std::string ip,int port) noexcept(false)
{
	sock::Socket client = sock::Socket::invalid();
	try
	{
		client = sock::Socket::client(ip.c_str(), port);
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
		throw e;
	}
	return client;
}

std::optional<std::string> downVerify(sock::Socket& client)
{
	wws::Json req;
	req.put("reqn", (int) abc::HandlerCode::DownloadVerifyKV);

	std::string key = "cq_verify";
	wws::Json data;

	data.put("key", key);

	req.put("data", std::move(data));

	std::string sendData = req.to_string();

	int len;

	if (!send(client, sendData))
	{
		return std::nullopt;
	}

	std::string res;
	try {
		res = recv(client);
		wws::Json repo(res);
		if (repo.get<int>("ret") != 0)
			return std::nullopt;
		wws::Json da = std::move(repo.get_obj("data"));
		std::string val = da.get<std::string>("val");
		if(val.empty())
			return std::nullopt;
		return std::optional(val);
	}
	catch (std::exception e)
	{
		throw e;
	}
	return std::nullopt;
}



bool verify()
{
	try{

		auto ip = config();
		if (!ip)
		{
			std::cout << "no ip " << std::endl;
			return false;
		}
		sock::WSAdata wsa_data(2, 2);

		sock::Socket client = link_server(std::get<0>(*ip),std::get<1>(*ip));

		if (client.is_invalid())
		{
			std::cout << "link server failed" << std::endl;
			return false;
		}
		auto data = downVerify(client);
		if (!data)
		{
			std::cout << "bad data" << std::endl;
			return false;
		}
		return wws::equal(*data, PREPARE_STRING("Verification_passed"));
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
	return false;
}

#define VERIFICATION if (!verify()) { std::cerr << "Verification failed!" << std::endl; return -1;}



