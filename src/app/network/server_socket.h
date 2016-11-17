#pragma once

#include <crlib/threading/thread.h>
#include <crlib/threading/waitable_event.h>
#include <crlib/network/tcp/tcp_server.h>
#include <crlib/network/udp/udp_socket.h>


class server_socket
{   
public:
    server_socket();

    void bind(const crlib::ip_address& ip);
    void start_accept_client();

    const crlib::ip_address& ip() { return m_server.local_ip(); }

    crlib::udp_socket* udp_socket() { return m_udp_socket; }

protected:
    void accept_thread_method();

private:
    crlib::thread m_thread;
    crlib::tcp_server m_server;
    crlib::udp_socket* m_udp_socket;
    crlib::waitable_event m_event;
};
