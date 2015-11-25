#ifndef _ACOROSS_WINAPI_H_
#define _ACOROSS_WINAPI_H_

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include <Windows.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

namespace acoross {
	namespace winapi {
		typedef ::SOCKET SOCKET;
		typedef ::sockaddr_in	sockaddr_in;
		typedef ::OVERLAPPED OVERLAPPED;
	}
}

#endif //_ACOROSS_WINAPI_H_