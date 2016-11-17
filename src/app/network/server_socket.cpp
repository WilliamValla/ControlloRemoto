#include "server_socket.h"

#include "connection_manager.h"
#include "client_socket.h"

server_socket::server_socket() :
    m_thread(std::bind(&server_socket::accept_thread_method, this)),
    m_server(),
    m_udp_socket(nullptr),
    m_event()
{
    m_thread.start();
}

void server_socket::bind(const crlib::ip_address& ip)
{
    m_server.start_listen(ip);
    delete m_udp_socket;
    m_udp_socket = new crlib::udp_socket(ip);
}

void server_socket::start_accept_client()
{
    m_event.signal();
}

void server_socket::accept_thread_method()
{
    while (true)
    {
        m_event.wait();

        client_socket* client = new client_socket(m_server.accept());
        connection_manager::instance().m_client = client;

        emit connection_manager::instance().on_client_connected();
    }
}
