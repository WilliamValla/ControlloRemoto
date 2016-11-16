#pragma once

#include <memory>
#include "file.h"
#include "..\utils\string_utils.h"

namespace crlib
{

enum class FILE_PERMISSIONS
{
	READ = 0x1,
	WRITE = 0x2,
	BOTH = READ | WRITE
};

enum class FILE_SHARE
{
	READ = 0x1,
	WRITE = 0x2,
	BOTH = READ | WRITE
};

enum class FILE_OPEN_MODE
{
	ALWAYS = 4,
	EXISTING = 3,
	TRUNCATE_ALWAYS = 2,
	TRUNCATE_EXISTING = 5
};

enum class FILE_ACCESS
{
	SEQUENTIAL = 0x08000000,
	RANDOM = 0x10000000
};

enum class FILE_SEEK_MODE
{
	START = 0,
	END = 2,
	CURRENT = 1
};

#define RW_BUFFER_SIZE 2048

/*
Class that offers buffered read/write file IO.
*/
class file_stream
{
public:
	file_stream(const std::string& path, FILE_PERMISSIONS permissions = FILE_PERMISSIONS::BOTH, FILE_SHARE sharing = FILE_SHARE::BOTH, FILE_OPEN_MODE open_mode = FILE_OPEN_MODE::ALWAYS, FILE_ACCESS access = FILE_ACCESS::SEQUENTIAL);
	file_stream(const file& f, FILE_PERMISSIONS permissions = FILE_PERMISSIONS::BOTH, FILE_SHARE sharing = FILE_SHARE::BOTH, FILE_OPEN_MODE open_mode = FILE_OPEN_MODE::ALWAYS, FILE_ACCESS access = FILE_ACCESS::SEQUENTIAL);
	file_stream(const file_stream&) = delete;
	file_stream(file_stream&&) = delete;
	~file_stream();

	file_stream& operator=(file_stream) = delete;

	//Unbuffered read of a chunk of file. Returns the actual amount of bytes read.
	size_t read(uint8_t* buffer, size_t buffer_size);

	//Buffered write. Will not actually write to the OS buffers until the internal buffer is filled.
	void write(const uint8_t* buffer, size_t buffer_size);


	std::shared_ptr<std::string> read_line(bool skip_empty = true);
	void read_lines(std::vector<std::shared_ptr<std::string>>& vec, bool skip_emptry = true);

	template<typename T>
	file_stream& operator<<(const T& t);

	file_stream& operator<<(const char* str);

	template<typename T>
	file_stream& operator>>(T& t);

	//Flush the internal buffer to the OS buffers.
	void flush();
	//Flushes the OS buffers, actually writing to disk.
	void flush_os();

	//Move the file pointer
	int64_t seek(int64_t distance, FILE_SEEK_MODE mode = FILE_SEEK_MODE::CURRENT);

	uint64_t size();

private:
	uint8_t* m_read_buffer;
	uint8_t* m_read_ptr;
	uint64_t m_read_buffer_size;

	uint8_t* m_write_buffer;
	uint64_t m_write_buffer_size;

	void* m_handle;

	uint64_t read_buffer(uint8_t** out_end = nullptr);
};

template<typename T>
inline file_stream& file_stream::operator<<(const T& t)
{
	write(reinterpret_cast<const uint8_t*>(&t), sizeof(T));
	return *this;
}

template<typename T>
inline file_stream& file_stream::operator>>(T& t)
{
	read(*t, sizeof(T));
	return *this;
}

//Template specializations
template<>
inline file_stream& file_stream::operator<<(const std::string& str)
{
	write(reinterpret_cast<const uint8_t*>(str.data()), str.length());
	return *this;
}

} //namespace