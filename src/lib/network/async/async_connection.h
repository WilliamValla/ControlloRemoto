#pragma once

#include <atomic>
#include "iocp_server.h"
#include "..\socket.h"

namespace crlib
{
/*
Abstract class for an async client connection,
usually generated from iocp_server::begin_accept.
*/
class async_connection
{
public:
	async_connection(const async_connection&) = delete;
	async_connection(async_connection&&) = delete;

	void close() { m_socket.close(); }

	ip_address& ip() { return m_socket.local_ip(); }
	const ip_address& ip() const { return m_socket.local_ip(); }
	intptr_t descriptor() const { return m_socket.descriptor(); }

protected:
	async_connection(iocp_server* ref);

	void begin_send(char* buff, uint32_t buff_size);
	void begin_receive(char* buff, uint32_t buff_size);

	virtual void on_data_received(iocp_server::async_request* req) = 0;
	virtual void on_data_sent(iocp_server::async_request* req) = 0;

	socket m_socket;
	iocp_server* m_server;

private:
	std::atomic<uint64_t> m_seq_num;

	friend class iocp_server;
	friend unsigned long __stdcall iocp_worker_thread_method(void*);
};


} //namespace