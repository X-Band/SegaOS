/*
	File:		PlaybackPriv.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  6/4/94	SAH		Added prototypes for private local stream stuff.
		 <1>	 5/26/94	KON		first checked in

	To Do:
*/



typedef
struct PlaybackGlobals
{
	sessionData *playbackSessionData;
	short	curStreamIDIndex;
	short	curStreamIndex;
	short	curStreamSize;
	Ptr		streamData;
} PlaybackGlobals;

#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	PlaybackGlobals
#else
extern PlaybackGlobals playback;
#endif

void _PlaybackCurrentStream( void ) =
	CallDispatchedFunction( kPlaybackCurrentStream );


/* these are here for GetData.c */
OSErr _GetLocalDataSync( unsigned long length, Ptr address );
unsigned long _GetLocalDataBytesReady( void );
short _GetLocalDataError( void );
