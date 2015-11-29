#ifndef _ACOROSS_ASIO_ACCEPTOR_HPP_
#define _ACOROSS_ASIO_ACCEPTOR_HPP_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>

#include <acoross\system.hpp>

namespace acoross{
namespace asio{

class dummysocket
{
public:
	dummysocket()
		: socket_(INVALID_SOCKET)
	{
	}

	~dummysocket()
	{
		if (is_open())
		{
			closesocket(socket_);
			socket_ = INVALID_SOCKET;
		}
	}

	bool is_open()
	{
		return socket_ != INVALID_SOCKET;
	}

	void assign(SOCKET new_sock)
	{
		socket_ = new_sock;
	}

private:
	SOCKET socket_;
};
typedef dummysocket socket;

class tcp_acceptor
{
public:
	NO_COPY(tcp_acceptor);

	// create new accept socket and listen
	// throw err_code as exception
	tcp_acceptor(int port)
	{
		acoross::system::err_code ec;
		impl_.socket_ = INVALID_SOCKET;
	
		impl_.socket_ = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		bind(7777, ec);
		if (ec != acoross::system::err_code::no_error)
			throw std::exception();

		listen(0, ec);
		if (ec != acoross::system::err_code::no_error)
			throw std::exception();
	}

	// move acceptor. remove original
	tcp_acceptor(tcp_acceptor&&) = delete;	

public:
	// Accept a new connectoin.
	acoross::system::err_code accept(socket& peer)
	{
		if (peer.is_open())
		{
			return acoross::system::err_code::error;
		}

		sockaddr_in cliaddr;

		SOCKET hClientSocket = ::accept(impl_.socket_, (SOCKADDR*)&cliaddr, NULL);
		if (hClientSocket == INVALID_SOCKET)
		{
			return acoross::system::err_code::error;
		}

		peer.assign(hClientSocket);
		return acoross::system::err_code::no_error;
	}

	acoross::system::err_code close()
	{
		closesocket(impl_.socket_);
	}

private:
	acoross::system::err_code bind(const int port, acoross::system::err_code& ec)
	{
		if (impl_.socket_ == INVALID_SOCKET)
		{
			return ec = acoross::system::err_code::error;
		}

		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		service.sin_port = htons(port);

		int result = ::bind(impl_.socket_, (SOCKADDR*)&service, sizeof(service));
		if (result == 0)
			ec = acoross::system::err_code::no_error;
		else
			ec = static_cast<acoross::system::err_code>(result);

		return ec;
	}

	acoross::system::err_code listen(int backlog, acoross::system::err_code& ec)
	{
		if (impl_.socket_ == INVALID_SOCKET)
		{
			return ec = acoross::system::err_code::error;
		}

		int result = ::listen(impl_.socket_, backlog);
		if (result == 0)
			ec = acoross::system::err_code::no_error;
		else
			ec = static_cast<acoross::system::err_code>(result);

		return ec;
	}

	struct acceptor_impl_type
	{
		SOCKET socket_;
	};
	acceptor_impl_type impl_;
};	// tcp_acceptor

} // namespace asio
} // namespace acoross

#endif //_ACOROSS_ASIO_ACCEPTOR_HPP_