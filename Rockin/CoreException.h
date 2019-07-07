#pragma once
#include <exception>
#include <string>

namespace Core
{
	/*
	CoreException - Extension of std::exception that allows for easy passing of error codes.
	*/

	class CoreException : public std::exception
	{
	public:
		CoreException(const char *what) :
			exception{ what }, errCode { 0 }
		{}
		CoreException(const char *what, int code) :
			exception{ what }, errCode { code }
		{}
		int errCode;
	};
}