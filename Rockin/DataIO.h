#pragma once

#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <iostream>

namespace Core
{
	/*
	Tokenizer - Class that helps factories parse data files. The next() method takes
	a reference to an input stream and extracts the next well-formed token found.
	IMPORTANT: initialize() must be called before use. Otherwise no tokens
	will be recognized.
	*/
	class Tokenizer
	{
	public:
		static void initialize();
		static std::string next(std::istream &ist);
	private:
		static std::vector<std::regex> m_regex;
	};
}