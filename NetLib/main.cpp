#include <acoross\asio\io_service.hpp>
#include <acoross\asio\socket.hpp>
#include <acoross\asio\acceptor.hpp>

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
	
	acoross::asio::tcp_acceptor acceptor(7777);
	
	acoross::asio::socket soc;
	acceptor.accept(soc);

	network::Cleanup();
}