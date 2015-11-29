#ifndef _ACOROSS_ASIO_DETAIL_WIN_IOCP_SOCKET_SERVICE_H_
#define _ACOROSS_ASIO_DETAIL_WIN_IOCP_SOCKET_SERVICE_H_

#include <mutex>
#include <WinSock2.h>
#include <Windows.h>

#include <acoross\asio\io_service.hpp>
#include <acoross\asio\detail\win_iocp_operation.hpp>

namespace acoross{
namespace asio{
namespace detail{

class operation;
class win_iocp_io_service;

class null_buffers
{

};

class win_iocp_socket_service
{
public:
	// The implementation type of the socket.
	struct socket_impl
	{
		SOCKET hSocket_;
		socket_impl* next_;
		socket_impl* prev_;
	};

	// Constructor.
	inline win_iocp_socket_service(
		acoross::asio::io_service& io_service);

	// Destroy all user-defined handler objects owned by the service.
	inline void shutdown_service();

	// Construct a new socket implementation.
	inline void construct(socket_impl& impl);

	// Destroy a socket implementation.
	inline void destroy(socket_impl& impl);

	// Determine wheter the socket is open.
	bool is_open(const socket_impl& impl) const
	{
		return impl.hSocket_ != INVALID_SOCKET;
	}

	// Destroy a socket implementation.
	inline acoross::system::err_code close(socket_impl& impl, acoross::system::err_code& ec);

	// Cancel all operation associated with the socket.
	inline acoross::system::err_code cancel(socket_impl& impl, acoross::system::err_code& ec);

	// Place the socket into the state where it will listen for new connections.
	acoross::system::err_code listen(socket_impl& impl, acoross::system::err_code& ec)
	{

	}

	// Disable sends or receives on the socket.
	acoross::system::err_code shutdown(socket_impl& impl, acoross::system::err_code& ec)
	{

	}

	// Start an asynchronous send. The data being sent must be valid for the
	// lifetime of the asynchronous operation.
	template <class ConstBufferSequence, class Handler>
	size_t async_send(socket_impl& impl, const ConstBufferSequence& buffers, Handler& handler)
	{

	}

	// Start an asynchronous wait until data can be sent without blocking.
	template <class Handler>
	void async_send(socket_impl& impl, const null_buffers&, Handler& handler);

	// Start an asynchronous receive. The buffer for the data being received
	// must be valid for the lifetime of the asynchronous operation.
	template <typename MutableBufferSequence, typename Handler>
	void async_receive(socket_impl& impl,
		const MutableBufferSequence& buffers, Handler& handler);

	// Wait until data can be received without blocking.
	template <typename Handler>
	void async_receive(socket_impl& impl, const null_buffers&, Handler& handler);

protected:
	// Open a new socket implementation.
	inline acoross::system::err_code do_open(socket_impl& impl, acoross::system::err_code& ec);

	// Helper function to start an asynchronous send operation.
	void start_send_op(socket_impl& impl, WSABUF* buffers, std::size_t buffer_count, bool noop, operation* op);

	// Helper function to start an asynchronous receive operation.
	void start_receive_op(socket_impl& impl, WSABUF* buffers, std::size_t buffer_count, bool noop, operation* op);

	// Helper function to close a socket when the associated object is being
	// destroyed.
	void close_for_destruction(socket_impl& impl);

	// The IOCP service used for running asynchronous operations and dispatching
	// handlers.
	win_iocp_io_service& iocp_service_;

	// Mutex to protect access to the linked list of implementations. 
	std::mutex mutex_;

	// The head of a linked list of all implementations.
	socket_impl* impl_list_;
};

} // namespace detail
} // namespace asio
} // namespace acoross

#endif //_ACOROSS_ASIO_DETAIL_WIN_IOCP_SOCKET_SERVICE_H_