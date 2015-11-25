#ifndef _ACOROSS_ASIO_TCP_SOCKET_H_
#define _ACOROSS_ASIO_TCP_SOCKET_H_

#pragma once

#include <utility>
#include <functional>
#include <memory>

#include "winapi.h"
#include "io_service.hpp"

namespace acoross {
namespace asio {
namespace tcp{

	class socket : public io_device_base
	{
	public:
		enum class state : char
		{
			not_registered_iocp,
			initialized,
		};

	public:
		NO_COPY(socket);

		socket(acoross::asio::io_service& ios)
			: _io_service(ios), _winsock(INVALID_SOCKET)
				, io_device_base()
		{
			memset(&_addr, 0, sizeof(_addr));
		}

		socket(acoross::asio::io_service& ios,
			acoross::winapi::SOCKET winsock,
			acoross::winapi::sockaddr_in addr
			)
			: _io_service(ios), _addr(addr), _winsock(winsock)
				, io_device_base()
		{	
		}

		~socket()
		{
			closesocket(_winsock);
		}

		void Set(acoross::winapi::SOCKET winsock,
			acoross::winapi::sockaddr_in addr)
		{
			_winsock = winsock;
			_addr = addr;
		}

	public:
		virtual HANDLE GetHandle() const override
		{
			return (HANDLE)_winsock;
		}

		template<size_t N>
		err_code Recv(char(&buffer)[N], DWORD& dwNumOfByteReceived) const
		{
			return Recv(buffer, N, dwNumOfByteReceived);
		}

		err_code Recv(char* buffer, size_t N, DWORD& dwNumOfByteReceived) const
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
		
		template<size_t N>
		err_code AsyncRecv(char(&buffer)[N], io_callback_t recvCallback) const
		{
			if (_state == state::not_registered_iocp)
			{
				return err_code::error;
			}

			WSABUF wsabuf;
			wsabuf.buf = buffer;
			wsabuf.len = N;

			DWORD flag = 0;

			auto* pMsg = new io_message(std::move(recvCallback));
			if (SOCKET_ERROR == WSARecv(_winsock, &wsabuf, 1, nullptr, &flag, pMsg, NULL))
			{
				int wsaerr = WSAGetLastError();
				if (wsaerr == WSA_IO_PENDING)
				{
					return err_code::no_error;
				}
			}

			//TODO: error 贸府 碍拳
			return err_code::error;
		}

		void Register2IOCP() const
		{
			if (_io_service.Register((HANDLE)_winsock, (ULONG_PTR)this))
			{
				_state = state::initialized;
			}
		}

	private:
		acoross::asio::io_service&		_io_service;
		acoross::winapi::SOCKET			_winsock;
		acoross::winapi::sockaddr_in	_addr;

		acoross::winapi::OVERLAPPED		_recvOverlapped;

		mutable state _state{ state::not_registered_iocp };
	};

	typedef std::shared_ptr<socket> SocketSP;
}}}

#endif /_ACOROSS_ASIO_TCP_SOCKET_H_