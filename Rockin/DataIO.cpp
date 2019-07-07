#include "DataIO.h"
#include <exception>

namespace Core
{
	std::vector<std::regex> Tokenizer::m_regex;

	void Tokenizer::initialize()
	{
		// Populate m_regex with necessary regular expressions,
		// in order of precedence

		m_regex.clear();
		std::string regStr;

		// Quoted strings
		regStr = "\"[^\"]*\"";
		m_regex.emplace_back(regStr, std::regex_constants::icase);

		// Single character tokens
		regStr = "[{},]";
		m_regex.emplace_back(regStr, std::regex_constants::icase);

		// Unquoted strings
		regStr = "[a-z][a-z0-9]*";
		m_regex.emplace_back(regStr, std::regex_constants::icase);

		// Floats
		regStr = "-*[0-9]*\\.[0-9]+";
		m_regex.emplace_back(regStr, std::regex_constants::icase);

		// Ints
		regStr = "-*[0-9]+";
		m_regex.emplace_back(regStr, std::regex_constants::icase);

	}

	std::string Tokenizer::next(std::istream &ist)
	{
		char c;
	
		// Parse out any preceding whitespace of newlines

		while (true)
		{
			// Return a single whitespace character at EOF
			if (!(ist.get(c))) return " ";
			if (' ' == c || '\n' == c) continue;
			ist.putback(c);
			break;
		}

		// Build current block of characters

		bool quoted{ false };
		std::string s;

		while (true)
		{
			ist.get(c);
			if (!ist)
			{
				ist.clear(std::ios_base::goodbit);
				break;
			}
			if ((' ' == c || '\n' == c) && !quoted)
			{
				ist.putback(c);
				break;
			}
			if ('\"' == c)
			{
				if (quoted) quoted = false;
				else quoted = true;
			}
			// Fix for newline characters
			if ('n' == c && s.length() > 0 && '\\' == s[s.length() - 1])
			{
				s.erase(s.end() - 1);
				c = '\n';
			}

			s += c;
		}

		// Attempt to match

		for (const auto &regex : m_regex)
		{
			std::smatch match;
			if (std::regex_search(s, match, regex) && match.position(0) == 0)
			{
				// Match found: Return current token and replace unused characters

				std::string token{ match.str() };
				
				// Remove quotes from quoted strings
				if (token[0] == '\"') token = token.substr(1, token.length() - 2);
				
				auto stringBeginIndex = s.length() - 1;
				auto stringEndIndex = match.length() - 1;

				for (int i = stringBeginIndex; i > stringEndIndex; --i)
				{
					ist.putback(s[i]);
				}

				return token;
			}
		}

		// If no regular expression match was found, throw an exception.

		std::string exceptionMessage{ "Unrecognized token found in data file: " + s };
		std::exception e{ exceptionMessage.c_str() };
		throw(e);

		return "";
	}
}