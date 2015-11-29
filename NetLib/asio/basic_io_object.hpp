#ifndef _ACOROSS_ASIO_BASIC_IO_OBJECT_HPP_
#define _ACOROSS_ASIO_BASIC_IO_OBJECT_HPP_

#include <acoross\asio\io_service.hpp>

namespace acoross {
namespace asio {

/// Base class for all I/O objects.
/**
* @note All I/O objects are non-copyable. 
*	And 'acoross' assumes that all I/O objects are movable.

basic_io_object
	io_service 와 연결.
	io_service 에서 IoObjectService 를 받아옴.
	IoObjectService 의 impl 을 할당받음.
*/
template <typename IoObjectService>
class basic_io_object
{
public:
	typedef IoObjectService service_t;
	typedef typename service_t::obj_impl_t obj_impl_t;

	// no copy ctor.
	basic_io_object(const basic_io_object&) = delete;
	// no copy assign.
	void operator=(const basic_io_object&) = delete;

	acoross::asio::io_service& get_io_service()
	{
		return service_->get_io_service();
	}

protected:
	explicit basic_io_object(acoross::asio::io_service& io_service)
		: service_(&acoross::asio::use_service<IoObjectService>(io_service))
	{
		service_->construct(impl);
	}

	// Move!
	basic_io_object(basic_io_object&& other)
		: service_(&other.get_service())
	{
		service_->move_construct(impl, other.impl);
	}

	~basic_io_object()
	{
		service_->destroy(impl);
	}

	basic_io_object& operator=(basic_io_object&& other)
	{
		service_->move_assign(impl, *other.service_, other.impl);
		service_ = other.service_;
	}
	
	service_t& get_service()
	{
		return *service_;
	}

	const service_t& get_service() const
	{
		return *service_;
	}

	obj_impl_t& get_impl()
	{
		return impl;
	}

	const obj_impl_t& get_impl() const
	{
		return impl;
	}

	obj_impl_t impl;

private:
	IoObjectService* service_;
};

} //namespace asio
} //namespace acoross


#endif //_ACOROSS_ASIO_BASIC_IO_OBJECT_HPP_

