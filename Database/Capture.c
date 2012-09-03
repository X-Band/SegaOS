/*
	File:		Capture.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 6/30/94	SAH		Removed database purge values.
		 <4>	  6/2/94	KON		Fix memory leaks: DisposePtr should be DisposeMemory!
		 <3>	 5/26/94	KON		Fix problems with globals and dispatcher.
		 <2>	 5/25/94	DJ		integrated into new Sega project
		 <4>	 5/25/94	KON		Update to use dispatcher.
		 <3>	 5/25/94	KON		Turn session model into a single page version, very similar to
									PICT. Can now receive and playback streams under server control
									during connect, and viewing under user control other times.
		 <2>	 5/25/94	KON		Complete V0. Not tested yet. Will probably scale back messaging
									plans.
		 <1>	 5/24/94	KON		first checked in

	To Do:
*/

#include "Capture.h"
#include "CapturePriv.h"
#include "Heaps.h"
#include "Errors.h"
#include "DBTypes.h"

//
// Prototypes - dispatched functions
//

void _BeginSession( short numStreams, sessionName *theName, DBID sessionID );
void _DeleteSession( DBID sessionID );
void _EndSession( void );
void _BeginStreamCapture( short size );
void _AddDataToStream( short size, Ptr theData );


#ifndef SIMULATOR
CaptureGlobals capture;
#endif	


long
_CaptureControl ( short command, long data )
{
long offset;
short error;
CaptureGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(capture,offset);
			
			error = AllocateGlobalSpace ( kCaptureManager, offset,
				sizeof(CaptureGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create capture globals" );
			if ( error != noErr )
				{
				return error;
				}
		
			/* install our selectors */
			SetDispatchedFunction ( kBeginSession,			kCaptureManager,	_BeginSession );
			SetDispatchedFunction ( kDeleteSession,			kCaptureManager,	_DeleteSession );
			SetDispatchedFunction ( kEndSession,			kCaptureManager,	_EndSession );
			SetDispatchedFunction ( kBeginStreamCapture,	kCaptureManager,	_BeginStreamCapture );
			SetDispatchedFunction ( kAddDataToStream,		kCaptureManager,	_AddDataToStream );
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



void _BeginSession( short numStreams, sessionName *theName, DBID sessionID )
{
short	iii;

//
// Create the session
//
	REFGLOBAL( capture, sessionID ) = sessionID;
	REFGLOBAL( capture, curStreamIDIndex ) = 0;
	REFGLOBAL( capture, theSessionData ) = (sessionData *) NewMemory( kTemp, sizeof(sessionData) + numStreams * sizeof( DBID ) );
	REFGLOBAL( capture, theSessionData )->sessionName = *theName;
	REFGLOBAL( capture, theSessionData )->numStreams = numStreams;
	for( iii = 0; iii < numStreams; iii++ )
	{
		REFGLOBAL( capture, theSessionData )->IDarray[iii] = DBGetUniqueID( kStreamType );
	}
}

void _EndSession( void )
{

	ASSERT( REFGLOBAL( capture, theSessionData ) );
//
// ==BRAIN DAMAGE== This database item (and all its relatives) should be marked unstable 
// until the end session is successfully reached.
//
	DBAddItem( kSessionType, REFGLOBAL( capture, sessionID ), (Ptr) REFGLOBAL( capture, theSessionData ) );

//
// Mark BeginSession item stable
//
// ==BRAIN DAMAGE==

	DisposeMemory( (Ptr)REFGLOBAL( capture, theSessionData ) );
	
	REFGLOBAL( capture, theSessionData ) = 0;

}

void _DeleteSession( DBID sessionID )
{
short itemCount;
short iii;


	if( DBCountItems( kSessionType ) )
	{
		DBDeleteItem( kSessionType, sessionID );
		
		itemCount = DBCountItems( kStreamType );
		for( iii = 0; iii < itemCount; iii++ )
		{
			DBDeleteItem( kStreamType, DBGetFirstItemID( kStreamType ) );
		}		
	}
}


void _BeginStreamCapture( short size )
{

//
// Create this stream
//
	REFGLOBAL( capture, streamData ) = NewMemory( kTemp, size );
	REFGLOBAL( capture, curStreamSize ) = size;
	REFGLOBAL( capture, curStreamIndex ) = 0;
}

void _AddDataToStream( short size, Ptr theData )
{
short	iii;

	ASSERT( REFGLOBAL( capture, streamData ) );
	ASSERT( REFGLOBAL( capture, theSessionData ) );
	
	for( iii = 0; iii < size; iii++ )
	{
		if( REFGLOBAL( capture, curStreamIndex ) >= REFGLOBAL( capture, curStreamSize ) )
		{
			ERROR_MESG( "Stream index out of range of stream size" );
			break;
		}
		REFGLOBAL( capture, streamData )[ REFGLOBAL( capture, curStreamIndex)++] = theData[iii];
	}
	
	if( REFGLOBAL( capture, curStreamIndex ) == REFGLOBAL( capture, curStreamSize ) )
	{
	// We've captured the stream, add it to the DB
		DBAddItem( kStreamType,
			REFGLOBAL( capture, theSessionData )->IDarray[REFGLOBAL( capture, curStreamIDIndex )++],
			REFGLOBAL( capture, streamData ) );	
		DisposeMemory( REFGLOBAL( capture, streamData ) );
		REFGLOBAL( capture, streamData ) = 0;
	}

	//
	// if this is the last stream of the session, end the session
	//
	if( REFGLOBAL( capture, curStreamIDIndex ) == REFGLOBAL( capture, theSessionData )->numStreams )
	{
		EndSession();
	}
}


