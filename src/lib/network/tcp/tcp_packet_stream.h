#pragma once

#include <cstdint>
#include "tcp_client.h"
#include "tcp_packet.h"

namespace crlib
{

/*
Helper class that splits a tcp byte stream in packets.
Packets are delimited by the size found in the header.
*/
template<size_t PktSize>
class tcp_packet_stream
{
public:
	tcp_packet_stream(tcp_client& sock);
	tcp_packet_stream(const tcp_packet_stream<PktSize>&) = delete;
	tcp_packet_stream(tcp_packet_stream<PktSize>&&) = delete;
	tcp_packet_stream& operator=(tcp_packet_stream<PktSize>) = delete;

	void recv(tcp_packet<PktSize>* out_pkt);
	void send(tcp_packet<PktSize>* pkt);

private:
	uint8_t m_buff[tcp_packet<PktSize>::capacity()];
	uint8_t* m_current_ptr;
	size_t m_bytes;
	socket m_sock;

	bool can_read_pkt(uint8_t* ptr, size_t bytes);
	void read_pkt(tcp_packet<PktSize>* out_pkt, uint8_t*& ptr, size_t& bytes);
};

template<size_t PktSize>
inline tcp_packet_stream<PktSize>::tcp_packet_stream(tcp_client& sock) :
	m_current_ptr(nullptr),
	m_bytes(0),
	m_sock(sock.raw_socket())
{
}

template<size_t PktSize>
inline void tcp_packet_stream<PktSize>::recv(tcp_packet<PktSize>* out_pkt)
{
	if (m_bytes > 0)
	{
		if (can_read_pkt(m_current_ptr, m_bytes))
		{
			read_pkt(out_pkt, m_current_ptr, m_bytes);
			return;
		}
		else
		{
			std::memmove(m_buff, m_current_ptr, m_bytes);
			m_current_ptr = m_buff;
			m_bytes = m_sock.recv(reinterpret_cast<char*>(m_buff + m_bytes), tcp_packet<PktSize>::capacity() - m_bytes);
		}
	}
	else
	{
		m_current_ptr = m_buff;
		m_bytes = m_sock.recv(reinterpret_cast<char*>(m_current_ptr), tcp_packet<PktSize>::capacity());
	}

	if (m_bytes == 0)
	{
		throw std::runtime_error("Invalid packet");
	}

	if (!can_read_pkt(m_current_ptr, m_bytes))
	{
		throw std::runtime_error("Invalid packet");
	}

	read_pkt(out_pkt, m_current_ptr, m_bytes);
}

template<size_t PktSize>
inline void tcp_packet_stream<PktSize>::send(tcp_packet<PktSize>* pkt)
{
	pkt->write_header();
	m_sock->send(reinterpret_cast<char*>(pkt->m_data()), pkt->m_size());
}

template<size_t PktSize>
inline bool tcp_packet_stream<PktSize>::can_read_pkt(uint8_t* ptr, size_t bytes)
{
	if (bytes >= sizeof(tcp_packet_header))
	{
		tcp_packet_header* header = reinterpret_cast<tcp_packet_header*>(ptr);

		if (bytes >= header->size)
		{
			return true;
		}

		return false;
	}

	return false;
}

template<size_t PktSize>
inline void tcp_packet_stream<PktSize>::read_pkt(tcp_packet<PktSize>* out_pkt, uint8_t*& ptr, size_t& bytes)
{
	tcp_packet_header* header = reinterpret_cast<tcp_packet_header*>(ptr);
	new(out_pkt) tcp_packet<PktSize>(ptr, static_cast<size_t>(header->size));
	bytes -= header->size;
	ptr += header->size;
}

} //namespace
