/*
	File:		progress.c

	Contains:	Icon-Motion progress indicator

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 8/21/94	KON		CreateSpriteInFront now takes a hueShift.
		 <5>	 7/10/94	ADS		Added animation capability, when using multi-frame sprites
		 <4>	 7/10/94	ADS		Sprites frontmost.  Use screen coords, not sprite HW
		 <3>	  7/9/94	JBH		Fix rectangle struct to make Andy happy, make simulator not
									crash.
		 <1>	  7/8/94	JBH		first checked in

	To Do:
*/


/* if we're building the ROM database, do a real sega build */
#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "SegaOS.h"
#include "Progress.h"
#include "ProgressPriv.h"
#include "Errors.h"
#include "Sprites.h"
#include "Heaps.h"
#include "Reliability.h"
#include "MungeCode.h"
#include "DBTypes.h"
#include "Time.h"


#define kSpriteOffsetX	(128)
#define kSpriteOffsetY	(128)

/*
*
* This is meant to be a simple "progress proc" used to tell the user how far in an operation
* they have waited, and how much is left (ie when loading news).
*/

typedef struct SimpleProgressState {
	Rect 		bounds;
	short		pixelWidth, pixelHeight;
	short		lastX, lastY;
	short		totalTime;
	short		frameCount, lastFrame;
	long		nextFrameTime, frameTime;
	SpriteRef	sprite;
} SimpleProgressState;


static	long	SimpleProgressProc ( short command, ProgressRec *progData, long moreData, long * ref );

static
long SimpleProgressProc ( short command, ProgressRec *progData, long moreData, long * ref )
{
long				returnVal;
SimpleProgressState	*state = (SimpleProgressState *) *ref;


	returnVal = kProgressOK;
	
	switch ( command )
		{
		case kInitProgressProcCmd:
			state = NewMemory ( kTemp, sizeof(SimpleProgressState) );
			*ref = (long) state;
			
			if ( state )
				{
				SegaBitMap	*animator;
				DBID		bitmapNum = moreData & 0xFF;
				short		frameTime = ((unsigned long) moreData >> 8) & 0xFF;
				
				animator = DBGetItem( kBitMapType, bitmapNum );
				
				ASSERT_MESG( animator, "couldn't load bitmap for progress");
				if (!animator) {
					DisposeMemory(state);
					returnVal = kProgressInitError;
					*ref = 0;
				} else {
					state->sprite = CreateSpriteInFront( animator, 0 );

					ASSERT_MESG(state->sprite, "couldn't create a sprite for progress");

					if (state->sprite) {
						state->bounds = progData->boundsRect;
						state->totalTime = progData->totalTime;
						ASSERT_MESG(state->totalTime, "0 total time will be a divide by zero soon.");
						
						state->pixelWidth = progData->boundsRect.right - progData->boundsRect.left;
						state->pixelHeight = progData->boundsRect.bottom - progData->boundsRect.top;
						state->lastX = progData->boundsRect.left;
						state->lastY = progData->boundsRect.top;
						
						state->frameCount = animator->frames;		// are there animation frames?
						state->lastFrame = 0;
						if (state->frameCount > 1)	{
							state->frameTime = frameTime;
							if (frameTime > 0)				// frameTime != 0 means auto-cycle
								state->nextFrameTime = GetCurrentTime() + frameTime;
						}
						
						MoveSprite(state->sprite, state->lastX + kSpriteOffsetX, state->lastY + kSpriteOffsetY);
						DrawSprite(state->sprite);
					} else {
						DisposeMemory(state);
						returnVal = kProgressInitError;
						*ref = 0;
					}
				}
				
				}
			else
				{
				returnVal = kProgressInitError;
				}
			break;
		
		case kDisposeProgressProcCmd:
			if ( state )
				{
				DisposeSprite( state->sprite );
				DisposeMemory ( state );
				}
			
			*ref = 0L;
			break;
		
		case kSetProgressPositionCmd:
			if ( state )
				{
					short	thisX, thisY;
					short	thisTime = moreData;
					short	pixelWidth = state->pixelWidth;
					short	pixelHeight = state->pixelHeight;
					short	totalTime = state->totalTime;
					short	thisFrame = state->lastFrame;
					short	totalFrames = state->frameCount;
					
					if (thisTime < 0)		// pin time so we don't overflow
						thisTime = 0;
					if (thisTime >= totalTime)
						thisTime = totalTime - 1;
					
					/* this slimy hack is to avoid using THINK library code to do */
					/* long divides. Basically, I want to multiply 2 16-bit numbers, and */
					/* divide the 32-bit result by a 16-bit number. */
					
#if 0		// THINK C SUCKS !!
					thisX = state->bounds.left + (thisTime * state->pixelWidth) / state->totalTime;
#else
					asm {
						MOVE.W	thisTime, D0
						MULS.W	pixelWidth, D0
						DIVS.W	totalTime, D0
						MOVE.W	D0, thisX
					}
					thisX += state->bounds.left;
#endif
#if 0		// THINK C SUCKS !!
					thisY = state->bounds.top + (thisTime * state->pixelHeight) / state->totalTime;
#else
					asm {
						MOVE.W	thisTime, D0
						MULS.W	pixelHeight, D0
						DIVS.W	totalTime, D0
						MOVE.W	D0, thisY
					}
					thisY += state->bounds.top;
#endif
					
					if (state->frameCount > 1 && ! state->frameTime)	{
#if 0
						thisFrame = (thisTime * state->frameCount) / state->totalTime;
#else
						asm {
							MOVE.W	thisTime, D0
							MULS.W	totalFrames, D0
							DIVS.W	totalTime, D0
							MOVE.W	D0, thisFrame
						}
#endif
						if (thisFrame < 0)		// careful we don't EVER overflow here
							thisFrame = 0;
						if (thisFrame >= totalFrames)
							thisFrame = totalFrames - 1;
					}
					
					
					
					if (thisX != state->lastX || thisY != state->lastY || thisFrame != state->lastFrame) {
						state->lastX = thisX;
						state->lastY = thisY;
						state->lastFrame = thisFrame;
						MoveSprite( state->sprite, thisX + kSpriteOffsetX, thisY + kSpriteOffsetY );
						SetSpriteFrame( state->sprite, thisFrame);
						DrawSprite( state->sprite );
					}
				}
			break;
		
		case kIdleProgressCmd:
			if ( state )
				{
					if (state->frameCount > 1 && state->frameTime)	{		// auto cycle IF multi frames
						long	curTime = GetCurrentTime();					// and IF cycle-time given
						long	triggerTime = state->nextFrameTime;
						long	inc = 0;
						
						while (curTime > triggerTime)	{					// might inc multiple frames if
							triggerTime += state->frameTime;				// we haven't been here in a while
							++inc;											//   (so we don't slow down)
						}
						if (inc)	{
							IncrementSpriteFrame ( state->sprite, inc );
							DrawSprite( state->sprite );

							state->nextFrameTime = triggerTime;				// setup for next
							GetSpriteFrame( state->sprite, &state->lastFrame);
						}
					}
				}
			break;
		
		}
	
	return returnVal;
}


Ptr
GetSimpleProgressProc ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) SimpleProgressProc + 2));

	size = (*(long *) ((Ptr) GetSimpleProgressProc + 2)) - srcFunction;
	data = NewMemory ( kTemp,  size );
	ASSERT_MESG( data, "Cannot allocate memory for simple progress proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) SimpleProgressProc;
#else
		CheckCodeForJTRefs((void *)SimpleProgressProc, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
