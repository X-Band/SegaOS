/*
	File:		PatchDBHeaps.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 5/23/94	BET		Add some code so handles can be passed to GetPtrSize
		 <4>	 5/19/94	KON		Fix GetSegaPtrSize to handle the MemErr case. IM says it will
									return a nil handle, not so.
		 <3>	 5/12/94	KON		Fixed Mac implementation of GetSegaPtrSize so that it deals with
									dereferenced handles properly. This is what the database always
									returns.
		 <2>	 5/12/94	KON		Added GetSegaPtrSize.

	To Do:
*/

#include "PatchDBHeaps.h"
#include "Errors.h"
#include "Memory.h"

Ptr NewPermPtr( long size )
{
	return NewPtr( size );
}

void DisposePermPtr( Ptr myPtr )
{
	DisposePtr( myPtr );
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

}

long GetSegaPtrSize( Ptr myPtr )
{
long	myResult;
Handle	myHandle = 0;
OSErr	error;
char	blockPoo;

	blockPoo = *((unsigned char *)&myPtr[-12]);
	if (blockPoo & 0x80) {
		myHandle = RecoverHandle( myPtr );
		error = MemErr;
		}
	else {
		myHandle = nil;
		error = noErr;
		}
	if( (myHandle != 0) && (error == noErr) )
		myResult = GetHandleSize( myHandle );
	else
		myResult = GetPtrSize( myPtr );

	ASSERT( myResult );
	return myResult;
}
	
