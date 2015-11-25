//#include "stdafx.h"

#include "io_service.hpp"
#include "socket.hpp"
#include "acceptor.h"

namespace network
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

	network::Init();

	asio::io_service iosvc;
	iosvc.run();
	
	asio::acceptor acceptor(iosvc, 7777);
	auto pSocket = std::make_shared<asio::tcp::socket>(iosvc);	

	while (true)
	{	
		asio::err_code err = acceptor.Accept(pSocket);
		if (err == asio::err_code::no_error)
		{
			char buffer[1000]{ 0, };
			DWORD dwNumOfByteReceived = 0;

			pSocket->AsyncRecv(buffer, [](acoross::asio::err_code, DWORD dwTransferred)
			{
				printf("asyncrecv\n");
			});

			/*err = socket.Recv(buffer, dwNumOfByteReceived);
			if (err == asio::err_code::no_error)
			{
				printf_s("buffer: %s\n", buffer);
			}*/
		}
	}

	network::Cleanup();
}