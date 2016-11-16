#pragma once

#include <string>
#include "udp_packet.h"
#include "..\socket.h"

namespace crlib
{

//Synchronous datagram socket.
class udp_socket
{
public:
	udp_socket(const ip_address& local_ip);
	udp_socket(const std::string& ip = "127.0.0.1", uint16_t port = 0);
	udp_socket(const udp_socket& other);
	udp_socket(udp_socket&& other);

	udp_socket& operator=(udp_socket other);

	template<size_t PktSize>
	size_t send_to(const ip_address& remote_ip, udp_packet<PktSize>* pkt);

	template<size_t PktSize, typename Allocator = std::allocator<udp_packet<PktSize>>>
	udp_packet<PktSize>* recv(ip_address& from, Allocator alloc = Allocator());

	void close();

	ip_address& local_ip() { return m_socket.local_ip(); }
	const ip_address& local_ip() const { return m_socket.local_ip(); }

	socket& raw_socket() { return m_socket; }
	const socket& raw_socket() const { return m_socket; }

	friend void swap(udp_socket& a, udp_socket& b);

private:
	socket m_socket;
};

template<size_t PktSize>
inline size_t udp_socket::send_to(const ip_address& remote_ip, udp_packet<PktSize>* pkt)
{
	return m_socket.send_to(reinterpret_cast<char*>(pkt->data()), pkt->size(), remote_ip);
}

template<size_t PktSize, typename Allocator>
udp_packet<PktSize>* udp_socket::recv(ip_address& from, Allocator alloc)
{
	udp_packet<PktSize> pkt* = alloc.allocate(1);
	alloc.construct(pkt);
	size_t bytes = m_socket.recv_from(pkt.data(), pkt.capacity(), from);
	pkt->m_size = bytes;
	pkt->m_write_index = bytes;
	return pkt;
}

} //namespace