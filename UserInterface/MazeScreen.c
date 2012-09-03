/*
	File:		MazeScreen.c

	Contains:	Secret screen to play the maze

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 8/13/94	JBH		first checked in
	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "Screens.h"
#include "Errors.h"
#include "Heaps.h"
#include "SegaScrn.h"
#include "GraphicsDB.h"
#include "MungeCode.h"
#include "DBTypes.h"
#include "Controls.h"
#include "SegaIn.h"
#include "Secrets.h"
#include "SegaText.h"

//
// Local routines
//

//
// Screen handling routines
//
static long MazePreflight( long initValue );
static void MazeReset( long refCon );
static long MazeSetup( ScreenParams *params );
static long MazeCommand( long command, long refCon, ScreenParams *params );
static void MazeKill( long refCon );

static long MazeDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = MazePreflight( params->initValue );
			break;

		case kDoScreenReset:
			MazeReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = MazeSetup( params );
			break;

		case kDoScreenCommand:
			result = MazeCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			MazeKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Maze Dispatch" );
			break;
			
		}
	return result;
}

static void MazeReset( long refCon )
{
}

static long MazePreflight( long initValue )
{
	return 0;
}

static long MazeSetup( ScreenParams *params )
{
	PlayMaze();
}


static long MazeCommand( long command, long refCon, ScreenParams *params )
{
	if (command & kButtonPressed)
		return 1;	//go back to main screen
	
	return 0;
}

static void MazeKill( long refCon )
{
	EndPlayMaze();	
}

Ptr GetMazeDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) MazeDispatch + 2));
	size = (*(long *) ((Ptr) GetMazeDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Maze proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) MazeDispatch;
#else
		CheckCodeForJTRefs((void *)MazeDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
