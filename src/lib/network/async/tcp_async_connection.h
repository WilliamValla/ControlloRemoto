#pragma once

#include <algorithm>
#include <deque>
#include "async_connection.h"
#include "..\tcp\tcp_packet.h"
#include "..\..\memory\pool_allocator.h"
#include "..\..\threading\spin_lock.h"

#define RECV_PACKET_BUFFER_SIZE 128

namespace crlib
{

/*
Implementation of an async tcp connection, 
usually constructed from tcp_async_server::begin_accept.
The PktSize is the maximums packet receive/send size, and should
be the same as the server.
*/
template<size_t PktSize>
class tcp_async_connection : public async_connection
{
private:
	typedef void(*receive_callback)(tcp_async_connection<PktSize>* connection, tcp_packet<PktSize>* pkt);

	struct prealloc_array
	{
		char buffer[tcp_packet<PktSize>::capacity()];
	};

public:
	tcp_async_connection(iocp_server* ref);
	tcp_async_connection(const tcp_async_connection<PktSize>&) = delete;
	tcp_async_connection(tcp_async_connection&&) = delete;

	tcp_async_connection<PktSize>& operator=(tcp_async_connection<PktSize>) = delete;

	tcp_packet<PktSize>* allocate_pkt();

	using async_connection::begin_send;
	void begin_send(tcp_packet<PktSize>* pkt);
	using async_connection::begin_receive;
	void begin_receive();

	void set_receive_callback(receive_callback callback) { m_receive_callback = callback; }

protected:
	virtual void on_data_received(iocp_server::async_request* req) override;
	virtual void on_data_sent(iocp_server::async_request* req) override;

private:
	std::atomic<uint64_t> m_seq_num;
	std::deque<iocp_server::async_request*> m_recv_queue;
	spin_lock m_lock;
	pool_allocator<prealloc_array, RECV_PACKET_BUFFER_SIZE> m_recv_pool;
	pool_allocator<tcp_packet<PktSize>, RECV_PACKET_BUFFER_SIZE> m_pkt_pool;
	receive_callback m_receive_callback;

	//Returns true if hand enough data to parse all packets
	bool handle_data(iocp_server::async_request* req);
	void handle_queue();
};

template<size_t PktSize>
inline tcp_async_connection<PktSize>::tcp_async_connection(iocp_server* ref) :
	async_connection(ref),
	m_seq_num(0),
	m_recv_queue(),
	m_lock(),
	m_recv_pool(),
	m_pkt_pool(),
	m_receive_callback()
{
}

template<size_t PktSize>
inline tcp_packet<PktSize>* tcp_async_connection<PktSize>::allocate_pkt()
{
	tcp_packet<PktSize>* pkt = m_pkt_pool.allocate();
	m_pkt_pool.construct(pkt);
	return pkt;
}

template<size_t PktSize>
inline void tcp_async_connection<PktSize>::begin_send(tcp_packet<PktSize>* pkt)
{
	pkt->write_header();
	begin_send(reinterpret_cast<char*>(pkt->data()), pkt->size());
}

template<size_t PktSize>
inline void tcp_async_connection<PktSize>::begin_receive()
{
	prealloc_array* buff = m_recv_pool.allocate();
	begin_receive(buff->buffer, tcp_packet<PktSize>::capacity());
}

template<size_t PktSize>
inline void tcp_async_connection<PktSize>::on_data_received(iocp_server::async_request* req)
{
	uint64_t seq_num = req->seq_num;
	uint64_t expected_seq_num = m_seq_num.load();

	if (expected_seq_num == seq_num)
	{
		//Packet in order, handle it 
		handle_data(req);
	}
	else
	{
		//Packet out of order, insert into the queue
		m_lock.lock();
		auto iter = std::find_if(m_recv_queue.begin(), m_recv_queue.end(), [&seq_num](iocp_server::async_request* a) { return a->seq_num > seq_num; });
		m_recv_queue.insert(iter, req);
		m_lock.unlock();
	}

	//Emptry the queue if possible
	handle_queue();
}

template<size_t PktSize>
inline void tcp_async_connection<PktSize>::on_data_sent(iocp_server::async_request* req)
{
	tcp_packet<PktSize>* ptr = reinterpret_cast<tcp_packet<PktSize>*>(req->buffer - sizeof(byte_buffer_wrapper);
	m_pkt_pool.deallocate(ptr);
}

template<size_t PktSize>
inline bool tcp_async_connection<PktSize>::handle_data(iocp_server::async_request * req)
{
	if (req->returned_bytes <= tcp_packet<PktSize>::header_size())
	{
		//Invalid packet
		throw std::runtime_error("Invalid packet");
		return true;
	}

	uint32_t& bytes_remaining = req->returned_bytes;
	uint8_t* ptr = reinterpret_cast<uint8_t*>(req->buffer);

	while (bytes_remaining > 0)
	{
		if (bytes_remaining <= tcp_packet<PktSize>::header_size())
		{
			std::memmove(req->buffer, ptr, bytes_remaining);
			m_lock.lock();
			m_recv_queue.push_front(req);
			m_lock.unlock();
			return false;
		}

		tcp_packet_header* header = reinterpret_cast<tcp_packet_header*>(ptr);

		if (bytes_remaining >= header->size)
		{
			tcp_packet<PktSize>* pkt = m_pkt_pool.allocate();
			m_pkt_pool.construct(pkt, ptr, header->size);

			if (m_receive_callback)
			{
				m_receive_callback(this, pkt);
			}

			m_pkt_pool.deallocate(pkt);
			bytes_remaining -= header->size;
			ptr += header->size;
		}
		else
		{
			//Add to front of receive queue
			std::memmove(req->buffer, ptr, bytes_remaining);
			m_lock.lock();
			m_recv_queue.push_front(req);
			m_lock.unlock();
			return false;
		}
	}

	m_recv_pool.deallocate(reinterpret_cast<prealloc_array*>(req->buffer));
	++m_seq_num;
	return true;
}

template<size_t PktSize>
inline void tcp_async_connection<PktSize>::handle_queue()
{
	while (true)
	{
		m_lock.lock();

		if (m_recv_queue.empty() || m_recv_queue[0]->seq_num != m_seq_num.load())
		{
			m_lock.unlock();
			return;
		}

		auto data1 = m_recv_queue[0];
		m_recv_queue.pop_front();
		m_lock.unlock();

		if (!handle_data(data1))
		{
			m_lock.lock();

			if (m_recv_queue.size() < 2 || m_recv_queue[0]->seq_num != (m_recv_queue[1]->seq_num - 1))
			{
				m_lock.unlock();
				return;
			}

			data1 = m_recv_queue[0];
			m_recv_queue.pop_front();

			auto data2 = m_recv_queue[1];
			m_recv_queue.pop_front();

			m_lock.unlock();

			size_t bytes_to_copy = std::min(tcp_packet<PktSize>::capacity() - data1->returned_bytes, data2->returned_bytes);
			std::memcpy(data1->buffer, data2->buffer, bytes_to_copy);
			data1->returned_bytes += bytes_to_copy;
			data2->returned_bytes -= bytes_to_copy;

			if (data2->returned_bytes > 0)
			{
				std::memmove(data2->buffer, data2->buffer + bytes_to_copy, data2->returned_bytes);
			}

			bool needs_data2 = !handle_data(data1);

			if (needs_data2)
			{
				if (data2->returned_bytes == 0)
				{
					throw std::runtime_error("Invalid packet");
				}

				m_lock.lock();
				m_recv_queue.insert(m_recv_queue.begin() + 1, data2);
				m_lock.unlock();
			}
			else if (data2->returned_bytes > 0)
			{
				handle_data(data2);
			}
		}
	}
}

}