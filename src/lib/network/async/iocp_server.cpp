#include "iocp_server.h"


#include <WinSock2.h>
#include <mswsock.h>
#include "async_connection.h"

#include <iostream>

void wsa_error_1(const char* msg)
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

crlib::iocp_server::async_request::async_request() :
	type(),
	sock(nullptr),
	buffer_size(0),
	buffer(nullptr),
	returned_bytes(0),
	flags(0),
	seq_num(0)
{
	ZeroMemory(reserved, sizeof(reserved));
}

crlib::iocp_server::iocp_server(const ip_address& local_ip) :
	m_socket(SOCKET_PROTOCOL_TCP),
	m_handle(nullptr),
	m_worker_thread_handles(nullptr),
	m_num_worker_threads(0),
	m_acceptex_func(nullptr)
{
	m_socket.bind(local_ip);
	m_socket.listen();

	m_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	if (m_handle == nullptr)
	{
		throw std::runtime_error("CreateIOCP failed");
	}

	if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_socket.descriptor()), m_handle, 0, 0) == nullptr)
	{
		throw std::runtime_error("CreateIoCompletionPort failed");
	}

	SYSTEM_INFO sys_info = { 0 };
	GetSystemInfo(&sys_info);

	m_num_worker_threads = sys_info.dwNumberOfProcessors;
	m_worker_thread_handles = new HANDLE[m_num_worker_threads];

	for (size_t i = 0; i < m_num_worker_threads; ++i)
	{
		m_worker_thread_handles[i] = CreateThread(nullptr, 0, crlib::iocp_worker_thread_method, reinterpret_cast<LPVOID>(this), 0, 0);

		if (m_worker_thread_handles[i] == nullptr)
		{
			throw std::runtime_error("CreateThread failed");
		}
	}

	GUID acceptex_guid = WSAID_ACCEPTEX;
	DWORD bytes = 0;

	WSAIoctl(m_socket.descriptor(), SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptex_guid, sizeof(GUID), &m_acceptex_func, sizeof(LPFN_ACCEPTEX), &bytes, nullptr, nullptr);

	if (m_acceptex_func == nullptr)
	{
		throw std::runtime_error("WSAIoctl failed");
	}
}

void crlib::iocp_server::begin_accept(async_connection* client)
{
	async_request* request = m_req_pool.allocate();
	m_req_pool.construct(request);

	request->type = ASYNC_REQUEST_TYPE_ACCEPT;
	request->sock = client;
	request->buffer_size = sizeof(sockaddr) + 16;
	request->buffer = new char[request->buffer_size];

	if (reinterpret_cast<LPFN_ACCEPTEX>(m_acceptex_func)(m_socket.descriptor(), request->sock->descriptor(), request->buffer, 0, 0, request->buffer_size, nullptr, reinterpret_cast<LPOVERLAPPED>(request->reserved)) == FALSE)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			wsa_error_1("AcceptEx");
			throw std::runtime_error("AcceptEx failed");
		}
	}
}

void crlib::iocp_server::stop()
{
	PostQueuedCompletionStatus(m_handle, 0, 0, nullptr);
}

void crlib::iocp_server::wait_until_stopped()
{
	DWORD wait_result;

	do
	{
		wait_result = WaitForMultipleObjects(m_num_worker_threads, m_worker_thread_handles, TRUE, INFINITE);
	} while (wait_result >= WAIT_OBJECT_0 + m_num_worker_threads);
}

crlib::iocp_server::async_request* crlib::iocp_server::allocate_request()
{
	async_request* ptr = m_req_pool.allocate();
	m_req_pool.construct(ptr);
	return ptr;
}

void crlib::iocp_server::free_request(async_request* request)
{
	m_req_pool.deallocate(request);
}

unsigned long __stdcall crlib::iocp_worker_thread_method(void* data_ptr)
{
	DWORD bytes = 0;
	ULONG_PTR unused = 0;
	crlib::iocp_server* iocp = reinterpret_cast<crlib::iocp_server*>(data_ptr);
	intptr_t local_sock = iocp->descriptor();
	crlib::iocp_server::async_request* request;

	while (true)
	{
		if (GetQueuedCompletionStatus(iocp->iocp_handle(), &bytes, &unused, reinterpret_cast<LPOVERLAPPED*>(&request), INFINITE) == FALSE)
		{
			return 1;
			//wsa_error_1("GetQueuedCompletionStatus");
			//throw std::runtime_error("GetQueuedCompletionStatus failed");
		}

		if (request == nullptr)
		{ 
			return 0; 
		}

		request->returned_bytes = bytes;

		if (request->returned_bytes == 0 && (request->type == crlib::iocp_server::ASYNC_REQUEST_TYPE_READ || request->type == crlib::iocp_server::ASYNC_REQUEST_TYPE_WRITE))
		{
			iocp->on_client_disconnect(request->sock);
			iocp->free_request(request);
			continue;
		}

		switch (request->type)
		{

		case crlib::iocp_server::ASYNC_REQUEST_TYPE_ACCEPT:
		{
			intptr_t remote_sock = request->sock->descriptor();

			if (setsockopt(remote_sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&local_sock), sizeof(intptr_t)) != 0)
			{
				throw std::runtime_error("setsockopt failed");
			}

			sockaddr_in addr = { 0 };
			int size = sizeof(addr);

			if (getpeername(remote_sock, reinterpret_cast<sockaddr*>(&addr), &size) == SOCKET_ERROR)
			{
				throw std::runtime_error("getpeername failed");
			}

			request->sock->ip() = ip_address(addr.sin_addr.S_un.S_addr, addr.sin_port);
			iocp->on_client_connect(request->sock);
			delete[] request->buffer;
			iocp->free_request(request);

			break;
		}

		case crlib::iocp_server::ASYNC_REQUEST_TYPE_READ:
			request->sock->on_data_received(request);
			break;

		case crlib::iocp_server::ASYNC_REQUEST_TYPE_WRITE:
			request->sock->on_data_sent(request);
			iocp->free_request(request);
			break;

		default:
			throw std::runtime_error("Invalid IO operation");
			break;
		}
	}

	return 0;
}
