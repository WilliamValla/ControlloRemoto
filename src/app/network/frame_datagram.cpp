#include "frame_datagram.h"

#include <crlib/memory/byte_buffer_wrapper.h>
#include "connection_manager.h"
#include "client_socket.h"
#include "server_socket.h"

frame_datagram::frame_datagram(uint64_t frame_num, uint32_t fragmnet_offset, uint8_t* frame_data, size_t size) :
    m_data(new uint8_t[frame_datagram::size()])
{
    crlib::byte_buffer_wrapper stream(m_data, frame_datagram::size());
    stream << frame_num;
    stream << fragmnet_offset;
    stream << size;

    std::memcpy(m_data + sizeof(frame_datagram_header), frame_data, size);
}

frame_datagram::frame_datagram() :
    m_data(new uint8_t[size()])
{
}

frame_datagram_header frame_datagram::read_header()
{
    frame_datagram_header header;
    crlib::byte_buffer_wrapper stream(m_data, sizeof(frame_datagram_header));
    stream >> header;
    return header;
}

void frame_datagram::recv()
{
    connection_manager::instance().client()->udp_socket()->recv(reinterpret_cast<char*>(m_data), frame_datagram::size());
}

void frame_datagram::send()
{
    connection_manager::instance().server()->udp_socket()->send_to(reinterpret_cast<char*>(m_data), frame_datagram::size(), connection_manager::instance().client()->ip());
}
