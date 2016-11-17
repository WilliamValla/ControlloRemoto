#pragma once

#include <QObject>
#include <crlib/string/string.h>
#include <crlib/network/ip_address.h>
#include <crlib/network/udp/udp_packet.h>

class client_socket;
class server_socket;

class connection_manager : public QObject
{
    Q_OBJECT
public:
    static connection_manager& instance();

    server_socket* server() { return m_server; }
    client_socket* client() { return m_client; }

    bool is_server() { return m_server != nullptr; }

    void host_server(const crlib::ip_address& ip);
    void connect_to_server(const crlib::ip_address& ip);

    void send_message(const crlib::string& str);

signals:
    void on_connected();
    void on_connection_failed();
    void on_client_connected();

signals:
    void on_msg_received(const crlib::string& msg);

protected:
    connection_manager();

private:
    server_socket* m_server;
    client_socket* m_client;

    friend class server_socket;
};
