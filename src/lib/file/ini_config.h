#pragma once

#include <functional>
#include <map>
#include "..\utils\string_utils.h"

namespace crlib
{

/*
Class for handling INI configuration.
INI files are split into sections.
Each entry is a key=value pair.
Sections and key are not case sensitive.
*/
class ini_config
{
public:
	struct comparator
	{
		bool operator()(const std::string& a, const std::string& b) const
		{
			return string_utils::cto_lower(a) < string_utils::cto_lower(b);
		}
	};

	//Doesn't actually load the INI file, just sets the file path
	ini_config(const std::string& file_path) : m_path(file_path), m_data() {}
	ini_config(const ini_config&) = delete;
	ini_config(ini_config&&) = delete;
	ini_config& operator=(ini_config) = delete;

	void load();
	void save();

	bool has_key(const std::string& key, const std::string& section = "General");

	//Will only fail when the key is non castable to T
	template<typename T>
	T get(const std::string& key, const std::string& section = "General");

	//No checking is made
	void set(const std::string& key, const char* value, const std::string& section = "General");

	//No checking is made, but T must have a to_string<T> specilization
	template<typename T>
	void set(const std::string& key, const T& value, const std::string& section = "General");


	auto begin() { return m_data.begin(); }
	auto end() { return m_data.end(); }

	//How many key/value pairs are in the config
	size_t size();

private:
	std::string m_path;
	std::map<std::string, std::map<std::string, std::string, comparator>, comparator> m_data;
};

template<typename T>
inline T ini_config::get(const std::string& key, const std::string& section)
{
	return lexical_cast<T>(m_data[section][key]);
}

template<>
inline std::string ini_config::get<std::string>(const std::string& key, const std::string& section)
{
	return m_data[section][key];
}

template<typename T>
inline void ini_config::set(const std::string& key, const T& value, const std::string& section)
{
	m_data[section][key] = string_cast<T>(value);
}

} //namespace