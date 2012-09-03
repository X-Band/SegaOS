/*
	File:		CapturePriv.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 5/25/94	DJ		integrated into new Sega project
		 <1>	 5/25/94	KON		first checked in

	To Do:
*/


typedef
struct CaptureGlobals
{
	Ptr streamData;
	short curStreamIndex;
	short curStreamSize;
	short curStreamIDIndex;
	sessionData *theSessionData;
	DBID sessionID;
} CaptureGlobals;

#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	CaptureGlobals
#else
extern CaptureGlobals capture;
#endif
