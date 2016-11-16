#include "tcp_client.h"

#include <utility>

crlib::tcp_client::tcp_client(const ip_address& local_ip) :
	m_socket(SOCKET_PROTOCOL_TCP)
{
	m_socket.bind(local_ip);
}

crlib::tcp_client::tcp_client(tcp_client&& other) :
	m_socket(std::move(other.m_socket))
{
}

crlib::tcp_client& crlib::tcp_client::operator=(tcp_client other)
{
	swap(*this, other);
	return *this;
}

void crlib::tcp_client::connect(const ip_address& host)
{
	m_socket.connect(host);
}

void crlib::tcp_client::disconnect()
{
	m_socket.disconnect();
}

void crlib::tcp_client::close()
{
	m_socket.close();
}

void crlib::swap(tcp_client & a, tcp_client & b)
{
	using std::swap;
	swap(a.m_socket, b.m_socket);
}
