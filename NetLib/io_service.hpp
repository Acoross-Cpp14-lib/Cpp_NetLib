#ifndef _ACOROSS_ASIO_IO_SERVICE_H_
#define _ACOROSS_ASIO_IO_SERVICE_H_

#pragma once

#include <array>
#include <iostream>
#include "winapi.h"

namespace acoross{
namespace asio{

enum class err_code
{
	no_error,
	error
};

static DWORD __stdcall IOThread(LPVOID arg)
{
	/*if (arg == nullptr)
		return -1;

	auto* pData = (CIOThreadControlBlock*)arg;
	if (pData->threadFunc)
		(pData->threadFunc)(pData);*/

	const HANDLE hIOCP = (*(HANDLE*)arg);

	while (true)
	{
		DWORD dwTransferred = 0;
		void* compKey = nullptr;
		acoross::winapi::OVERLAPPED* pOverlapped = nullptr;

		if (GetQueuedCompletionStatus(hIOCP, &dwTransferred, (PULONG_PTR)&compKey, &pOverlapped, INFINITE))
		{
			std::cout << "got char!" << std::endl;
		}
		else
		{
			int err = GetLastError();
			std::cerr << "error: " << err << std::endl;
		}
	}

	return 0;
}

struct io_device_base
{
	virtual HANDLE GetHandle() const = 0;
};

class io_service
{
public:
	io_service()
	{
		hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	}

	// windows dependent code
	bool Register(io_device_base& device, ULONG_PTR completion_key)
	{
		if (device.GetHandle() == INVALID_HANDLE_VALUE)
			return false;
		if (hIOCP == NULL)
			return false;

		HANDLE ret = CreateIoCompletionPort(device.GetHandle(), hIOCP, completion_key, 0);
		if (ret == hIOCP)
		{
			return true;
		}

		//GetLastError();
		return false;
	}

	bool run()
	{
		if (hIOCP == NULL)
			return false;

		// run event processing loop
		int id = 0;
		for (auto& hThread : hThreadarr)
		{
			hThread = CreateThread(NULL, 0, IOThread, &hIOCP, CREATE_SUSPENDED, (LPDWORD)&id);
			if (hThread == NULL)
			{
				//Log.Add(L"CreateThread[%d] is failed: %d\n", id, GetLastError());
				return false;

				// throw exception
			}

			++id;
		}

		for (auto& hThread : hThreadarr)
		{
			if (-1 == ResumeThread(hThread))
			{
				//throw exception
				return false;
			}
		}

		return true;
	}

private:
	HANDLE hIOCP{ NULL };
	std::array<HANDLE, 8> hThreadarr{ NULL, };
};



}}


#endif //_ACOROSS_ASIO_IO_SERVICE_H_