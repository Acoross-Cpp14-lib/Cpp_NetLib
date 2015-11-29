#include <acoross/asio/detail/win_iocp_io_service.hpp>
#include <acoross/asio/detail/win_iocp_operation.hpp>

namespace acoross{
namespace asio{
namespace detail{
// Stop the event processing loop.

win_iocp_io_service::win_iocp_io_service()
	:
	iocp_(),
	outstanding_work_(0),
	stopped_(0),
	stop_event_posted_(0),
	shutdown_(0),
	gqcs_timeout_(INFINITE)
{
	iocp_.handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (!iocp_.handle)
	{
		auto last_error = ::GetLastError();

		throw std::exception("win_iocp_io_service", static_cast<int>(last_error));
	}
}

// Destroy all user-defined handler objects owned by the service.
void win_iocp_io_service::shutdown_service()
{
	::InterlockedExchange(&shutdown_, 1);

	while (::InterlockedExchangeAdd(&outstanding_work_, 0) > 0)
	{
		DWORD bytes_transferred = 0;
		ULONG_PTR completion_key = 0;
		LPOVERLAPPED overlapped = 0;
		::GetQueuedCompletionStatus(iocp_.handle, &bytes_transferred,
			&completion_key, &overlapped, gqcs_timeout_);
		if (overlapped)
		{
			::InterlockedDecrement(&outstanding_work_);
			static_cast<win_iocp_operation*>(overlapped)->destroy();
		}
	}
}


// Register a handle with the IO completion port.

acoross::system::err_code win_iocp_io_service::register_handle(HANDLE handle, acoross::system::err_code & ec)
{
	if (::CreateIoCompletionPort(handle, iocp_.handle, 0, 0) == 0)
	{
		auto last_error = ::GetLastError();
		ec = static_cast<acoross::system::err_code>(last_error);
	}
	else
	{
		ec = acoross::system::err_code::no_error;
	}
	return ec;
}

void win_iocp_io_service::stop()
{
	if (::InterlockedExchange(&stopped_, 1) != 0)
		return;

	if (::InterlockedExchange(&stop_event_posted_, 1) != 0)
		return;

	if (false == ::PostQueuedCompletionStatus(iocp_.handle, 0, 0, 0))
	{
		DWORD last_error = ::GetLastError();
		auto ec = static_cast<acoross::system::err_code>(last_error);

		//boost::asio::detail::throw_error(ec, "pqcs");
	}
}

// message 를 한개 처리하거나,
// stop chain 을 처리하거나,
// timeout 되어 return 0;
// 아니면 error!
// block = true 일 경우 timeout 되지 않는다.
size_t win_iocp_io_service::do_one(bool block, acoross::system::err_code& ec)
{
	while (true)
	{
		// Get the next operation from the queue.
		DWORD bytes_transferred = 0;
		ULONG_PTR completion_key = 0;
		LPOVERLAPPED overlapped = 0;

		::SetLastError(0);
		BOOL ok = ::GetQueuedCompletionStatus(iocp_.handle, &bytes_transferred,
			&completion_key, &overlapped, block ? gqcs_timeout_ : 0);
		DWORD last_error = ::GetLastError();

		if (overlapped)
		{
			auto* op = static_cast<win_iocp_operation*>(overlapped);
			auto result_ec = static_cast<acoross::system::err_code>(last_error);

			//if (completion_key == completion_type::overlapped_contains_result)
			{
				result_ec = static_cast<acoross::system::err_code>(op->Offset);
				bytes_transferred = op->OffsetHigh;
			}

			if (::InterlockedCompareExchange(&op->ready_, 1, 0) == 1)
			{
				work_finished_on_block_exit onexit = { this };
				(void)onexit;

				op->complete(*this, result_ec, bytes_transferred);
				ec = acoross::system::err_code::no_error;
				return 1;
			}
		}
		else if (!ok)
		{
			if (last_error != WAIT_TIMEOUT)
			{
				ec = static_cast<acoross::system::err_code>(last_error);
				return 0;
			}

			if (block)
				continue;

			ec = acoross::system::err_code::no_error;
			return 0;
		}
		else
		{	// stop event chain.
			
			// Indicate that there is no longer an in-flight stop event.
			::InterlockedExchange(&stop_event_posted_, 0);

			// The stopped_ flag is always checked to ensure that any leftover
			// stop events from a previous run invocation are ignored.
			if (::InterlockedExchangeAdd(&stopped_, 0) != 0)
			{
				// Wake up next thread that is blocked on GetQueuedCompletionStatus.
				if (::InterlockedExchange(&stop_event_posted_, 1) == 0)
				{
					if (!::PostQueuedCompletionStatus(iocp_.handle, 0, 0, 0))
					{
						last_error = ::GetLastError();
						ec = static_cast<acoross::system::err_code>(last_error);
						return 0;
					}
				}

				ec = acoross::system::err_code::no_error;
				return 0;
			}
		}
	}
}

} //namespace detail
} //namespace asio
} //namespace acoross