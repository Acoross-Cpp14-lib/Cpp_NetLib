#ifndef _ACOROSS_ASIO_TCP_SOCKET_H_
#define _ACOROSS_ASIO_TCP_SOCKET_H_

#pragma once

#include <utility>
#include <functional>

#include "winapi.h"
#include "io_service.hpp"

namespace acoross {
namespace asio {
namespace tcp{

	class socket : public io_device_base
	{
	public:
		socket(acoross::asio::io_service& ios)
			: _io_service(ios), _winsock(INVALID_SOCKET)
				, io_device_base()
		{
			memset(&_addr, 0, sizeof(_addr));
			memset(&_recvOverlapped, 0, sizeof(_recvOverlapped));
		}

		socket(acoross::asio::io_service& ios,
			acoross::winapi::SOCKET winsock,
			acoross::winapi::sockaddr_in addr
			)
			: _io_service(ios), _addr(addr), _winsock(winsock)
				, io_device_base()
		{
			memset(&_recvOverlapped, 0, sizeof(_recvOverlapped));
			_io_service.Register(*this, (ULONG_PTR)this);
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
		virtual HANDLE GetHandle() const override
		{
			return (HANDLE)_winsock;
		}

		template<size_t N>
		err_code Recv(char(&buffer)[N], DWORD& dwNumOfByteReceived)
		{
			return Recv(buffer, N, dwNumOfByteReceived);
		}

		err_code Recv(char* buffer, size_t N, DWORD& dwNumOfByteReceived)
		{
			WSABUF wsabuf;
			wsabuf.buf = buffer;
			wsabuf.len = static_cast<ULONG>(N);

			//DWORD dwNumOfByteReceived = 0;
			DWORD flag = 0;

			if (SOCKET_ERROR == WSARecv(_winsock, &wsabuf, 1, &dwNumOfByteReceived, &flag, NULL /*lpoverlapped*/, NULL))
			{
				return err_code::error;
			}

			//TODO: error 贸府 碍拳
			return err_code::no_error;
		}

		typedef std::function<void(acoross::asio::err_code, DWORD dwNumOfByteReceived)> RecvCallbackF;
		
		template<size_t N>
		err_code AsyncRecv(char(&buffer)[N], RecvCallbackF recvCallback)
		{
			WSABUF wsabuf;
			wsabuf.buf = buffer;
			wsabuf.len = N;

			DWORD flag = 0;

			if (SOCKET_ERROR == WSARecv(_winsock, &wsabuf, 1, nullptr, &flag, &_recvOverlapped, NULL) && 
				GetLastError() == WSA_IO_PENDING)
			{
				return err_code::no_error;
			}

			//TODO: error 贸府 碍拳
			return err_code::error;
		}

	private:
		acoross::asio::io_service&		_io_service;
		acoross::winapi::SOCKET			_winsock;
		acoross::winapi::sockaddr_in	_addr;

		acoross::winapi::OVERLAPPED		_recvOverlapped;
	};

}}}

#endif /_ACOROSS_ASIO_TCP_SOCKET_H_