#pragma once

#include "..\socket.h"
#include "..\..\memory\pool_allocator.h"

#if defined(_WIN64)
	#define ASYNC_REQUEST_RESERVED 32
#elif defined(_WIN32)
	#define ASYNC_REQUEST_RESERVED 20
#endif

#define REQEST_POOL_SIZE 1024

namespace crlib
{

class async_connection;

/*
Abstract class that encapsulates an async winsock IOCP server.
*/
class iocp_server
{
public:
	enum ASYNC_REQUEST_TYPE
	{
		ASYNC_REQUEST_TYPE_READ,
		ASYNC_REQUEST_TYPE_WRITE,
		ASYNC_REQUEST_TYPE_ACCEPT
	};

	struct async_request
	{
		async_request();

		char reserved[ASYNC_REQUEST_RESERVED];
		ASYNC_REQUEST_TYPE type;
		async_connection* sock;
		uint32_t buffer_size;
		char* buffer;
		uint32_t returned_bytes;
		uint32_t flags;
		uint64_t seq_num;
	};

	iocp_server(const ip_address& local_ip);
	iocp_server(const iocp_server&) = delete;
	iocp_server(iocp_server&&) = delete;

	void begin_accept(async_connection* client);

	void stop();
	void wait_until_stopped();

	void* iocp_handle() { return m_handle; }
	intptr_t descriptor() const { return m_socket.descriptor(); }
	socket& raw_socket() { return m_socket; }
	
	ip_address& ip() { return m_socket.local_ip(); }
	const ip_address& ip() const { return m_socket.local_ip(); }

protected:
	async_request* allocate_request();
	void free_request(async_request* request);

	virtual void on_client_connect(async_connection* client) = 0;
	virtual void on_client_disconnect(async_connection* client) = 0;

private:
	socket m_socket;
	pool_allocator<async_request, REQEST_POOL_SIZE> m_req_pool;
	void* m_handle;
	void** m_worker_thread_handles;
	size_t m_num_worker_threads;
	void* m_acceptex_func;

	friend class async_connection;
	friend unsigned long __stdcall iocp_worker_thread_method(void*);
};


}