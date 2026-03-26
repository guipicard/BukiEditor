#pragma once
#include "ILogger.h"
#include <fstream>
#include <iostream>

namespace buki {
	struct LogFile : public ILogger {
		LogFile();
		virtual ~LogFile() override;
		virtual void LogError(std::string _text) override;
		virtual void LogSdlError() override;
		virtual void LogWarning(std::string _text) override;
		virtual void LogSuccess(std::string _text) override;
		virtual void LogMessage(std::string _text) override;
	private:
		std::ofstream MyFile;
	};
}