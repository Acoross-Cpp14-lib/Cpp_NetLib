#ifndef _ACOROSS_ASIO_TCP_SOCKET_H_
#define _ACOROSS_ASIO_TCP_SOCKET_H_

#pragma once

#include <utility>

#include "winapi.h"
#include "io_service.hpp"

namespace acoross {
namespace asio {
namespace tcp{

	class socket
	{
	public:
		socket(acoross::asio::io_service& ios)
			: _io_service(ios), _winsock(INVALID_SOCKET)
		{
			memset(&_addr, 0, sizeof(_addr));
		}

		socket(acoross::asio::io_service& ios,
			acoross::winapi::SOCKET winsock,
			acoross::winapi::sockaddr_in addr
			)
			: _io_service(ios), _winsock(winsock), _addr(addr)
		{
		}

		~socket()
		{
			closesocket(_winsock);
		}

		socket& operator=(socket&& rhs)
		{
			_io_service = rhs._io_service;
			_winsock = rhs._winsock;
			_addr = rhs._addr;

			rhs._winsock = INVALID_SOCKET;
			rhs._addr = sockaddr_in();

			return (*this);
		}

	public:
		template<size_t N>
		err_code Recv(char(&buffer)[N])
		{
			return Recv(buffer, N);
		}

		err_code Recv(char* buffer, size_t N)
		{
			WSABUF wsabuf;
			wsabuf.buf = buffer;
			wsabuf.len = N;

			DWORD dwNumOfByteReceived = 0;
			DWORD flag = 0;

			if (SOCKET_ERROR == WSARecv(_winsock, &wsabuf, 1, &dwNumOfByteReceived, &flag, NULL /*lpoverlapped*/, NULL))
			{
				return err_code::error;
			}

			//TODO: error 처리 강화
			return err_code::no_error;
		}

	private:
		acoross::asio::io_service&		_io_service;
		acoross::winapi::SOCKET			_winsock;
		acoross::winapi::sockaddr_in	_addr;
	};

}}}

#endif /_ACOROSS_ASIO_TCP_SOCKET_H_