#ifndef _ACOROSS_ASIO_IO_SERVICE_H_
#define _ACOROSS_ASIO_IO_SERVICE_H_

#pragma once

#include <array>
#include <iostream>
#include <functional>
#include <utility>

#include <acoross\util\Util.h>

namespace acoross{
namespace asio{

class io_service
{};

//enum class err_code
//{
//	no_error,
//	error
//};
//
//typedef std::function<void(err_code err, DWORD dwTransferred)> io_callback_t;
//
//struct io_message_base : public acoross::winapi::OVERLAPPED
//{
//	io_message_base()
//	{
//		this->hEvent = 0;
//		this->Internal = 0;
//		this->InternalHigh = 0;
//		this->Offset = 0;
//		this->OffsetHigh = 0;
//		this->Pointer = 0;
//	}
//};
//
//struct io_message : public io_message_base
//{
//	io_message(io_callback_t func)
//		: callback(std::move(func)), io_message_base()
//	{	
//	}
//
//	io_callback_t callback;
//};
//
//static DWORD __stdcall IOThread(LPVOID arg)
//{
//	/*if (arg == nullptr)
//		return -1;
//
//	auto* pData = (CIOThreadControlBlock*)arg;
//	if (pData->threadFunc)
//		(pData->threadFunc)(pData);*/
//
//	const HANDLE hIOCP = (*(HANDLE*)arg);
//
//	while (true)
//	{
//		DWORD dwTransferred = 0;
//		void* compKey = nullptr;
//		acoross::winapi::OVERLAPPED* pOverlapped = nullptr;
//
//		if (GetQueuedCompletionStatus(hIOCP, &dwTransferred, (PULONG_PTR)&compKey, &pOverlapped, INFINITE))
//		{
//			std::cout << "got char!" << std::endl;
//			if (pOverlapped)
//			{
//				((io_message*)pOverlapped)->callback(err_code::no_error, dwTransferred);
//				delete pOverlapped;
//			}
//		}
//		else
//		{
//			if (pOverlapped)
//			{
//				((io_message*)pOverlapped)->callback(err_code::error, dwTransferred);
//				delete pOverlapped;
//			}
//
//			int err = GetLastError();
//			std::cerr << "error: " << err << std::endl;
//		}
//	}
//
//	return 0;
//}
//
//struct io_device_base
//{
//	virtual HANDLE GetHandle() const = 0;
//};
//
//class io_service
//{
//public:
//	NO_COPY(io_service);
//
//	io_service()
//	{
//		hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
//	}
//
//	~io_service()
//	{
//		for (auto& hThread : hThreadarr)
//		{
//			// TODO: clear threads
//		}
//	}
//
//public:
//	// windows dependent code
//	bool Register(HANDLE hDevice, const ULONG_PTR completion_key)
//	{
//		if (hIOCP == NULL)
//			return false;
//
//		if (hDevice == INVALID_HANDLE_VALUE)
//			return false;
//		
//		HANDLE ret = CreateIoCompletionPort(hDevice, hIOCP, completion_key, 0);
//		if (ret == hIOCP)
//		{
//			return true;
//		}
//
//		//int err = GetLastError();
//		return false;
//	}
//
//	err_code run()
//	{
//		if (hIOCP == NULL)
//			return err_code::error;
//
//		// run event processing loop
//		int id = 0;
//		for (auto& hThread : hThreadarr)
//		{
//			hThread = CreateThread(NULL, 0, IOThread, &hIOCP, CREATE_SUSPENDED, (LPDWORD)&id);
//			if (hThread == NULL)
//			{
//				return err_code::error;
//			}
//
//			++id;
//		}
//
//		for (auto& hThread : hThreadarr)
//		{
//			if (-1 == ResumeThread(hThread))
//			{
//				//throw exception
//				return err_code::error;
//			}
//		}
//
//		return err_code::no_error;
//	}
//
//private:
//	HANDLE hIOCP{ NULL };
//	std::array<HANDLE, 8> hThreadarr{ NULL, };
//};

}}

#endif //_ACOROSS_ASIO_IO_SERVICE_H_