#include "LogConsole.h"
#include <cstdlib>
#include <iostream>
#include <SDL_error.h>

buki::LogConsole::LogConsole()
{
	AllocConsole();
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int _r = freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
}

buki::LogConsole::~LogConsole()
{
	FreeConsole();
}

void buki::LogConsole::LogError(std::string _text)
{
	SetConsoleTextAttribute(hConsole, static_cast<int>(EColor::ECOLOR_RED));
	std::cout << "ERROR: " << _text << "\n";

}

void buki::LogConsole::LogSdlError()
{
	LogError(SDL_GetError());
}

void buki::LogConsole::LogWarning(std::string _text)
{
	SetConsoleTextAttribute(hConsole, static_cast<int>(EColor::ECOLOR_YELLOW));
	std::cout << "Warning: " << _text << "\n";
}

void buki::LogConsole::LogSuccess(std::string _text)
{
	SetConsoleTextAttribute(hConsole, static_cast<int>(EColor::ECOLOR_GREEN));
	std::cout << "Success: " << _text << "\n";
}

void buki::LogConsole::LogMessage(std::string _text)
{
	SetConsoleTextAttribute(hConsole, static_cast<int>(EColor::ECOLOR_WHITE));
	std::cout << _text << "\n";
}
