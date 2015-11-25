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
	iosvc.run();
	
	asio::acceptor acceptor(iosvc, 7777);
	
	while (true)
	{
		asio::tcp::socket socket(iosvc);
		asio::err_code err = acceptor.Accept(socket);
		if (err == asio::err_code::no_error)
		{
			char buffer[1000]{ 0, };
			DWORD dwNumOfByteReceived = 0;

			socket.AsyncRecv(buffer, [](acoross::asio::err_code, DWORD dwTransferred)
			{
				//TODO;
			});

			/*err = socket.Recv(buffer, dwNumOfByteReceived);
			if (err == asio::err_code::no_error)
			{
				printf_s("buffer: %s\n", buffer);
			}*/
		}
	}

	Network::Cleanup();
}