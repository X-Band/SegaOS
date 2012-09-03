/*
	File:		Playback.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 7/21/94	BET		add #ifdef __SERVER__
		 <4>	  6/4/94	SAH		Removed the GetData things as they're now managerized in
									GetData.c
		 <3>	 5/26/94	BET		Update for managerized Disk Transport.
		 <2>	 5/26/94	KON		Add dispatcher support.
		 <3>	 5/25/94	KON		Turn session model into a single page version, very similar to
									PICT. Can now receive and playback streams under server control
									during connect, and viewing under user control other times.
		 <2>	 5/25/94	KON		Update for V0.
		 <1>	 5/24/94	KON		first checked in

	To Do:
*/

#ifndef __Playback__
#define __Playback__

#ifndef __DataBase__
#include "DataBase.h"
#endif

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifdef __SERVER__
// do something so the server can use the file

#else

void BeginSessionPlayback( DBID sessionID ) =
	CallDispatchedFunction( kBeginSessionPlayback );

Boolean SessionExists( DBID sessionID ) =
	CallDispatchedFunction( kSessionExists );

void PlaybackNextStream( void ) =
	CallDispatchedFunction( kPlaybackNextStream );

void PlaybackPreviousStream( void ) =
	CallDispatchedFunction( kPlaybackPreviousStream );

Boolean DoesNextSessionStreamExist( void ) =
	CallDispatchedFunction( kDoesNextSessionStreamExist );

Boolean DoesPreviousSessionStreamExist( void ) =
	CallDispatchedFunction( kDoesPreviousSessionStreamExist );
#endif //__SERVER__

#endif __Playback__