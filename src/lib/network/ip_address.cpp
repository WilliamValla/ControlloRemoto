#include "ip_address.h"

#include <Ws2tcpip.h>

crlib::ip_address::ip_address(const std::string& str, uint16_t port) :
	m_address(0),
	m_port(htons(port))
{
	IN_ADDR addr = { 0 };

	if (InetPton(AF_INET, str.c_str(), &addr) != 1)
	{
		throw std::runtime_error("Could not convert string to ip address");
	}

	m_address = addr.S_un.S_addr;
}

crlib::ip_address& crlib::ip_address::operator=(ip_address other)
{
	swap(*this, other);
	return *this;
}

uint16_t crlib::ip_address::port() const
{
	return ntohs(m_port);
}

std::string crlib::ip_address::to_string() const
{
	char buff[16] = { 0 };
	uint32_t addr_copy = m_address;
	InetNtop(AF_INET, &addr_copy, buff, 16);
	return std::string(buff);
}

void crlib::swap(ip_address& a, ip_address& b)
{
	using std::swap;
	swap(a.m_address, b.m_address);
	swap(a.m_port, b.m_port);
}

std::ostream& crlib::operator<<(std::ostream& stream, const crlib::ip_address& ip)
{
	stream << ip.to_string() << ":" << ip.port();
	return stream;
}
