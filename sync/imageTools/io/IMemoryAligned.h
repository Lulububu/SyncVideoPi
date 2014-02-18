// Copyright (C) 2013 Matt Ownby
// You are free to use this for educational/non-commercial purposes only
// http://my-cool-projects.blogspot.com

#ifndef IMEMORYALIGNED_H
#define IMEMORYALIGNED_H

class IMemoryAligned
{
public:
	// same usage as posix_memalign but returns true on success or false on error
	virtual bool MyMalloc(void **memptr, size_t alignment, size_t size) = 0;

	virtual void Free(void *pMem) = 0;
};

#endif // IMEMORYALIGNED_H
