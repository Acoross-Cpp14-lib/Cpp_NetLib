#ifndef _ACOROSS_ASIO_DETAIL_WIN_IOCP_IO_SERVICE_HPP_
#define _ACOROSS_ASIO_DETAIL_WIN_IOCP_IO_SERVICE_HPP_

#include <iostream>
#include <Windows.h>

#include <acoross\system.hpp>

namespace acoross{
namespace asio{
namespace detail{

/*
win_iocp_io_service
	: iocp �� �̿���, io completion �� ���� callback ó���� �Ѵ�.
	�����ϴ� ����:
		* iocp �� regist
		* io completion ó�� - do_one ȣ�� �� �Ѱ� ó��
		* io_service shutdown �� ó��

do_one() �Լ���, iocp �Ϸ������� ó���Ѵ�.
	overlapped �� �������, overlapped �� ó���Ѵ�.
	ó������ ���� job �� ������ stop chain �� ���� stop ó���Ѵ�.
		( ��� thread �� stop signal ���� )

shutdown()
	: IO ��û�� ��� user_defined handler �� ã�� destroy �Ѵ�.
		( win_iocp_operation::destroy() )
	 ? socket IO �� ��¥ IO �� ��� �ǳ�???
	 ? Timeout �� INFINITE �� ���� ������?
		-> ��� IO �� ó���� �� ���� ���
*/
class win_iocp_io_service
{
public:
	// Creator
	win_iocp_io_service();

	// Destroy all user-defined handler objects owned by the service.
	void shutdown_service();

	// Register a handle with the IO completion port.
	acoross::system::err_code register_handle(
		HANDLE handle, acoross::system::err_code& ec);
	
	// Stop the event processing loop.
	void stop();

	// Notify that some work has started.
	void work_started()
	{
		::InterlockedIncrement(&outstanding_work_);
	}

	// Notify that some work has finished.
	void work_finished()
	{
		if (::InterlockedDecrement(&outstanding_work_) == 0)
			stop();
	}

	// Dequeues at most one operation from the I/O completion port, and then
	// executes it. Returns the number of operations that were dequeued (i.e.
	// either 0 or 1).
	size_t do_one(bool block, acoross::system::err_code& ec);

private:
	// Helper class to call work_finished() on block exit.
	struct work_finished_on_block_exit
	{
		~work_finished_on_block_exit()
		{
			io_service_->work_finished();
		}

		win_iocp_io_service* io_service_;
	};

	// Helper class for managing a HANDLE.
	struct auto_handle
	{
		HANDLE handle;
		auto_handle() : handle(0) {}
		~auto_handle() { if (handle) ::CloseHandle(handle); }
	};

	// The IO completion port used for queueing operations.
	auto_handle iocp_;

	///////////////////////////////////////////////////////////////////////////////
	// The count of unfinished work.
	long outstanding_work_;

	// Flag to indicate whether the event loop has been stopped.
	mutable long stopped_;

	// Flag to indicate whether there is an in-flight stop event. Every event
	// posted using PostQueuedCompletionStatus consumes non-paged pool, so to
	// avoid exhausting this resouce we limit the number of outstanding events.
	long stop_event_posted_;
	///////////////////////////////////////////////////////////////////////////////

	// Flag to indicate whether the service has been shut down.
	long shutdown_;

	// Timeout to use with GetQueuedCompletionStatus.
	const DWORD gqcs_timeout_{ 0 };
};

}//namespace detail
}//namespace asio
}//namespace acoross

#endif //_ACOROSS_ASIO_DETAIL_WIN_IOCP_IO_SERVICE_HPP_