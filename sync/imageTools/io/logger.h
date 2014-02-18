// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "../common/mpo_deleter.h"
using namespace std;

class ILogger : public MpoDeleter
{
public:
	virtual void Log(const string &strText) = 0;
	virtual void LogError(const string &strText) = 0;
};

typedef std::shared_ptr <ILogger> ILoggerSPtr;

#endif // LOGGER_H
