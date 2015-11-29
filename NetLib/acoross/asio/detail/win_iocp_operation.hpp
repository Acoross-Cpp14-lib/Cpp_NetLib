#ifndef _ACOROSS_ASIO_DETAIL_WIN_IOCP_OPERATION_HPP_
#define _ACOROSS_ASIO_DETAIL_WIN_IOCP_OPERATION_HPP_

#include <iostream>
#include <Windows.h>
#include <acoross\system.hpp>

namespace acoross{
namespace asio{
namespace detail{

class win_iocp_io_service;

class win_iocp_operation
	: public OVERLAPPED
{
public:
	void complete(win_iocp_io_service& owner,
		const acoross::system::err_code& ec,
		std::size_t bytes_transferred)
	{
		func_(&owner, this, ec, bytes_transferred);
	}

	void destroy()
	{
		func_(0, this, acoross::system::err_code::no_error, 0);
	}

protected:
	typedef void(*func_type)(
		win_iocp_io_service*, win_iocp_operation*,
		const acoross::system::err_code&, std::size_t);

	win_iocp_operation(func_type func)
		: next_(0),
		func_(func)
	{
		reset();
	}

	~win_iocp_operation()
	{}

	void reset()
	{
		// base overlapped members.
		Internal = 0;
		InternalHigh = 0;
		Offset = 0;
		OffsetHigh = 0;
		hEvent = 0;

		// added member.
		ready_ = 0;
	}

private:
	friend class op_queue_access;
	friend class win_iocp_io_service;
	win_iocp_operation* next_;
	func_type func_;
	long ready_;
};

} //namespace acoross
} //namespace asio
} //namespace detail

#endif //_ACOROSS_ASIO_DETAIL_WIN_IOCP_OPERATION_HPP_