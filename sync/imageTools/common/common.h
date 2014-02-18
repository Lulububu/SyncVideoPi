// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef COMMON_H
#define COMMON_H

#include "datatypes.h"
#include <vector>
using namespace std;

typedef vector<uint8_t> byteSA;

#ifdef WIN32
#define strcasecmp _stricmp
#endif // WIN32

#endif // COMMON_H
