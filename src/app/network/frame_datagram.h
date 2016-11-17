#pragma once

#include <cstdint>
#include <crlib/network/udp/udp_socket.h>

#pragma pack(push, 1)
struct frame_datagram_header
{
    uint64_t m_frame_num;
    uint32_t m_fragment_offset;
    uint16_t m_fragment_size;
};
#pragma pack(pop)

class frame_datagram
{
public:
    static size_t constexpr size() { return 1000 + sizeof(frame_datagram_header); }

    frame_datagram(uint64_t frame_num, uint32_t fragmnet_offset, uint8_t* frame_data, size_t size);
    frame_datagram();
    ~frame_datagram() { delete[] m_data; }

    frame_datagram_header read_header();
    uint8_t* frame_data() { return m_data + sizeof(frame_datagram_header); }

    void recv();
    void send();

private:
    uint8_t* m_data;
};
