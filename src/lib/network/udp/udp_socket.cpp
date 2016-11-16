#include "udp_socket.h"

#include <utility>


crlib::udp_socket::udp_socket(const ip_address& local_ip) :
	m_socket(SOCKET_PROTOCOL_UDP)
{
	m_socket.bind(local_ip);
}

crlib::udp_socket::udp_socket(const std::string& ip, uint16_t port) : 
	udp_socket(ip_address(ip, port))
{
}

crlib::udp_socket::udp_socket(const udp_socket& other) :
	m_socket(other.m_socket)
{
}

crlib::udp_socket::udp_socket(udp_socket&& other) :
	m_socket(std::move(other.m_socket))
{
}

crlib::udp_socket& crlib::udp_socket::operator=(udp_socket other)
{
	swap(*this, other);
	return *this;
}

void crlib::udp_socket::close()
{
	m_socket.close();
}

void crlib::swap(udp_socket& a, udp_socket& b)
{
	using std::swap;
	swap(a.m_socket, b.m_socket);
}