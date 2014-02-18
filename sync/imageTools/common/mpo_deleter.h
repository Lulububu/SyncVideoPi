// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef MPO_DELETER_H
#define MPO_DELETER_H

#include <tr1/memory>

using namespace std::tr1;

class MpoDeleter
{
protected:

	virtual void DeleteInstance() = 0;

	class deleter;
	friend class deleter;

	class deleter
	{
	public:
		void operator()(MpoDeleter *p) { p->DeleteInstance(); }
	};

};

#endif // MPO_DELETER_H
