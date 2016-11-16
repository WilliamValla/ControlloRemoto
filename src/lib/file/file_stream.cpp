#include "file_stream.h"

#include <algorithm>
#include <stdexcept>
#include <Windows.h>

crlib::file_stream::file_stream(const std::string& path, FILE_PERMISSIONS permissions, FILE_SHARE sharing, FILE_OPEN_MODE open_mode, FILE_ACCESS access) :
	m_read_buffer(new uint8_t[RW_BUFFER_SIZE]),
	m_read_ptr(m_read_buffer),
	m_read_buffer_size(0),
	m_write_buffer(new uint8_t[RW_BUFFER_SIZE]),
	m_write_buffer_size(0),
	m_handle(CreateFile(path.c_str(), static_cast<DWORD>(permissions), static_cast<DWORD>(sharing), nullptr, static_cast<DWORD>(open_mode), static_cast<DWORD>(access), nullptr))
{
	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("CreateFile failed");
	}
}

crlib::file_stream::file_stream(const file& f, FILE_PERMISSIONS permissions, FILE_SHARE sharing, FILE_OPEN_MODE open_mode, FILE_ACCESS access) :
	file_stream(f.path(), permissions, sharing, open_mode, access)
{
}

crlib::file_stream::~file_stream()
{
	if (m_write_buffer_size > 0)
	{
		flush();
	}

	delete[] m_write_buffer;
	delete[] m_read_buffer;
	CloseHandle(m_handle);
}

std::shared_ptr<std::string> crlib::file_stream::read_line(bool skip_empty)
{
	std::shared_ptr<std::string> ptr(nullptr);
	uint8_t* end;
	uint8_t* find;

	while (true)
	{
		end = m_read_buffer + m_read_buffer_size;

		if (m_read_ptr == end && (read_buffer(&end) == 0))
		{
			break;
		}

		find = std::find(m_read_ptr, end, '\n');

		if (ptr == nullptr)
		{
			ptr.reset(new std::string);
		}

		ptr->append(m_read_ptr, find);

		if (find != end)
		{
			m_read_ptr = std::min(find + 1, end);
			break;
		}
		else if (read_buffer() == 0)
		{
			break;
		}
	}

	if (ptr != nullptr)
	{
		string_utils::trim(*ptr);

		if (skip_empty && string_utils::is_whitespace(*ptr))
		{
			return read_line(skip_empty);
		}
	}

	return ptr;
}

void crlib::file_stream::read_lines(std::vector<std::shared_ptr<std::string>>& vec, bool skip_empty)
{
	std::shared_ptr<std::string> line;

	while ((line = read_line(skip_empty)) != nullptr)
	{
		vec.push_back(line);
	}
}

size_t crlib::file_stream::read(uint8_t* buffer, size_t buffer_size)
{
	DWORD bytes = 0;

	if (!ReadFile(m_handle, buffer, buffer_size, &bytes, nullptr))
	{
		throw std::runtime_error("ReadFile failed");
	}

	return static_cast<size_t>(bytes);
}

void crlib::file_stream::write(const uint8_t* buffer, size_t buffer_size)
{
	uintptr_t offset = 0;

	while (buffer_size > 0)
	{
		size_t to_copy = RW_BUFFER_SIZE - m_write_buffer_size;

		if (to_copy == 0)
		{
			flush();
		}
		else if (to_copy >= buffer_size)
		{
			std::memcpy(m_write_buffer + m_write_buffer_size, buffer + offset, buffer_size);
			m_write_buffer_size += buffer_size;
			buffer_size = 0;			
		}
		else
		{
			std::memcpy(m_write_buffer + m_write_buffer_size, buffer + offset, to_copy);
			m_write_buffer_size += to_copy;
			offset += to_copy;
			buffer_size -= to_copy;
		}
	}
}

void crlib::file_stream::flush()
{
	DWORD bytes = 0;

	if (!WriteFile(m_handle, m_write_buffer, m_write_buffer_size, &bytes, nullptr))
	{
		throw std::runtime_error("WriteFile failed");
	}

	m_write_buffer_size = 0;
}

void crlib::file_stream::flush_os()
{
	if (!FlushFileBuffers(m_handle))
	{
		throw std::runtime_error("FlushFileBuffers failed");
	}
}

int64_t crlib::file_stream::seek(int64_t distance, FILE_SEEK_MODE mode)
{
	LARGE_INTEGER move = { 0 };
	move.QuadPart = distance;

	LARGE_INTEGER new_pos = { 0 };

	if (!SetFilePointerEx(m_handle, move, &new_pos, static_cast<DWORD>(mode)))
	{
		throw std::runtime_error("SetFilePointerEx failed");
	}

	if (distance > 0)
	{
		m_read_ptr = m_read_buffer;
	}

	return new_pos.QuadPart;
}

uint64_t crlib::file_stream::size()
{
	LARGE_INTEGER size = { 0 };

	if (!GetFileSizeEx(m_handle, &size))
	{
		throw std::runtime_error("GetFileSizeEx failed");
	}

	return size.QuadPart;
}

uint64_t crlib::file_stream::read_buffer(uint8_t** out_end)
{
	m_read_buffer_size = read(m_read_buffer, RW_BUFFER_SIZE);
	m_read_ptr = m_read_buffer;

	if (out_end != nullptr)
	{
		*out_end = m_read_buffer + RW_BUFFER_SIZE;
	}

	return m_read_buffer_size;
}

crlib::file_stream& crlib::file_stream::operator<<(const char* str)
{
	write(reinterpret_cast<const uint8_t*>(str), strlen(str));
	return *this;
}
