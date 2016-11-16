#include "tcp_server.h"

#include <utility>

crlib::tcp_server::tcp_server(const ip_address& local_ip) :
	m_socket(SOCKET_PROTOCOL_TCP)
{
	m_socket.bind(local_ip);
	m_socket.listen();
}

crlib::tcp_server::tcp_server(const std::string& ip, uint16_t port) :
	tcp_server(ip_address(ip, port))
{
}

crlib::tcp_server::tcp_server(const tcp_server& other) :
	m_socket(other.m_socket)
{
}

crlib::tcp_server::tcp_server(tcp_server&& other) :
	m_socket(std::move(other.m_socket))
{
}

crlib::tcp_server& crlib::tcp_server::operator=(tcp_server other)
{
	swap(*this, other);
	return *this;
}

void crlib::tcp_server::close()
{
	m_socket.close();
}

void crlib::swap(tcp_server & a, tcp_server & b)
{
	using std::swap;
	swap(a.m_socket, b.m_socket);
}
