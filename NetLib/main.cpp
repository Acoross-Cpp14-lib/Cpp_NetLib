//#include "stdafx.h"

#include "io_service.hpp"
#include "socket.hpp"
#include "acceptor.h"

namespace Network
{
	// initialize Winsock
	bool Init()
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		{
			WSACleanup();
			return false;
		}
		return true;
	}

	void Cleanup()
	{
		WSACleanup();
	}
}//Network

int main(int argc, char* argv[])
{
	using namespace acoross;

	Network::Init();

	asio::io_service iosvc;
	asio::acceptor acceptor(iosvc, 7777);
	
	while (true)
	{
		asio::tcp::socket socket(iosvc);
		asio::err_code err = acceptor.Accept(socket);
		if (err == asio::err_code::no_error)
		{
			char buffer[1000]{ 0, };
			err = socket.Recv(buffer);
			if (err == asio::err_code::no_error)
			{
				printf_s("buffer: %s\n", buffer);
			}
		}
	}

	iosvc.run();

	Network::Cleanup();
}