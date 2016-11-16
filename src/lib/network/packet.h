#pragma once

#include "..\memory\byte_buffer_wrapper.h"

namespace crlib
{

template<uint16_t Size, typename Header>
class packet : public byte_buffer_wrapper
{
public:
	packet() : byte_buffer_wrapper(m_data + sizeof(Header), Size) {}

	uint8_t* data() { return m_data; }

	void write_header();
	Header read_header();

	static constexpr size_t capacity() { return  sizeof(Header) + Size; }

	uint16_t size() const { return header_size() + payload_size(); }
	constexpr uint16_t header_size() const { return static_cast<uint16_t>(sizeof(Header)); }
	uint16_t payload_size() const { return static_cast<uint16_t>(m_write_index); }

private:
	uint8_t m_data[Size + sizeof(Header)];
};


template<uint16_t Size, typename Header>
inline void packet<Size, Header>::write_header()
{
	Header h;
	byte_buffer_wrapper buff(m_data, sizeof(Header));
	buff << h;
}

template<uint16_t Size, typename Header>
inline Header packet<Size, Header>::read_header()
{
	Header h;
	byte_buffer_wrapper buff(m_data, sizeof(Header));
	buff >> h;
}

} //namespace