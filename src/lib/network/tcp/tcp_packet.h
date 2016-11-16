#pragma once

#include <atomic>
#include <cstdint>
#include "..\..\memory\byte_buffer_wrapper.h"


namespace crlib
{

enum class TCP_PACKET_OPCODE
{
	//Login/Registration packets.
	REQUEST_REGISTRATION,
	CONFIRM_REGISTRATION,
	DENY_REGISTRATION,
	REQUEST_LOGIN,
	CONFIRM_LOGIN,
	DENY_LOGIN,

	//Friend operations
	SEND_FRIEND_REQUEST,
	RECEIVE_FRIEND_REQUEST,
	BLOCK_USER,
	UNBLOCK_USER,
	BLOCK_NOTIFICATION,
	FRIEND_LIST,
	SEND_STATE_UPDATE,
	USER_CHANGE_STATE,

	//Message exchange
	SEND_MESSAGE,
	RECEIVE_MESSAGE,
	UNREAD_MESSAGE,

	//Desktop sharing
	USER_REQUEST_SHARING,
	USER_REQUEST_STOP_SHARING,

	INVALID_OPCODE
};


//Disable struct packing (will cause wrong sizeof() results)
#pragma pack(push, 1)
struct tcp_packet_header
{
	uint16_t size;
};
#pragma pack(pop)

template<uint16_t Size>
class tcp_packet : public byte_buffer_wrapper
{
public:
	tcp_packet(uint8_t* buffer, size_t size);
	tcp_packet() : byte_buffer_wrapper(m_data + sizeof(tcp_packet_header), Size) {}

	void write_header();
	tcp_packet_header read_header();

	uint8_t* data() { return m_data; }

	uint8_t* payload_data() { return m_data + sizeof(tcp_packet_header); }

	static constexpr size_t capacity() { return  sizeof(tcp_packet_header) + Size; }

	uint16_t size() const { return header_size() + payload_size(); }
	static constexpr uint16_t header_size() { return static_cast<uint16_t>(sizeof(tcp_packet_header)); }
	uint16_t payload_size() const { return static_cast<uint16_t>(m_write_index); }

private:
	uint8_t m_data[sizeof(tcp_packet_header) + Size];
};


template<uint16_t Size>
inline tcp_packet<Size>::tcp_packet(uint8_t* buffer, size_t size) :
	byte_buffer_wrapper(m_data + sizeof(tcp_packet_header), Size)
{
	if (size > Size)
	{
		throw std::runtime_error("Invalid buffer size");
	}

	std::memcpy(m_data, buffer, size);
	m_write_index = size - sizeof(tcp_packet_header);
}

template<uint16_t Size>
inline void tcp_packet<Size>::write_header()
{
	byte_buffer_wrapper buffer(m_data, sizeof(tcp_packet_header));
	buffer << size();
}

template<uint16_t Size>
inline tcp_packet_header tcp_packet<Size>::read_header()
{
	byte_buffer_wrapper buffer(m_data, sizeof(tcp_packet_header));
	return *buffer.read<tcp_packet_header>();
}

} //namespace