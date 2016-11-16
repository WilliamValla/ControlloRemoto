#pragma once

#include <atomic>
#include <cstdint>
#include "..\..\memory\byte_buffer_wrapper.h"


//Don't include the entire Windows.h for 1 func
extern "C"
{
	__declspec(dllimport) uint64_t __stdcall GetTickCount64(void);
}


namespace crlib
{

//Disable struct packing (will cause wrong sizeof() results)
#pragma pack(push, 1)
struct udp_packet_header
{
	uint16_t size;
	uint32_t seq_num;
	uint64_t time_stamp;
};
#pragma pack(pop)


template<uint16_t Size>
class udp_packet : public byte_buffer_wrapper
{
public:
	udp_packet(uint8_t* buffer, size_t size);
	udp_packet() : byte_buffer_wrapper(m_data + sizeof(udp_packet_header), Size) {}

	void write_header();
	udp_packet_header read_header();

	uint8_t* data() { return m_data; }

	static constexpr size_t capacity() { return  sizeof(udp_packet_header) + Size; }

	uint16_t size() const { return header_size() + payload_size(); }
	constexpr uint16_t header_size() const { return static_cast<uint16_t>(sizeof(udp_packet_header)); }
	uint16_t payload_size() const { return static_cast<uint16_t>(m_write_index); }

private:
	uint8_t m_data[sizeof(udp_packet_header) + Size];

	friend class udp_socket;
};


template<uint16_t Size>
inline udp_packet<Size>::udp_packet(uint8_t* buffer, size_t size) :
	byte_buffer_wrapper(m_data + sizeof(udp_packet_header), Size)
{
	if (size > Size)
	{
		throw std::runtime_error("Invalid buffer size");
	}

	std::memcpy(m_data, buffer, size);
	m_write_index = size - sizeof(udp_packet_header);
}

template<uint16_t Size>
inline void udp_packet<Size>::write_header()
{
	static std::atomic<uint32_t> nums = 0;
	byte_buffer_wrapper buffer(m_data, sizeof(udp_packet_header));
	buffer << size();
	buffer << (nums++);
	buffer << GetTickCount64();
}

template<uint16_t Size>
inline udp_packet_header udp_packet<Size>::read_header()
{
	byte_buffer_wrapper buffer(m_data, sizeof(udp_packet_header));
	return *buffer.read<udp_packet_header>();
}
}