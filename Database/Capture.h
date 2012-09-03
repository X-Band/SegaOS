/*
	File:		Capture.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 7/21/94	BET		add #ifdef __SERVER__
		 <2>	 5/25/94	DJ		integrated into new Sega project
		 <4>	 5/25/94	KON		Update to use dispatcher.
		 <3>	 5/25/94	KON		Turn session model into a single page version, very similar to
									PICT. Can now receive and playback streams under server control
									during connect, and viewing under user control other times.
		 <2>	 5/25/94	KON		Update for V0.
		 <1>	 5/24/94	KON		first checked in

	To Do:
*/

#ifndef __Capture__
#define __Capture__

#ifndef __DataBase__
#include "DataBase.h"
#endif

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

//
// standard session ID's that can be retrieved later
//
#define kCatapultSession	0
#define kAdvertiserSession	1

#define kSessionNameSize	16

typedef struct
{
	char	sessionName[kSessionNameSize+1];			//extra character to buffer for C-string
} sessionName;

typedef struct
{
	sessionName	sessionName;			//extra character to buffer for C-string
	short		numStreams;
	DBID		IDarray[1];
} sessionData;

#ifdef __SERVER__
// do something so the server can use the file

#else

void BeginSession( short numStreams, sessionName *theName, DBID sessionID ) =
	CallDispatchedFunction( kBeginSession );

void DeleteSession( DBID sessionID ) =
	CallDispatchedFunction( kDeleteSession );

void EndSession( void ) =
	CallDispatchedFunction( kEndSession );

void BeginStreamCapture( short size ) =
	CallDispatchedFunction( kBeginStreamCapture );

void AddDataToStream( short size, Ptr theData ) =
	CallDispatchedFunction( kAddDataToStream );
#endif //__SERVER__


#endif __Capture__