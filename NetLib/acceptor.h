#ifndef _ACOROSS_ASIO_ACCEPTOR_H_
#define _ACOROSS_ASIO_ACCEPTOR_H_

#pragma once

#include <utility>

#include "winapi.h"
#include "io_service.hpp"
#include "socket.hpp"

namespace acoross {
namespace asio {

	class acceptor : public io_device_base
	{
	public:
		acceptor(acoross::asio::io_service& ios, int port)
			: m_io_service(ios), m_port(port), m_bInitialized(false), io_device_base()
		{
			m_bInitialized = listenStart();
			m_io_service.Register(*this, (ULONG_PTR)this);
		}

		~acceptor()
		{
			close();
		}

		void AsyncAccept()
		{
			//TODO
		}

		virtual HANDLE GetHandle() const override
		{
			return (HANDLE)m_listenSock;
		}

		err_code Accept(tcp::socket& socket)
		{
			if (!m_bInitialized)
				return err_code::error;

			SOCKET hClientSocket = INVALID_SOCKET;
			sockaddr_in cliaddr;

			hClientSocket = accept(m_listenSock, (SOCKADDR*)&cliaddr, NULL);
			if (hClientSocket == INVALID_SOCKET)
			{
				return err_code::error;
			}

			socket = std::move(
				tcp::socket(m_io_service, hClientSocket, cliaddr));

			return err_code::no_error;
		}

	private:
		bool listenStart()
		{
			m_listenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (m_listenSock == INVALID_SOCKET)
			{
				return false;
			}

			sockaddr_in service;
			service.sin_family = AF_INET;
			service.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			service.sin_port = htons(m_port);
			if (bind(m_listenSock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
			{
				return false;
			}

			if (listen(m_listenSock, SOMAXCONN) == SOCKET_ERROR)
			{
				return false;
			}

			return true;
		}

		void close()
		{
			if (m_listenSock != INVALID_SOCKET)
			{
				closesocket(m_listenSock);
				m_listenSock = INVALID_SOCKET;
			}
		}

	private:
		bool m_bInitialized;

		winapi::SOCKET m_listenSock;
		int m_port;
		io_service& m_io_service;
	};

}}

#endif //_ACOROSS_ASIO_ACCEPTOR_H_