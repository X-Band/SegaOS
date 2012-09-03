/*
	File:		ButtonAnimator.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 7/30/94	CMY		Cache the graphics dictionary for the animating button.
		 <2>	 7/27/94	CMY		Create them sonsabitches high in pattern mems!
		 <1>	 7/26/94	CMY		first checked in

	To Do:
*/



#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "SegaOS.h"
#include "animation.h"
#include "Errors.h"
#include "Animators.h"
#include "Sprites.h"
#include "Heaps.h"
#include "MungeCode.h"
#include "Paths.h"
#include "DBTypes.h"
#include "Time.h"
#include "GraphicsDB.h"
#include "Decompress.h"



typedef struct ButtonAnimatorState
{
	SpriteRef	sprite;				// The button's sprite
	PathRef		path;				// The path to slide in along
	long		slideDoneTime;		// When the sliding is done
	short		flipUpFrame;
	void		*decompressCache;	// Trade mems for speed
} ButtonAnimatorState;


static long TheAnimateProc ( short command, long *time, ButtonAnimatorData *data, long *ref );



static long ButtonAnimateProc ( short command, long *time, ButtonAnimatorData *data, long *ref )
{
long				returnVal;
Point				p;
ButtonAnimatorState *state;
SegaBitMapPtr		bitmap;

	returnVal = kAnimationOk;
	
	state = (ButtonAnimatorState *) *ref;
	
	switch ( command )
	{
		case kInitAnimationProc:
			state = NewMemory ( kTemp, sizeof(ButtonAnimatorState) );
			*ref = (long) state;
			
			if ( state )
			{
				Decompressor dec;
				
				state->sprite = 0L;
				state->path = 0L;
				state->flipUpFrame = 0;
				
				/* Make a sprite for the button */
				bitmap = GetDBButtonFrame( data->buttonID, kFlatFrame );
				dec = CreateDecompressor( bitmap, 0, 0 );
				state->decompressCache = GetDecompressorCache( dec );
				DisposeDecompressor( dec );
				state->sprite = CreateSpriteHigh( bitmap );
				if ( !state->sprite )
				{
					return kAnimationError;
				}

				/* create the animation Vector path */
				state->path = CreatePath ( 3, (Ptr)((data->frameDelay<<8) | data->pathID) );
				if ( state->path )
				{
					SetPathPoints ( state->path, data->startPoint, data->stopPoint );
				}
					
				MoveSprite ( state->sprite, data->startPoint.h, data->startPoint.v );
				DrawSprite( state->sprite );
				
				state->slideDoneTime = GetCurrentTime() + data->slideTime;
			}
			break;
		
		case kDisposeAnimationProc:
			if ( state )
			{
				if ( state->decompressCache )
				{
					Decompressor dec = CreateDecompressor(
						GetDBButtonFrame( data->buttonID, kFlatFrame ), 0, 0);
						
					ReleaseDecompressorCache( dec, state->decompressCache );
					DisposeDecompressor( dec );
				}
				if ( state->sprite )
				{
					DisposeSprite( state->sprite );
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
			if ( state )
			{
				long curTime = *time;
				
				if (curTime < state->slideDoneTime)
				{
					GetPathPoint ( state->path, *time, &p );
					MoveSprite ( state->sprite, p.h, p.v );
					DrawSprite ( state->sprite );
				}
				else if ( state->flipUpFrame == 0 )
				{
					++state->flipUpFrame;
					MoveSprite( state->sprite, data->stopPoint.h, data->stopPoint.v );
					DrawSprite( state->sprite );
				}
				else if ( state->flipUpFrame <= kAnimationFrames )
				{
					SetSpriteImage( state->sprite, GetDBButtonFrame( data->buttonID, state->flipUpFrame ));
					DrawSprite( state->sprite );
					++state->flipUpFrame;
				}
				else
				{
					if ( state->decompressCache )
					{
						Decompressor dec = CreateDecompressor(
							GetDBButtonFrame( data->button