#ifndef _ACOROSS_UTIL_H_
#define _ACOROSS_UTIL_H_

#pragma once

#define NO_COPY(T) \
	T(T&) = delete;\
	T& operator=(T&) = delete;\

#endif //_ACOROSS_UTIL_H_