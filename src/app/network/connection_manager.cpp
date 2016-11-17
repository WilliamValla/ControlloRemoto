#include "connection_manager.h"

#include "client_socket.h"
#include "server_socket.h"
#include "../ui/login_window.h"

connection_manager& connection_manager::instance()
{
    static connection_manager manager;
    return manager;
}

connection_manager::connection_manager() :
    m_server(nullptr),
    m_client(nullptr)
{
    login_window* login_w = &login_window::instance();
    connect(this, &connection_manager::on_connected, login_w, &login_window::on_connected);
    connect(this, &connection_manager::on_connection_failed, login_w, &login_window::on_connection_failed);
    connect(this, &connection_manager::on_client_connected, login_w, &login_window::on_client_connected);
}

void connection_manager::host_server(const crlib::ip_address& ip)
{
    m_server = new server_socket;
    m_server->bind(ip);
    m_server->start_accept_client();
}

void connection_manager::connect_to_server(const crlib::ip_address& ip)
{
    if (m_client == nullptr)
    {
        m_client = new client_socket;
    }

    m_client->connect_to(ip);
}

void connection_manager::send_message(const crlib::string& str)
{
    m_client->send(str);
}
