/*
	File:		VectorPath.c

	Contains:	Path proc that takes a vector DBID.

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 8/21/94	KON		Add speed control to vector path.
		 <6>	 8/20/94	KON		Fix initialization values so sparks line up right.
		 <5>	 7/18/94	JOE		shift frequency down to low byte
		 <4>	 7/17/94	JBH		After a jump frame, process next frame before continuing
									(instead of just bumping index).
		 <3>	 7/15/94	JBH		Bump vectorIndex after a "jump" frame.
		 <2>	 7/12/94	KON		Add sound fx opcode.
		 <1>	 7/10/94	KON		first checked in

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
#include "DBTypes.h"
#include "Time.h"
#include "SegaSound.h"

#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif

//
// Masks to convert data long to parameters
//
#define kVectorDBIDMask	0x000000FF
#define kVectorSpeedMask 0x0000FF00

#define kVectorEnd -1
#define kVectorNewIndex -2
#define kVectorAbsolute -3
#define kPlaySoundFX	-4
#define kSetPathSpeed	-5

#define kDBSoundIDMask 0x00FF
#define kFrequencyMask 0xFF00
#define kFrequencyShift	8

typedef struct VectorList
{
	short	count;
	short	newX;
	short	newY;
} VectorList;

typedef struct VectorPathState
{
	Point	curPoint;
	short	vectorIndex;
	short	curCount;
	VectorList	*vectors;
	short	curXVector;
	short	curYVector;
	short	speed;
	long	targetTime;
} VectorPathState;


static	short	VectorPath ( short command, long data1, long data2, long * ref );
static	SegaErr	AnimateVectorStep ( VectorPathState * state, long time, Point * nxt );


static short VectorPath( short command, long data1, long data2, long * ref )
{
short				err;
VectorPathState *	state;

	err = commandSelectorUnknown;
	
	switch ( command )
		{
		case kInitPathProc:
			state = NewMemory ( false, sizeof(VectorPathState) );
			if ( state )
			{
				err = noErr;
				*ref = (long) state;
				
				state->curPoint.v = 0;
				state->curPoint.h = 0;
				state->vectorIndex = 0;
				state->curCount = -1;
				state->targetTime = 0;

				state->curXVector = 0;
				state->curYVector = 0;
				state->speed = (data1 & 0x0000FF00) >> 8;
				state->vectors = (VectorList *)DBGetItem( kVectorType, (data1 & kVectorDBIDMask) );
				ASSERT( state->vectors );
			}
			break;
		
		case kSetPathProcPoints:
			err = noErr;
			state = (VectorPathState *) *ref;
			state->curPoint = *(Point *) &data1;
			break;
			
		case kSetPathProcFrames:
			err = noErr;
			state = (VectorPathState *) *ref;
			break;
			
		case kSetPathProcVelocity:
			err = noErr;
   			break;
			
		case kGetPathProcPoint:
			state = (VectorPathState *) *ref;
			err = AnimateVectorStep ( state, data1, (Point *) data2 );
			break;

		case kClosePathProc:
			err = noErr;
			state = (VectorPathState *) *ref;
			DisposeMemory ( state );
			break;
		}
	
	return err;
}

static
SegaErr	AnimateVectorStep ( VectorPathState * state, long time, Point * nxt )
{

	if ( time >= state->targetTime && state->curCount >= 0 )
	{
	//
	// time to do a frame...update for the next point
	//
		state->curCount--;
		state->curPoint.h += state->curXVector;
		state->curPoint.v += state->curYVector;
		state->targetTime = GetCurrentTime() + state->speed;
	}
	
	*nxt = state->curPoint;


//
// Check if we need to update the state from the next vector
//	
	if( state->curCount < 0 )
	{
	//
	// Walk vector list and suck down next data
	//
	
	getNextData:
	
		switch( state->vectors[state->vectorIndex].count )
		{
			case kVectorEnd:
				state->targetTime = kInfiniteTime;
				break;
				
			case kVectorAbsolute:
				state->curPoint.h = state->vectors[state->vectorIndex].newX;
				state->curPoint.v = state->vectors[state->vectorIndex].newY;
				state->curCount = -1;	//fire again immediately
				state->targetTime = 0;
				state->vectorIndex++;
				break;
	
			case kVectorNewIndex:
				state->vectorIndex = state->vectors[state->vectorIndex].newX;	//wacky overloading!!!
				
				goto getNextData;		/* Evil goto - but we will process next data */

			case kPlaySoundFX:
				PlayDBFX( state->vectors[state->vectorIndex].newX & kDBSoundIDMask,
							state->vectors[state->vectorIndex].newY,
							(state->vectors[state->vectorIndex].newX & kFrequencyMask) >> kFrequencyShift );
				state->curCount = -1;	//fire again immediately
				state->targetTime = 0;
				state->vectorIndex++;
				break;

			case kSetPathSpeed:
				state->speed = state->vectors[state->vectorIndex].newX;			
				state->curCount = -1;	//fire again immediately
				state->targetTime = 0;
				state->vectorIndex++;
				goto getNextData;		/* Evil goto - but we will process next data */
	
			default:
				state->curXVector = state->vectors[state->vectorIndex].newX;
				state->curYVector = state->vectors[state->vectorIndex].newY;
				state->curCount = state->vectors[state->vectorIndex].count;
				state->targetTime = 0;
				state->vectorIndex++;
				break;
		
		}
	}
		
	return noErr;
}


Ptr GetVectorPathPtr ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) VectorPath + 2));

	size = (*(long *) ((Ptr) GetVectorPathPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Line Path proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) VectorPath;
#else
		CheckCodeForJTRefs((void *)VectorPath, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
