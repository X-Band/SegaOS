/*
	File:		Results.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 6/14/94	SAH		Managerized.
		 <3>	 5/29/94	DJ		tweaked the #ifdef
		 <2>	 5/26/94	DJ		#ifdefed out routines for SERVER
		 <2>	 5/12/94	KON		Update to use the send Q rather than special messages and
									control for just sending results. Ripped out all the flush code.

	To Do:
*/


#ifndef __Results__
#define __Results__

typedef struct
{
	long	gameID;
	short	userID;
	long	userScore;
	short	opponentID;
	long	opponentScore;
} Result;

typedef struct
{
	short	nextID;
	Result	thisResult;
} FIFOEntry;


#ifndef __SERVER__		// Server doesn't want these cheezeball routines.

#define	kFIFOSize	10


void AddToResultFIFO( Result *myResult ) = 
	CallDispatchedFunction( kAddToResultFIFO );

void ReplaceTopEntryOfResultFIFO( Result *newResult ) = 
	CallDispatchedFunction( kReplaceTopEntryOfResultFIFO );

Result *GetTopEntryOfResultFIFO( void ) = 
	CallDispatchedFunction( kGetTopEntryOfResultFIFO );

Result *GetIndexEntryInResultFIFO( short index ) = 
	CallDispatchedFunction( kGetIndexEntryInResultFIFO );

short CountEntriesInResultFIFO( void ) = 
	CallDispatchedFunction( kCountEntriesInResultFIFO );


#endif __SERVER__


#endif __Results__