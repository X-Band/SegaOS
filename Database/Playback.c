/*
	File:		Playback.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	  6/4/94	SAH		Tweaked for GetData manager.
		 <4>	 5/26/94	BET		Update for managerized Disk Transport.
		 <3>	 5/26/94	SAH		GetManagerGlobals unbiases the globals ptr for you now.
		 <2>	 5/26/94	KON		Add support for dispatcher.
		 <3>	 5/25/94	KON		Turn session model into a single page version, very similar to
									PICT. Can now receive and playback streams under server control
									during connect, and viewing under user control other times.
		 <2>	 5/25/94	KON		Not tested, in marginal condition. Will probably scale back
									plans and back this out.
		 <1>	 5/24/94	KON		first checked in

	To Do:
*/

#include "Capture.h"
#include "Heaps.h"
#include "Errors.h"
#include "GetData.h"
#include "Playback.h"
#include "PlaybackPriv.h"
#include "DBTypes.h"


//
// Prototypes - dispatched functions
//

void _BeginSessionPlayback( DBID sessionID );
Boolean _SessionExists( DBID sessionID );
void _PlaybackNextStream( void );
void _PlaybackPreviousStream( void );
Boolean _DoesNextSessionStreamExist( void );
Boolean _DoesPreviousSessionStreamExist( void );

//
// not exported, but dispatched
//
void _PlaybackCurrentStream( void );
void PlaybackCurrentStream( void ) = 
	CallDispatchedFunction( kPlaybackCurrentStream );

#ifndef SIMULATOR
PlaybackGlobals playback;
#endif	

long
_PlaybackControl ( short command, long data )
{
long offset;
short error;
PlaybackGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(playback,offset);
			
			error = AllocateGlobalSpace( kPlaybackManager, offset,
				sizeof(PlaybackGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create playback globals" );
			if ( error != noErr )
				{
				return error;
				}
		
			/* install our selectors */
			SetDispatchedFunction ( kBeginSessionPlayback,		kPlaybackManager,	_BeginSessionPlayback );
			SetDispatchedFunction ( kSessionExists,				kPlaybackManager,	_SessionExists );
			SetDispatchedFunction ( kPlaybackNextStream,		kPlaybackManager,	_PlaybackNextStream );
			SetDispatchedFunction ( kPlaybackCurrentStream,		kPlaybackManager,	_PlaybackCurrentStream );
			SetDispatchedFunction ( kPlaybackPreviousStream,	kPlaybackManager,	_PlaybackPreviousStream );
			SetDispatchedFunction ( kDoesNextSessionStreamExist, kPlaybackManager,	_DoesNextSessionStreamExist );
			SetDispatchedFunction ( kDoesPreviousSessionStreamExist, kPlaybackManager,	_DoesPreviousSessionStreamExist );
			break;
		
		case kSoftInialize:
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}

void _BeginSessionPlayback( DBID sessionID )
{
	REFGLOBAL( playback, playbackSessionData ) = (sessionData *)DBGetItem( kSessionType, sessionID );
	if( REFGLOBAL( playback, playbackSessionData ) )
	{
		REFGLOBAL( playback, curStreamIDIndex ) = 0;
		REFGLOBAL( playback, curStreamIndex ) = 0;
		PlaybackCurrentStream( );
	}
}

Boolean _SessionExists( DBID sessionID )
{
	if( DBGetItem( kSessionType, sessionID ) )
		return true;
	else
		return false;
}

static void _PlaybackCurrentStream( void )
{
short result;

	REFGLOBAL( playback, streamData ) = DBGetItem( kStreamType, REFGLOBAL( playback, playbackSessionData )->IDarray[REFGLOBAL(playback, curStreamIDIndex)] );
	REFGLOBAL( playback, curStreamSize ) = DBGetItemSize( kStreamType, REFGLOBAL( playback, playbackSessionData )->IDarray[REFGLOBAL(playback, curStreamIDIndex)] );
//
// grab stream data local
//
	SetGetDataProcLocal();

	result = 0;
	while( (result != kStreamDone) )
	{
		result = ProcessServerData();

	}
	
//
// grab stream data network again
//	
	SetGetDataProcNetwork();

}

void _PlaybackNextStream( void )
{
	REFGLOBAL( playback, curStreamIndex )++ ;
	if( REFGLOBAL( playback, curStreamIndex ) >= REFGLOBAL( playback, playbackSessionData )->numStreams )
	{
		ERROR_MESG( "There is no next stream to playback!" );
		REFGLOBAL( playback, curStreamIndex ) = REFGLOBAL( playback, playbackSessionData )->numStreams-1;
	}
	PlaybackCurrentStream();
}

void _PlaybackPreviousStream( void )
{
	REFGLOBAL(playback, curStreamIndex)--;
	if( REFGLOBAL( playback, curStreamIndex ) < 0 )
	{
		ERROR_MESG( "There is no previous stream to playback!" );
		REFGLOBAL( playback, curStreamIndex ) = 0;
	}
	PlaybackCurrentStream();
}


OSErr _GetLocalDataSync( unsigned long length, Ptr address )
{
PlaybackGlobals	*myGlobals;
short	error;
unsigned long	iii;

	error = GetManagerGlobals( kPlaybackManager, (Ptr *)&myGlobals, 0L );
	ASSERT_MESG( !error, "Couldn't get globals" );

	for( iii = 0; iii < length; iii++ )
	{
		if( myGlobals->curStreamIndex > myGlobals->curStreamSize )
		{
			ERROR_MESG( "Reading more data than in local stream." )
			break;
		}

		*address++ = myGlobals->streamData[ myGlobals->curStreamIndex++ ];
	}
	return noErr;
}

unsigned long _GetLocalDataBytesReady( void )
{
PlaybackGlobals	*myGlobals;
short	error;

	error = GetManagerGlobals( kPlaybackManager, (Ptr *)&myGlobals, 0L );
	ASSERT_MESG( !error, "Couldn't get globals" );

	return ( myGlobals->curStreamSize - myGlobals->curStreamIndex + 1 );
}

short _GetLocalDataError( void )
{
	return 0;
}

Boolean _DoesNextSessionStreamExist( void )
{
Boolean result;

	result = false;
	if( ( REFGLOBAL( playback, curStreamIndex ) +1 < REFGLOBAL(playback, playbackSessionData)->numStreams) )
	{
		if( DBGetItem( kStreamType, REFGLOBAL( playback, playbackSessionData )->IDarray[REFGLOBAL( playback, curStreamIDIndex)+1 ]  ) )
			result = true;
	}		
	return result;
}

Boolean _DoesPreviousSessionStreamExist( void )
{
Boolean result;

	result = false;
	if( (REFGLOBAL( playback, curStreamIndex)-1 >= 0 ) )
	{
		if( DBGetItem( kStreamType, REFGLOBAL(playback, playbackSessionData)->IDarray[REFGLOBAL( playback, curStreamIDIndex)-1 ] ) )
			result = true;
	}		
	return result;
}
















