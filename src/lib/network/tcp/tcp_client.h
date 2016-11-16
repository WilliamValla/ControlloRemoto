#pragma once

#include "..\socket.h"
#include "tcp_packet.h"

namespace crlib
{

/*
Packet based synchronous tcp client.
*/
class tcp_client
{

public:
	tcp_client() : m_socket(SOCKET_PROTOCOL_TCP) {}
	tcp_client(const ip_address& local_ip);
	tcp_client(const std::string& ip, uint16_t port) : tcp_client(ip_address(ip, port)) {}
	tcp_client(const socket& sock) : m_socket(sock) {}
	tcp_client(const tcp_client& other) : m_socket(other.m_socket) {}
	tcp_client(tcp_client&& other);

	tcp_client& operator=(tcp_client other);

	void connect(const ip_address& host);
	void disconnect();

	void close();

	template<size_t PktSize>
	void send(tcp_packet<PktSize>* pkt);

	ip_address& local_ip() { return m_socket.local_ip(); }
	const ip_address& local_ip() const { return m_socket.local_ip(); }

	ip_address& remote_ip() { return m_socket.remote_ip(); }
	const ip_address& remote_ip() const { return m_socket.remote_ip(); }

	socket& raw_socket() { return m_socket; }
	const socket& raw_socket() const { return m_socket; }

	friend void swap(tcp_client& a, tcp_client& b);

private:
	socket m_socket;
};


template<size_t PktSize>
inline void tcp_client::send(tcp_packet<PktSize>* pkt)
{
	pkt->write_header();
	m_socket.send(reinterpret_cast<char*>(pkt->data()), pkt->size());
}

} //namespace