#pragma once

#include "tcp_client.h"
#include "..\ip_address.h"
#include "..\socket.h"


namespace crlib
{

//Packet based synchronous tcp server.
class tcp_server
{
public:
	tcp_server(const ip_address& local_ip);
	tcp_server(const std::string& ip = "127.0.0.1", uint16_t port = 0);
	tcp_server(const tcp_server& other);
	tcp_server(tcp_server&& other);

	tcp_server& operator=(tcp_server other);

	template<typename Allocator = std::allocator<tcp_client>>
	tcp_client* accept(Allocator alloc = Allocator());

	void close(); 

	ip_address& local_ip() { return m_socket.local_ip(); }
	const ip_address& local_ip() const { return m_socket.local_ip(); }

	socket& raw_socket() { return m_socket; }
	const socket& raw_socket() const { return m_socket; }

	friend void swap(tcp_server& a, tcp_server& b);

private:
	socket m_socket;
};


template<typename Allocator>
inline tcp_client* tcp_server::accept(Allocator alloc)
{
	socket conn = m_socket.accept();
	tcp_client* client = alloc.allocate(1);
	alloc.construct(client, conn);
	return client;
}

} //namespace