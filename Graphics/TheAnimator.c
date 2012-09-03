/*
	File:		TheAnimator.c

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 7/24/94	JBH		It can now get its graphics directly from animatorData, as well
									as the SEGA DBs and button sliders. I did this for the sparks.
		 <5>	 7/19/94	CMY		It can now get its graphics from buttonGraphics as well as SEGA
									DB items
		 <4>	 7/10/94	KON		Pass pathData through to CreatePath.
		 <3>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <2>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <1>	  7/6/94	SAH		first checked in

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
#include "Paths.h"
#include "DBTypes.h"
#include "Time.h"



/*
*
*	This proc is an attempt to create a fairly general purpose animation proc. It 
*	takes a bitmap DBID to animate as well as a path proc DBID (along with start and
*	end points). This should allow us to do many different animations).
*/


typedef
struct TheAnimatorState
{
	SpriteRef	sprite;
	PathRef		path;
	long		nextBMFrame;		// when we want our next one
	long		endFrame;
	Boolean		spriteIsOwnedByUs;
} TheAnimatorState;


static	long	TheAnimateProc ( short command, long * time, TheAnimatorData * data, long * ref );


static
long TheAnimateProc ( short command, long * time, TheAnimatorData * data, long * ref )
{
long				returnVal;
Point				p;
TheAnimatorState * 	state;
SegaBitMapPtr		bitmap;
long				nextTime;

	returnVal = kAnimationOk;
	
	state = (TheAnimatorState *) *ref;
	
	switch ( command )
		{
		case kInitAnimationProc:
			state = NewMemory ( kTemp, sizeof(TheAnimatorState) );
			*ref = (long) state;
			
			if ( state )
				{
				state->sprite = 0L;
				state->path = 0L;
				
				/* create the animation graphic */
				if (data->animatorData == -1)
				{
					bitmap = DBGetItem ( kButtonGraphicType, data->bitmap );
					if ( !bitmap )
						{
						return kAnimationError;
						}
				}
				else if (data->animatorData == 0)
				{
					bitmap = DBGetItem ( kBitMapType, data->bitmap );
					if ( !bitmap )
						{
						return kAnimationError;
						}
				}
				
				if (data->animatorData == 0 || data->animatorData == -1) {
					state->spriteIsOwnedByUs = true;
					state->sprite = CreateSprite ( bitmap );
					if ( !state->sprite )
						{
						return kAnimationError;
						}
				} else {
					state->spriteIsOwnedByUs = false;
					state->sprite = (SpriteRef) data->animatorData;
				}
				
				/* create the animation path */
				/* no path just means that we run through frames at a stationary spot */
				state->path = CreatePath ( data->path, (Ptr)data->pathData );
				if ( state->path )
					{
					SetPathPoints ( state->path, data->startPoint, data->stopPoint );
					}
					
				MoveSprite ( state->sprite, data->startPoint.h, data->startPoint.v );
				}
			break;
		
		case kDisposeAnimationProc:
			if ( state )
				{
				if ( state->sprite && state->spriteIsOwnedByUs )
					{
					DisposeSprite ( state->sprite );
					}
					
				if ( state->path )
					{
					DisposePath ( state->path );
					}
				
				DisposeMemory ( state );
				}
			
			*ref = 0L;
			break;
		
		case kStartAnimationProc:
			if ( state )
				{
				nextTime = GetCurrentTime();
				state->endFrame = nextTime + data->animationLenth;
				
				if ( state->path )
					{
					SetPathFrames ( state->path, nextTime, state->endFrame );
					}
				}
			break;
		
		case kStopAnimationProc:
			break;
			
		case kSetAnimationGrayProc:
			if ( state->sprite )
				{
				SetSpriteGrayFlag( state->sprite, true );
				DrawSprite( state->sprite );
				}
			break;
			
		case kClearAnimationGrayProc:
			if ( state->sprite )
				{
				SetSpriteGrayFlag( state->sprite, false );
				DrawSprite( state->sprite );
				}
			break;
		
		case kDoAnimationEvent:
			/* assume we almost never want to be called */
			nextTime = 0x7ffffff;

			if ( state )
				{
 				/* get the new point if we have a path */
				if ( state->path )
					{
					GetPathPoint ( state->path, *time, &p );
					MoveSprite ( state->sprite, p.h, p.v );
					nextTime = data->pathFrameDelay;
					}
					
				if ( state->sprite && *time >= state->nextBMFrame )
					{
					IncrementSpriteFrame ( state->sprite, 1 );
					DrawSprite ( state->sprite );
					}
				
				/* if the frame animator wants to be called before the path, let it */
				if ( data->bmFrameDelay < nextTime )
					{
					nextTime = data->bmFrameDelay;
					}
				}
				
			/* we want to be called for the earlier path or frame step */
			nextTime += GetCurrentTime();
			state->nextBMFrame = nextTime;
			*time = nextTime;
			break;
		}
	
	return returnVal;
}


Ptr
GetTheAnimatorProc ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) TheAnimateProc + 2));

	size = (*(long *) ((Ptr) GetTheAnimatorProc + 2)) - srcFunction;
	data = NewMemory ( kTemp,  size );
	ASSERT_MESG( data, "Cannot allocate memory for frame animator proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) TheAnimateProc;
#else
		CheckCodeForJTRefs((void *)TheAnimateProc, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
