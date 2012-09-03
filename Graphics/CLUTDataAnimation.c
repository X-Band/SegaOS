/*
	File:		CLUTDataAnimation.c

	Contains:	animation proc that animates a color table based on data (CDAT resource)

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/26/94	CMY		Updated to pass VBL flag to SetupClut.
		 <1>	 7/15/94	JBH		first checked in. A fine data-driven CLUT animator.
	To Do:
*/

/* if we're building the ROM database, do a real sega build */
#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "DBTypes.h"
#include "SegaOS.h"
#include "animation.h"
#include "Errors.h"
#include "Animators.h"
#include "Sprites.h"
#include "Heaps.h"
#include "Reliability.h"
#include "MungeCode.h"
#include "SegaScrn.h"

typedef struct CLUTDataFrame {
	short	frames;					// 0 means jump to frame data[0]
	short	data[16];				// else the data is a color table
} CLUTDataVector;

typedef struct CLUTDataState {
	short			clutNumber;
	short			clutData[16];
	short			framesLeft;
	short			vectorIndex;
	CLUTDataVector	*vectors;
	long			frameDelay;
} CLUTDataState;


static	long	CLUTDataAnimatorProc ( short command, long * time, CLUTDataAnimatorData * data, long * ref );

static void DoCLUTAnimation( CLUTDataState *state );

static
long CLUTDataAnimatorProc ( short command, long * time, CLUTDataAnimatorData * data, long * ref )
{
long			returnVal;
short i;
CLUTDataState	*privData = (CLUTDataState *) *ref;

	returnVal = kAnimationOk;
	
		
	switch ( command )
		{
		case kInitAnimationProc:
			privData = (CLUTDataState *) NewMemory( kTemp, sizeof(CLUTDataState) );
			if (privData) {

				privData->vectors = (CLUTDataVector *) DBGetItem( kCLUTDataType, data->dataID );
				
				ASSERT_MESG(privData->vectors, "didn't receive data to animate CLUT");
			
				if ( privData->vectors ) {
					privData->clutNumber = data->clutNumber;
					privData->vectorIndex = 0;
					privData->framesLeft = 0;
					privData->frameDelay = data->frameDelay;
				
					*ref = (long) privData;
				} else {
					DisposeMemory( (Ptr) privData );
					*ref = 0;
					returnVal = kAnimationError;
				}
			} else {
				*ref = 0;
				returnVal = kAnimationError;
			}
			break;
		
		case kDisposeAnimationProc:
			if ( privData )
				{
				DisposeMemory ( (Ptr) privData );
				}
			*ref = 0L;
			break;
		
		case kStartAnimationProc:
			break;
		
		case kStopAnimationProc:
			break;
			
		case kSetAnimationGrayProc:
			break;
			
		case kClearAnimationGrayProc:
			break;
		
		case kDoAnimationEvent:
			if ( privData )
				{
				DoCLUTAnimation( privData );
				*time += privData->frameDelay;	
				}
				
			break;
		}
	
	return returnVal;
}

static void DoCLUTAnimation( CLUTDataState *state )
{
	short i;

	if ( state->framesLeft < 1 ) {
		short vectorIndex;
		CLUTDataVector *vectorPtr;

		/* time to process a frame */
		vectorIndex = state->vectorIndex++;
		vectorPtr = &state->vectors[vectorIndex];
		
		if (vectorPtr->frames == 0) {
			/* this is a "jump" command */
			vectorIndex = vectorPtr->data[0];
			state->vectorIndex = vectorIndex;
			vectorPtr = &state->vectors[vectorIndex];
			
			ASSERT_MESG(vectorPtr->frames, "CLUT data jump frame points to a jump frame!");
					
		}
		
		/* handle a new frame */
		state->framesLeft = vectorPtr->frames;		
		
		/* for now, this is a tad wasteful. But if we add relative animations, the copy will */
		/* be useful */
		for (i=0;i<16;i++) {
			state->clutData[i] = vectorPtr->data[i];
		}
		
		SetupClut( state->clutNumber, state->clutData, true );

	}
	
	state->framesLeft--;
}

Ptr
GetCLUTDataAnimatorProc ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) CLUTDataAnimatorProc + 2));

	size = (*(long *) ((Ptr) GetCLUTDataAnimatorProc + 2)) - srcFunction;
	data = NewMemory ( kTemp,  size );
	ASSERT_MESG( data, "Cannot allocate memory for table animator proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) CLUTDataAnimatorProc;
#else
		CheckCodeForJTRefs((void *)CLUTDataAnimatorProc, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
