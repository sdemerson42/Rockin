#include "DataIO.h"

namespace Core
{

	std::string nextToken(std::istream &ist)
	{
		std::string token;
		char c = ' ';
		char type;
		bool quotes{ false };

		// Skip whitespace and newline...
		while (' ' == c || '\n' == c)
		{
			ist.get(c);
			if (!ist)
			{
				token = " ";
				return token;
			}
		}

		// Deduce token type

		if (isalpha(c)) type = 's';
		else if (c == '\"')
		{
			type = 's';
			quotes = true;
		}
		else if (isdigit(c) || c == '-' || c == '.') type = '#';
		else
		{
			token += c;
			return token;
		}
		if (c != '\"')
			token += c;

		while (true)
		{
			ist.get(c);
			if (!ist)
			{
				return token;
			}

			if ('s' == type)
			{
				if (quotes)
				{
					if (c == '\"')
					{
						break;
					}
					if (c == 'n' && token.size() > 0 && token[token.size() - 1] == '\\')
					{
						token[token.size() - 1] = '\n';
					}
					else
					{
						token += c;
					}
				}
				else
				{
					if (isalnum(c)) token += c;
					else
					{
						ist.putback(c);
						break;
					}
				}
			}
			else if ('#' == type)
			{
				if (isdigit(c) || c == '.') token += c;
				else
				{
					ist.putback(c);
					if (token[0] == '.') token = "0" + token;
					else if (token[0] == '-' && token[1] == '.') token.insert(std::begin(token) + 1, '0');
					break;
				}
			}
		}
		return token;
	}
}