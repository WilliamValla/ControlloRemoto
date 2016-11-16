#include "async_connection.h"

#include <WinSock2.h>

#include <iostream>

void wsa_error(const char* msg)
{
	DWORD error = static_cast<DWORD>(WSAGetLastError());

	std::cout << msg << " failed!" << std::endl;
	std::cout << "Winsock error: " << WSAGetLastError() << std::endl;

	LPSTR str = NULL;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, 0, reinterpret_cast<LPSTR>(&str), 0, NULL) == 0)
	{
		std::cout << "Failed to get description for message." << std::endl;
	}
	else
	{
		std::cout << "Description: " << str << std::endl;
		LocalFree(str);
	}

	throw std::runtime_error("Winsock error");
}


crlib::async_connection::async_connection(iocp_server* ref) :
	m_socket(SOCKET_PROTOCOL_TCP),
	m_server(ref),
	m_seq_num(0)
{
	if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket.descriptor()), m_server->iocp_handle(), 0, 0) == nullptr)
	{
		throw std::runtime_error("CreateIoCompletionPort failed");
	}
}

void crlib::async_connection::begin_send(char * buff, uint32_t buff_size)
{
	iocp_server::async_request* request = m_server->allocate_request();
	request->type = iocp_server::ASYNC_REQUEST_TYPE_WRITE;
	request->buffer = buff;
	request->buffer_size = buff_size;
	request->sock = this;

	WSABUF* wsa_buf = reinterpret_cast<WSABUF*>(&request->buffer_size);

	if (WSASend(m_socket.descriptor(), wsa_buf, 1, nullptr, request->flags, reinterpret_cast<LPOVERLAPPED>(request->reserved), nullptr) != 0)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			wsa_error("WSASend");
			throw std::runtime_error("WSASend failed");
		}
	}
}

void crlib::async_connection::begin_receive(char* buff, uint32_t buff_size)
{
	iocp_server::async_request* request = m_server->allocate_request();
	request->type = iocp_server::ASYNC_REQUEST_TYPE_READ;
	request->buffer = buff;
	request->buffer_size = buff_size;
	request->sock = this;
	request->seq_num = m_seq_num++;

	WSABUF* wsa_buf = reinterpret_cast<WSABUF*>(&request->buffer_size);

	if (WSARecv(m_socket.descriptor(), wsa_buf, 1, nullptr, reinterpret_cast<DWORD*>(&request->flags), reinterpret_cast<LPOVERLAPPED>(request->reserved), nullptr) != 0)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			wsa_error("WSARecv");
			throw std::runtime_error("WSARecv failed");
		}
	}
}
