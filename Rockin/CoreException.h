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

		// Error codes
		// (NOTE: Opting for enum over enum class to avoid casting of error codes.)

		enum ErrCode{badLogFile = 1, badToken = 2, badBlueprintFile = 3,
		missingDefaultSceneData = 4, missingSceneData = 5, missingSubsceneData = 6,
		missingTilesetData = 7, badSceneInheritanceData = 8};
	};
}