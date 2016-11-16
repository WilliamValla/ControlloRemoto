#pragma once

#include "iocp_server.h"
#include "tcp_async_connection.h"

namespace crlib
{

/*
Implementation of an IOCP tcp server.
The PktSize template is the maximum packet size that the server can receive.
*/
template<size_t PktSize>
class tcp_async_server : public iocp_server
{
public:
	typedef void(*connect_callback)(tcp_async_connection<PktSize>*);
	typedef void(*disconnect_callback)(tcp_async_connection<PktSize>*);

	//Bind the server to a local ip. Will throw an exception if the IP is invalid/the bind call fails.
	tcp_async_server(const ip_address& ip) : iocp_server(ip), m_conn_callback(nullptr) {}
	//Bind the server to a local ip. Will throw an exception if the IP is invalid/the bind call fails.
	tcp_async_server(const std::string& ip = "127.0.0.1", uint16_t port = 0) : tcp_async_server(ip_address(ip, port)) {}
	tcp_async_server(const tcp_async_server<PktSize>&) = delete;
	tcp_async_server(tcp_async_server<PktSize>&&) = delete;

	tcp_async_server<PktSize>& operator=(tcp_async_server<PktSize>) = delete;


	void set_connect_callback(connect_callback callback);
	void set_disconnect_callback(disconnect_callback callback);

	//Begin an async accept. Optional allocator must be of tcp_async_connection<PktSize> template.
	template<typename Allocator = std::allocator<tcp_async_connection<PktSize>>>
	void begin_accept(Allocator alloc = Allocator());

protected:
	using iocp_server::begin_accept;
	virtual void on_client_connect(async_connection* client) override;
	virtual void on_client_disconnect(async_connection* client) override;

private:
	connect_callback m_conn_callback;
	disconnect_callback m_disconn_callback;
};


template<size_t PktSize>
inline void tcp_async_server<PktSize>::set_connect_callback(connect_callback callback)
{
	m_conn_callback = callback;
}

template<size_t PktSize>
inline void tcp_async_server<PktSize>::set_disconnect_callback(disconnect_callback callback)
{
	m_disconn_callback = callback;
}

template<size_t PktSize>
template<typename Allocator>
inline void tcp_async_server<PktSize>::begin_accept(Allocator alloc)
{
	tcp_async_connection<PktSize>* conn = alloc.allocate(1);
	alloc.construct(conn, this);
	begin_accept(dynamic_cast<async_connection*>(conn));
}

template<size_t PktSize>
inline void tcp_async_server<PktSize>::on_client_connect(async_connection* client)
{
	if (m_conn_callback)
	{
		m_conn_callback(dynamic_cast<tcp_async_connection<PktSize>*>(client));
	}
}

template<size_t PktSize>
inline void tcp_async_server<PktSize>::on_client_disconnect(async_connection* client)
{
	if (m_disconn_callback)
	{
		m_disconn_callback(dynamic_cast<tcp_async_connection<PktSize>*>(client));
	}
}

} //namespace