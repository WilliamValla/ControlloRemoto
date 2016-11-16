#include "ini_config.h"

#include "file_stream.h"

void crlib::ini_config::load()
{
	file_stream stream(m_path, FILE_PERMISSIONS::READ, FILE_SHARE::READ, FILE_OPEN_MODE::ALWAYS);

	std::shared_ptr<std::string> line;
	std::string::size_type index = 0;
	std::string section = "General";
	std::string key;
	std::string value;

	while ((line = stream.read_line()) != nullptr)
	{
		if (string_utils::starts_with(*line, ';'))
		{
			continue;
		}

		index = line->find(';');

		if (index != std::string::npos)
		{
			line->erase(index);
		}

		if (string_utils::starts_with(*line, '['))
		{
			if (line->size() <= 2 || !string_utils::ends_with(*line, ']'))
			{
				throw std::runtime_error("Invalid INI file");
			}

			section = line->substr(1, line->size() - 2);

			if (string_utils::is_whitespace(section))
			{
				throw std::runtime_error("Invalid INI file");
			}

			string_utils::trim(section);
		}
		else
		{
			index = line->find('=');

			if (index == std::string::npos || index == (line->size() - 1))
			{
				throw std::runtime_error("Invalid INI file");
			}

			key = line->substr(0, index);
			string_utils::trim_end(key);

			value = line->substr(index + 1);
			string_utils::trim_start(value);

			if (string_utils::is_whitespace(key) || string_utils::is_whitespace(value))
			{
				throw std::runtime_error("Invalid INI file");
			}

			m_data[section][key] = value;
		}
	}
}

void crlib::ini_config::save()
{
	file_stream stream(m_path, FILE_PERMISSIONS::WRITE, FILE_SHARE::WRITE, FILE_OPEN_MODE::TRUNCATE_ALWAYS);

	for (auto& entry : m_data)
	{
		stream << '[' << string_utils::to_camel_case(entry.first) << ']' << '\n';

		for (auto& ini_value : entry.second)
		{
			stream << string_utils::to_camel_case(ini_value.first) << '=' << ini_value.second << '\n';
		}

		stream << '\n';
	}
}

bool crlib::ini_config::has_key(const std::string& key, const std::string& section)
{
	auto result = m_data.find(section);

	if (result == m_data.end())
	{
		return false;
	}

	return result->second.find(key) != result->second.end();
}

void crlib::ini_config::set(const std::string& key, const char* value, const std::string& section)
{
	m_data[section][key] = value;
}

size_t crlib::ini_config::size()
{
	size_t size = 0;

	for (auto& entry : m_data)
	{
		for (auto& sub_entry : entry.second)
		{
			++size;
		}
	}

	return size;
}
