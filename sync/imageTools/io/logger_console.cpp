// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#include <stdio.h>
#include "logger_console.h"

ILoggerSPtr ConsoleLogger::GetInstance()
{
	return ILoggerSPtr(new ConsoleLogger(), ConsoleLogger::deleter());
}

void ConsoleLogger::DeleteInstance()
{
	delete this;
}

void ConsoleLogger::Log(const string &strText)
{
	printf("%s\n", strText.c_str());
}

void ConsoleLogger::LogError(const string &strText)
{
	fprintf(stderr, "%s\n", strText.c_str());
}

ConsoleLogger::ConsoleLogger()
{
}

ConsoleLogger::~ConsoleLogger()
{
}
