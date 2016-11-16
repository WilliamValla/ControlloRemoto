#pragma once

#include <algorithm>
#include <cctype>
#include <limits>
#include <string>
#include <vector>

namespace crlib
{

namespace string_utils
{
	inline bool starts_with(const std::string& str, char c)
	{
		return str.empty() ? false : str[0] == c;
	}

	inline bool ends_with(const std::string& str, char c)
	{
		return str.empty() ? false : str[str.size() - 1] == c;
	}

	inline bool is_whitespace(const std::string& str)
	{
		return str.empty() || (std::find_if_not(str.begin(), str.end(), ::isspace) == str.end());
	}

	inline void trim_start(std::string& str)
	{
		str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), ::isspace));
	}

	inline void trim_end(std::string& str)
	{
		str.erase(std::find_if_not(str.rbegin(), str.rend(), ::isspace).base(), str.end());
	}

	inline void trim(std::string& str)
	{
		trim_start(str);
		trim_end(str);
	}

	inline std::string ctrim_start(const std::string& str)
	{
		auto iter = std::find_if_not(str.begin(), str.end(), ::isspace);
		return iter == str.end() ? str : std::string(iter, str.end());
	}

	inline std::string ctrim_end(const std::string& str)
	{
		auto iter = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
		return iter == str.end() ? str : std::string(str.begin(), iter);
	}

	inline std::string ctrim(const std::string& str)
	{
		return ctrim_start(ctrim_end(str));
	}

	inline void split(const std::string& str, std::vector<std::string>& out_vec, char delim = ' ', bool auto_trim = true, bool add_empty = false)
	{
		std::string::size_type prev_index = 0;
		std::string::size_type index = str.find(delim);
		std::string sub_str;

		while (index != std::string::npos)
		{
			sub_str = str.substr(prev_index, index - prev_index);

			if (add_empty || !is_whitespace(sub_str))
			{
				if (auto_trim)
				{
					trim(sub_str);
				}

				out_vec.emplace_back(sub_str);
			}

			prev_index = index + 1;
			index = str.find(delim, prev_index);
		}

		if (prev_index < str.size() - 1)
		{
			sub_str = str.substr(prev_index, str.size() - prev_index);

			if (add_empty || !is_whitespace(sub_str))
			{
				if (auto_trim)
				{ 
					trim(sub_str);
				}

				out_vec.emplace_back(sub_str);
			}
		}
	}

	inline bool is_alpha(const std::string& str)
	{
		return std::find_if_not(str.begin(), str.end(), [](char c) { return ::isalpha(c); }) == str.end();
	}

	inline bool is_num(const std::string& str)
	{
		return std::find_if_not(str.begin(), str.end(), [](char c) { return ::isdigit(c); }) == str.end();
	}

	inline bool is_alpha_num(const std::string& str)
	{
		return std::find_if_not(str.begin(), str.end(), [](char c) { return ::isdigit(c) || ::isalpha(c); }) == str.end();
	}

	inline void to_lower(std::string& str)
	{
		for (char& c : str)
		{
			c = std::tolower(c);
		}
	}

	inline std::string cto_lower(const std::string& str)
	{
		std::string result = str;

		for (char& c : result)
		{
			c = std::tolower(c);
		}

		return result;
	}

	inline void to_upper(std::string& str)
	{
		for (char& c : str)
		{
			c = std::toupper(c);
		}
	}

	inline std::string cto_upper(const std::string& str)
	{
		std::string result = str;

		for (char& c : result)
		{
			c = std::toupper(c);
		}

		return result;
	}

	inline void to_camel_case(std::string& str)
	{
		if (str.size() > 0)
		{
			to_lower(str);
			str[0] = std::toupper(str[0]);
		}
	}

	inline std::string to_camel_case(const std::string& str)
	{
		if (str.size() > 0)
		{
			std::string result = cto_lower(str);
			result[0] = std::toupper(str[0]);
			return result;
		}

		return str;
	}

	inline bool non_cs_equals(const std::string& str1, const std::string& str2)
	{
		if (str1.size() != str2.size())
		{
			return false;
		}

		auto size = str1.size();

		for (std::string::size_type i = 0; i < size; ++i)
		{
			if (std::tolower(str1[i]) != std::tolower(str2[i]))
			{
				return false;
			}
		}

		return true;
	}

	inline bool compare_with_lower(const std::string& str, const std::string& lower)
	{
		auto size = str.size();

		if (size != lower.size())
		{
			return false;
		}

		for (std::string::size_type i = 0; i < size; ++i)
		{
			if (std::tolower(str[i]) != lower[i])
			{
				return false;
			}
		}

		return true;
	}

} //namespace string_utils


template<typename T>
inline std::string string_cast(const T& value) { return std::to_string(value); }
template<> inline std::string string_cast<std::string>(const std::string& str) { return str; }
template<> inline std::string string_cast<bool>(const bool& b) { return b ? "true" : "false"; }

template<typename T>
inline T lexical_cast(const std::string& str) { throw std::runtime_error("Unsupported lexical_cast"); };
template<> inline std::string lexical_cast(const std::string& str) { return str; }
template<> inline int32_t lexical_cast(const std::string& str) { return std::stoi(str); }
template<> inline uint32_t lexical_cast(const std::string& str) { return std::stoul(str); }
template<> inline int64_t lexical_cast(const std::string& str) { return std::stoll(str); }
//TODO: Fix negative number parsing
template<> inline uint64_t lexical_cast(const std::string& str) { return std::stoull(str); }

template<> inline int8_t lexical_cast(const std::string& str)
{
	int32_t i = lexical_cast<int32_t>(str);

	if (i < std::numeric_limits<int8_t>::min() || i > std::numeric_limits<int8_t>::max())
	{
		throw std::bad_cast();
	}

	return static_cast<int8_t>(i);
}

template<> inline uint8_t lexical_cast(const std::string& str)
{
	int32_t i = lexical_cast<int32_t>(str);

	if (i < 0 || i > std::numeric_limits<uint8_t>::max())
	{
		throw std::bad_cast();
	}

	return static_cast<uint8_t>(i);
}

template<> inline int16_t lexical_cast(const std::string& str)
{
	int32_t i = lexical_cast<int32_t>(str);

	if (i < std::numeric_limits<int16_t>::min() || i > std::numeric_limits<int16_t>::max())
	{
		throw std::bad_cast();
	}

	return static_cast<int16_t>(i);
}

template<> inline uint16_t lexical_cast(const std::string& str)
{
	int32_t i = lexical_cast<int32_t>(str);

	if (i < 0 || i > std::numeric_limits<uint16_t>::max())
	{
		throw std::bad_cast();
	}

	return static_cast<uint16_t>(i);
}

template<> inline bool lexical_cast(const std::string& str)
{
	if (string_utils::compare_with_lower(str, "false"))
	{
		return false;
	}

	if (string_utils::compare_with_lower(str, "true"))
	{
		return true;
	}

	throw std::bad_cast();
}

} //namespace