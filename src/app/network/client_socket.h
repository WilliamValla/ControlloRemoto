#pragma once

#include <crlib/string/string.h>
#include <crlib/threading/thread.h>
#include <crlib/threading/blocking_queue.h>
#include <crlib/threading/waitable_event.h>
#include <crlib/network/tcp/tcp_client.h>
#include <crlib/network/udp/udp_socket.h>
#include <crlib/network/network_stream.h>

class client_socket
{
public:
    client_socket();
    client_socket(crlib::tcp_client* client);
    ~client_socket();

    void send(const crlib::string& str);
    void connect_to(const crlib::ip_address& ip);

    crlib::udp_socket* udp_socket() { return m_udp_socket; }

    const crlib::ip_address& ip() { return m_client->local_ip(); }
    const crlib::ip_address& remote_ip() { return m_client->remote_ip(); }

protected:
    void recv_thread_method();
    void send_thread_method();
    void connect_thread_method();
    void recv_datagram_method();

private:
    crlib::tcp_client* m_client;
    crlib::udp_socket* m_udp_socket;
    crlib::network_stream m_stream;
    crlib::blocking_queue<crlib::string> m_send_queue;
    crlib::waitable_event m_recv_event;
    crlib::waitable_event m_recv_dgram_event;
    crlib::waitable_event m_connect_event;
    crlib::thread m_recv_thread;
    crlib::thread m_send_thread;
    crlib::thread* m_connect_thread;
    crlib::thread* m_datagram_thread;
};
