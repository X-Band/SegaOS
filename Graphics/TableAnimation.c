/*
	File:		TableAnimation.c

	Contains:	animation proc that just bumps a color table's entries after a fixed delay

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/26/94	CMY		Updated to pass VBL flag to SetupClut.
		 <1>	 7/15/94	JBH		first checked in. A lovely animation proc to do CLUT fades.
	To Do:
*/

/* if we're building the ROM database, do a real sega build */
#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "SegaOS.h"
#include "animation.h"
#include "Errors.h"
#include "Animators.h"
#include "Sprites.h"
#include "Heaps.h"
#include "Reliability.h"
#include "MungeCode.h"
#include "SegaScrn.h"

typedef
struct ClutAnimationPrivData		// internal data for TableAnimation.
{
	short	clutNumber;
	short	beginningValues[16];
	short	endingValues[16];
	short	totalSteps;
	short	stepsSoFar;
	short	stepTime;
} ClutAnimationPrivData;

static	long	TableAnimateProc ( short command, long * time, ClutAnimationData * data, long * ref );


static
long TableAnimateProc ( short command, long * time, ClutAnimationData * data, long * ref )
{
long			returnVal;
ClutAnimationPrivData *privData = (ClutAnimationPrivData *) *ref;
short i;

	returnVal = kAnimationOk;
	
		
	switch ( command )
		{
		case kInitAnimationProc:
			privData = (ClutAnimationPrivData *) NewMemory( kTemp, sizeof(ClutAnimationPrivData) );
			if (privData) {
				privData->clutNumber = data->clutNumber;
				for (i=0; i<16; i++) {
					privData->beginningValues[i] = data->beginningValues[i];
					privData->endingValues[i] = data->endingValues[i];
					privData->totalSteps = data->steps;
					privData->stepsSoFar = 0;
					privData->stepTime = data->stepTime;
				}
			
				SetupClut( data->clutNumber, privData->beginningValues, true );
			
				*ref = (long) privData;
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
			if ( privData && privData->stepsSoFar < privData->totalSteps)
				{
				short newClut[16];
				short stepsSoFar;
				short totalSteps;
				short stepsLeft;
				short i;
				short startValue, endValue;
				short startPart, endPart;
				short color;
				short mergedColor;
				
				privData->stepsSoFar++;				
				stepsSoFar = privData->stepsSoFar;
				totalSteps = privData->totalSteps;
				stepsLeft = totalSteps - stepsSoFar;
				
				for (i=0;i<16;i++) {
					startValue = privData->beginningValues[i];
					endValue = privData->endingValues[i];

					startPart = (startValue & 0xe00) >> 9;
					endPart = (endValue & 0xe00) >> 9;
					startPart = (startPart * stepsLeft) + (endPart * stepsSoFar);
					startPart /= totalSteps;
					color = startPart << 9;
					color &= 0xe00;
					
					mergedColor = color;
					
					startPart = (startValue & 0x0e0) >> 5;
					endPart = (endValue & 0x0e0) >> 5;
					startPart = (startPart * stepsLeft) + (endPart * stepsSoFar);
					startPart /= totalSteps;
					color = startPart << 5;
					color &= 0x0e0;
					
					mergedColor |= color;
					
					startPart = (startValue & 0x00e) >> 1;
					endPart = (endValue & 0x00e) >> 1;
					startPart = (startPart * stepsLeft) + (endPart * stepsSoFar);
					startPart /= totalSteps;
					color = startPart << 1;
					color &= 0x00e;
					
					mergedColor |= color;
					
					newClut[i] = mergedColor;
				}
				
				SetupClut( privData->clutNumber, newClut, true );
				
				*time += privData->stepTime;
				}
				
			break;
		}
	
	return returnVal;
}


Ptr
GetTableAnimatorProc ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) TableAnimateProc + 2));

	size = (*(long *) ((Ptr) GetTableAnimatorProc + 2)) - srcFunction;
	data = NewMemory ( kTemp,  size );
	ASSERT_MESG( data, "Cannot allocate memory for table animator proc" );
	