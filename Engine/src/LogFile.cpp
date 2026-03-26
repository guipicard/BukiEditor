#include "LogFile.h"
//#include <ctime>
#include <time.h>
#include <SDL_error.h>
#include <SDL.h>


buki::LogFile::LogFile()
{
	 MyFile.open("./assets/LogFile.txt", std::ios_base::app);
	 __time64_t longTime = time(NULL);
	 struct tm newTime;


	 errno_t err = _localtime64_s(&newTime, &longTime);

	 if (err)
	 {
		 LogError("Invalid argument to _localtime64_s.");
		 SDL_Event quitEvent;
		 quitEvent.type = SDL_QUIT;
		 SDL_PushEvent(&quitEvent);
	 }

	 int year = 1900 + newTime.tm_year;
	 int mounth = 1 + newTime.tm_mon;
	 int day = newTime.tm_mday;
	 int hour = 5 + newTime.tm_hour;
	 int min = 30 + newTime.tm_min;
	 int sec = newTime.tm_sec;
	 MyFile << year << ":" << mounth << ":" << day << ":" << hour << "h" << min << "m" << sec << "s" << "\n" << std::endl;
}

buki::LogFile::~LogFile()
{
	MyFile << "\n" << "\n";
	MyFile.close();
}

void buki::LogFile::LogError(std::string _text)
{
	MyFile << "ERROR: " << _text << std::endl;
}

void buki::LogFile::LogSdlError()
{
	MyFile << SDL_GetError() << std::endl;
}

void buki::LogFile::LogWarning(std::string _text)
{
	MyFile << "Warning: " << _text << std::endl;
}

void buki::LogFile::LogSuccess(std::string _text)
{
	MyFile << "Success: " << _text << std::endl;
}

void buki::LogFile::LogMessage(std::string _text)
{
	MyFile << _text << std::endl;
}
