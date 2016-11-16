#include "file.h"

#include <Windows.h>

bool crlib::file::exists() noexcept
{
	DWORD attributes = GetFileAttributes(m_path.c_str());
	return attributes != INVALID_FILE_ATTRIBUTES;
}

bool crlib::file::is_directory() noexcept
{
	DWORD attributes = GetFileAttributes(m_path.c_str());
	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

void crlib::file::del()
{
	if (!DeleteFile(m_path.c_str()))
	{
		throw std::runtime_error("DeleteFile failed");
	}
}

void crlib::file::move(const file& new_path)
{
	if (!MoveFile(m_path.c_str(), new_path.m_path.c_str()))
	{
		throw std::runtime_error("MoveFile failed");
	}
}

void crlib::file::copy(const file& to, bool overwrite)
{
	if (!CopyFile(m_path.c_str(), to.m_path.c_str(), !overwrite))
	{
		throw std::runtime_error("CopyFile failed");
	}
}

uint64_t crlib::file::size()
{
	WIN32_FILE_ATTRIBUTE_DATA attributes;

	if (!GetFileAttributesEx(m_path.c_str(), GetFileExInfoStandard, &attributes))
	{
		throw std::runtime_error("GetFileAttributesEx failed");
	}

	LARGE_INTEGER size;
	size.HighPart = attributes.nFileSizeHigh;
	size.LowPart = attributes.nFileSizeLow;

	return size.QuadPart;
}
