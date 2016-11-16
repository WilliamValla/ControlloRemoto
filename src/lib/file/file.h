#pragma once

#include <cstdint>
#include <string>

namespace crlib
{

/*
Abstraction class for a file. 
Encapsulates a file path (string) and offers some utility functions.
*/
class file
{

public:
	file(const std::string& path) : m_path(path) {};

	bool exists() noexcept;
	bool is_directory() noexcept;

	void del();
	void move(const file& new_path);
	void copy(const file& to, bool overwrite);

	uint64_t size();

	std::string& path() { return m_path; }
	const std::string& path() const { return m_path; }

private:
	std::string m_path;
};


} //namespace