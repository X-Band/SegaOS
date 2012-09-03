/*
	File:		Synch.c

	Contains:	Game Modem Synching Routines

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/18/94	ADS		Dispatcher cleanup
		 <2>	 5/10/94	SAH		Add Header.

	To Do:
*/



#include "SegaOS.h"
#include "synch.h"
#include "SynchPriv.h"
#include "Reliability.h"


#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif



/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
SynchGlobals synch;
#endif

short	_SynchModems ( void );
short	_SynchVbls ( void );


long
_SynchControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			
			/* install our selectors */
			SetDispatchedFunction ( kSynchModems,		kSynchManager,	_SynchModems );
			SetDispatchedFunction ( kSynchVbls,			kSynchManager,	_SynchVbls );
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
#ifdef SIMULATOR
			printf ( "synch code block moved\n");
#endif
			error = noErr;
			break;
					
		case kGlobalsMoved:
#ifdef SIMULATOR
			printf ( "synch global block moved\n");
#endif
			error = noErr;
			break;
		}
		
	return error;
}


short	_SynchModems ( void )
{

	return noErr;
}


short	_SynchVbls ( void )
{

	return noErr;
}


