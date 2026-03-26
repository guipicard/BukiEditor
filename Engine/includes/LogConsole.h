#pragma once
#include "ILogger.h"
enum struct EColor
{
	ECOLOR_BLUE = 1,
	ECOLOR_GREEN = 2,
	ECOLOR_LIGHTBLUE = 3,
	ECOLOR_RED = 4,
	ECOLOR_PURPLE = 5,
	ECOLOR_YELLOW = 6,
	ECOLOR_WHITE = 7,
	ECOLOR_GREY = 8,
	ECOLOR_MEDIUMBLUE = 9,
	ECOLOR_LIGHTGREEN = 10,
	ECOLOR_TURQUOISE = 11,
	ECOLOR_LIGHTRED = 12
	//ECOLOR_BLUE = 1,
	//ECOLOR_BLUE = 1,
};

namespace buki {
	struct LogConsole : public ILogger {
		LogConsole();
		virtual ~LogConsole() override;
		virtual void LogError(std::string _text) override;
		virtual void LogSdlError() override;
		virtual void LogWarning(std::string _text) override;
		virtual void LogSuccess(std::string _text) override;
		virtual void LogMessage(std::string _text) override;
	private:
		HANDLE hConsole;
	};
}
