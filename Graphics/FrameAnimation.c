/*
	File:		FrameAnimation.c

	Contains:	animation proc that just bumps a sprite to the next frame after a fixed delay

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		 <7>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <6>	 6/29/94	CMY		Added support for grayscale animations and enabled Think C
									debugging in stimulator.
		 <5>	 6/28/94	CMY		Updated sprite calls to use dynamic palette allocation.
		 <4>	 5/28/94	SAH		Added the ROMDB conditional for the rom database build.
		 <3>	 5/28/94	CMY		Added calls to CheckCodeForJTRefs when adding procs to DB.
		 <2>	 5/27/94	SAH		Copy the proc in for real.
		 <3>	 5/25/94	HEC		new memory manager
		 <2>	 5/23/94	SAH		Don't draw the sprite on StartAnimation.
		 <1>	 5/21/94	SAH		first checked in

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


static	long	FrameAnimateProc ( short command, long * time, FrameDelayData * data, long * ref );


static
long FrameAnimateProc ( short command, long * time, FrameDelayData * data, long * ref )
{
long			returnVal;
Point			p;
SpriteRef		sprite;

	returnVal = kAnimationOk;
	
	sprite = (SpriteRef) *ref;
		
	switch ( command )
		{
		case kInitAnimationProc:
			sprite = CreateSprite ( data->bitmap );
			if ( sprite )
				{
				MoveSprite ( sprite, data->xPos + 0x80, data->yPos + 0x80 );
				}
			
			*ref = (long) sprite;
			break;
		
		case kDisposeAnimationProc:
			if ( sprite )
				{
				DisposeSprite ( sprite );
				}
			*ref = 0L;
			break;
		
		case kStartAnimationProc:
			break;
		
		case kStopAnimationProc:
			break;
			
		case kSetAnimationGrayProc:
			SetSpriteGrayFlag(sprite, true);
			DrawSprite(sprite);
			break;
			
		case kClearAnimationGrayProc:
			SetSpriteGrayFlag(sprite, false);
			DrawSprite(sprite);
			break;
		
		case kDoAnimationEvent:
			if ( sprite )
				{
				IncrementSpriteFrame ( sprite, 1 );
				DrawSprite ( sprite );
				}
				
			*time += data->frameDelay;
			break;
		}
	
	return returnVal;
}


Ptr
GetFrameAnimatorProc ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) FrameAnimateProc + 2));

	size = (*(long *) ((Ptr) GetFrameAnimatorProc + 2)) - srcFunction;
	data = NewMemory ( kTemp,  size );
	ASSERT_MESG( data, "Cannot allocate memory for frame animator proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) FrameAnimateProc;
#else
		CheckCodeForJTRefs((void *)FrameAnimateProc, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
