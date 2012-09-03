/*
	File:		LinePath.c

	Contains:	Path proc for line paths.

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	  7/8/94	CMY		Call CheckCodeForJTRefs for the ROMDB build.
		 <5>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <4>	  7/6/94	SAH		Updated to use math manager.
		 <3>	 5/28/94	CMY		Added calls to CheckCodeForJTRefs when adding procs to DB.
		 <2>	 5/28/94	SAH		Really copy the proc rather than just the jump table.
		 <2>	 5/25/94	HEC		new memory manager
		 <1>	 5/16/94	SAH		first checked in

	To Do:
*/

#ifdef ROMDB
#undef SIMULATOR
#endif

#include "SegaOS.h"
#include "Paths.h"
#include "PathProcs.h"
#include "Heaps.h"
#include "Reliability.h"
#include "PatchDBHeaps.h"
#include "Errors.h"
#include "Math.h"
#include "MungeCode.h"


#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif


typedef
struct LinePathState
{
	Point	startPoint;
	Point	stopPoint;
	long	startFrame;
	long	stopFrame;
	long	velocity;
	long	fractH;
	long	fractV;
	long	bumpH;
	long	bumpV;
} LinePathState;



static	short	LinePath ( short command, long data1, long data2, long * ref );
static	SegaErr ComputeAnimationStep ( LinePathState * state );
static	SegaErr	AnimateLineStep ( LinePathState * state, long time, Point * nxt );


static	short	LinePath ( short command, long data1, long data2, long * ref )
{
short			err;
LinePathState *	state;

	err = commandSelectorUnknown;
	
	switch ( command )
		{
		case kInitPathProc:
			state = NewMemory ( false, sizeof(LinePathState) );
			if ( state )
				{
				err = noErr;
				*ref = (long) state;
				
				state->startPoint.v = 0;
				state->startPoint.h = 0;
				state->stopPoint.v = 0;
				state->stopPoint.h = 0;

				state->startFrame = 0;
				state->stopFrame = 0;
				state->velocity = 0;
				}
			break;
		
		case kSetPathProcPoints:
			err = noErr;
			state = (LinePathState *) *ref;
			state->startPoint = *(Point *) &data1;
			state->stopPoint = *(Point *) &data2;
			
			ComputeAnimationStep ( state );
			break;
			
		case kSetPathProcFrames:
			err = noErr;
			state = (LinePathState *) *ref;
			state->startFrame = data1;
			state->stopFrame = data2;
			
			ComputeAnimationStep ( state );
			break;
			
		case kSetPathProcVelocity:
			err = noErr;
			state = (LinePathState *) *ref;
			state->velocity = data1;
			break;
			
		case kGetPathProcPoint:
			state = (LinePathState *) *ref;
			err = AnimateLineStep ( state, data1, (Point *) data2 );
			break;

		case kClosePathProc:
			err = noErr;
			state = (LinePathState *) *ref;
			DisposeMemory ( state );
			break;
		}
	
	return err;
}


static
SegaErr ComputeAnimationStep ( LinePathState * state )
{
long			diff;
long			dist;
	
	diff = state->stopFrame - state->startFrame;
	if ( diff > 0 )
		{
		dist = state->stopPoint.h - state->startPoint.h;
		dist <<= 16;
		state->bumpH = LongDivide ( dist, diff );
		
		dist = state->stopPoint.v - state->startPoint.v;
		dist <<= 16;
		state->bumpV = LongDivide ( dist, diff );
		}

	diff = state->startPoint.h;
	diff <<= 16;
	state->fractH = diff;
	
	diff = state->startPoint.v;
	diff <<= 16;
	state->fractV = diff;
	
	return noErr;
}


static
SegaErr	AnimateLineStep ( LinePathState * state, long time, Point * nxt )
{
long	timeDiff;
long	val;

	if ( time > state->stopFrame )
		{
		nxt->h = state->stopPoint.h;
		nxt->v = state->stopPoint.v;
		return noErr;
		}
		
	timeDiff = time - state->startFrame;
	if ( timeDiff > 0 )
		{
		state->startFrame = time;
		
		val = state->fractH + LongMultiply ( timeDiff, state->bumpH );
		state->fractH = val;
		val += 0x8000;
		val >>= 16;
		nxt->h = val;
		
		val = state->fractV + LongMultiply ( timeDiff, state->bumpV );
		state->fractV = val;
		val += 0x8000;
		val >>= 16;
		nxt->v = val;
		}
	else
		{
		nxt->h = state->startPoint.h;
		nxt->v = state->startPoint.v;
		}
	
	return noErr;
}


Ptr GetLinePathPtr ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) LinePath + 2));

	size = (*(long *) ((Ptr) GetLinePathPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Line Path proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) LinePath;
#else
		CheckCodeForJTRefs((void *)LinePath, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
