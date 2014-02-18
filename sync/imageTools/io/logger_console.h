// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef STDIO_LOGGER_H
#define STDIO_LOGGER_H

#include "logger.h"

// logs to the console
class ConsoleLogger : public ILogger
{
public:
	static ILoggerSPtr GetInstance();
	void Log(const string &strText);
	void LogError(const string &strText);
private:
	ConsoleLogger();
	~ConsoleLogger();

	void DeleteInstance();
};

#endif // STDIO_LOGGER_H
