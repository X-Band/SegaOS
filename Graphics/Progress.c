/*
	File:		Progress.c

	Contains:	Progress Bar Manager

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  7/9/94	JBH		Added timer progress proc.
		 <1>	  7/8/94	JBH		first checked in

	To Do:
*/



#include "SegaOS.h"
#include "Progress.h"
#include "Time.h"
#include "ProgressPriv.h"
#include "Reliability.h"
#include "Heaps.h"
#include "Errors.h"
#include "DBTypes.h"
#include "PatchDBHeaps.h"


#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif



/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
ProgressGlobals gProgress;
#endif

short				_InitProgressProcs ( void );
Progress *			_SpawnProgressProc ( ProgressRec *progData, long data );
short				_DisposeProgressProc( Progress *ref );
short				_SetProgressPosition( Progress *ref, short newPosition );
short				_ProgressIdle( Progress *ref );

long
_ProgressControl ( short command, long data )
{
long offset;
short error;
ProgressGlobals *globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(gProgress,offset);
			
			error = AllocateGlobalSpace ( kProgressManager, offset, sizeof(ProgressGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create progress globals" );
			if ( error != noErr )
				{
				return error;
				}
			
			/* install our selectors */
			SetDispatchedFunction ( kInitProgressProcs,	kProgressManager,	_InitProgressProcs );
			SetDispatchedFunction ( kSpawnProgressProc,	kProgressManager,	_SpawnProgressProc );
			SetDispatchedFunction ( kDisposeProgressProc,kProgressManager,	_DisposeProgressProc );
			SetDispatchedFunction ( kSetProgressPosition,	kProgressManager,	_SetProgressPosition );
			SetDispatchedFunction ( kProgressIdle,		kProgressManager,	_ProgressIdle );
			break;
		
		case kSoftInialize:
			/* here we set globals */
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


short
_InitProgressProcs ( void )
{
#ifdef SIMULATOR
Ptr progress;
short err;

	/* this one inits the database for the standard progress procs */
	/* this is used to update new RAM instances. the ROM build will probably */
	/* use this somehow? */
	
	err = -1;
	
	/* install the simple progress */
	progress = GetSimpleProgressProc();
	ASSERT_MESG( progress, "Can't load simple progress" );
	if ( progress )
		{
		err = DBAddItem( kProgressType, kSimpleProgress, progress );
		DisposeMemory( progress );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding progress proc to DB" );
			return err;
			}
		}
	
	/* install the timer progress */
	progress = GetTimerProgressProc();
	ASSERT_MESG( progress, "Can't load timer progress" );
	if ( progress )
		{
		err = DBAddItem( kProgressType, kCountdownTimerProgress, progress );
		DisposeMemory( progress );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding progress proc to DB" );
			return err;
			}
		}

#endif
	return noErr;
}

Progress *
_SpawnProgressProc ( ProgressRec *progData, long data )
{
ProgressProc	proc;
Progress		*progress;
SegaErr			err;

	progress = 0L;
	
	proc = (ProgressProc) DBGetItem ( kProgressType, progData->progressProcID );
	if ( proc )
		{
			progress = NewMemory( kTemp, sizeof(Progress) );
			ASSERT_MESG( progress, "Can't allocate progress data" );
			if (progress) {
				progress->proc = proc;
				progress->data = data;
				
				err = (*proc) (kInitProgressProcCmd, progData, data, &progress->procRef);
				ASSERT_MESG ( !err, "Error initing progress proc" );

				if ( err != noErr )
					{
					DisposeMemory ( progress );
					progress = 0L;
					}
			}
		}
	
	return progress;

}

short
_DisposeProgressProc( Progress *ref )
{
SegaErr err;
	
	if (ref) {
		err = (*ref->proc) ( kDisposeProgressProcCmd, 0, ref->data, &ref->procRef );
		ASSERT_MESG ( !err, "Error disposing progress proc" );
	
		DisposeMemory( ref );
	} else {
		err = 1;		/* ===BRAIN DAMAGE=== pick a real value here! */
	}
	
	return err;
}

short
_SetProgressPosition( Progress *ref, short newPosition )
{
	SegaErr err;
	if (ref) {
		ASSERT_MESG(ref->proc, "No progress proc to call - you're doomed" );
		err = (*ref->proc) (kSetProgressPositionCmd, 0L, newPosition, &ref->procRef);
	} else {
	}
	
	return err;
}

short
_ProgressIdle( Progress *ref )
{
	SegaErr err;
	ASSERT_MESG(ref->proc, "No progress proc to call - you're doomed" );
	err = (*ref->proc) (kIdleProgressCmd, 0L, 0L, &ref->procRef);
	
	return err;
}