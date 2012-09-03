/*
	File:		PatchDBHeaps.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/21/94	BET		add #ifdef __SERVER__
		 <2>	 5/12/94	KON		Added GetSegaPtrSize.

	To Do:
*/


/************************************************************************

	These routines do nothing yet, but could be implemented to clean
	up the PermMems heap and to check if it is empty (debug).
		ValidatePermHeapEmpty
		EmptyPermHeap

	See the note in PatchDB.c above DefragmentDB() for more info on useage.

*************************************************************************/

#ifndef PATCHDBHEAPS_H
#define PATCHDBHEAPS_H

#include "Errors.h"
#ifdef unix
#include "NetTypes.h"
#endif

Ptr NewPermPtr(
	long size );

void DisposePermPtr( 
	Ptr myPtr );

/* mostly a debugging call to verify that heap is empty when I think it is */
Err ValidatePermHeapEmpty(
	void);

/* a handy call to initialize the heap at startup (and could be handy during defrag?) */
void EmptyPermHeap(
	void);


/************************************************************************
	What:
		Temp Mems heap allocators.

*************************************************************************/

Ptr NewTempPtr( 
	long size );

void DisposeTempPtr( 
	Ptr myPtr );

long GetSegaPtrSize( Ptr myPtr );

#endif	/* PATCHDBHEAPS_H */


