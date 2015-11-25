#ifndef _ACOROSS_WINAPI_H_
#define _ACOROSS_WINAPI_H_

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Windows.h>

namespace acoross {
	namespace winapi {
		typedef ::SOCKET SOCKET;
		typedef ::sockaddr_in	sockaddr_in;
		typedef ::OVERLAPPED OVERLAPPED;
	}
}

#endif //_ACOROSS_WINAPI_H_