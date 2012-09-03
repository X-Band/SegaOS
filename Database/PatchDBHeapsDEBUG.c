#include "PatchDBHeaps.h"
#include "Errors.h"

#define kMaxDBHeapSize	10000

static long gAllocatedSize = 0;	/* control the size of the heap */

Ptr NewPermPtr( long size )
{
	long *p;

	if(size + gAllocatedSize > kMaxDBHeapSize)
		return(NULL);

	p = (long *)NewPtr( size + sizeof(long));
	*p++ = size;
	gAllocatedSize += size;
	return((Ptr)p);
}

void DisposePermPtr( Ptr myPtr )
{
	long *p;
	
	p = (long *)myPtr;
	p--;
	gAllocatedSize -= *p;
	ASSERT(gAllocatedSize >= 0);
	DisposePtr( (Ptr)p );
}

Ptr NewTempPtr( long size )
{
	return NewPtr( size );
}

void DisposeTempPtr( Ptr myPtr )
{
	DisposePtr( myPtr );
}

/*
	This should check if the heap is totally empty and return kNoError if so.
*/

Err ValidatePermHeapEmpty(
	void)
{
	return(kNoError);
}

/*
	This should initialize the heap to a virginal state!  Yay!
*/

void EmptyPermHeap(
	void)
{
	gAllocatedSize = 0;
}

