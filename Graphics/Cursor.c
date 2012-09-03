/*
	File:		Cursor.c

	Contains:	Cursor management code

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 8/21/94	KON		CreateSpriteInFront now takes a hueShift.
		<11>	 8/13/94	CMY		Always create cursors in front.
		<10>	 7/24/94	CMY		Add custom cluts to the sprite device decompressor.
		 <9>	 7/21/94	ADS		Remove temp use of OSManagersTEMP
		 <8>	 7/19/94	ADS		Vector Mania
		 <7>	 7/11/94	CMY		Added DrawSegaCursor, which immediately updates a cursor's
									hardware sprite.
		 <6>	  7/2/94	CMY		Fixed SetSegaCursorImage to add 128 to x and y.
		 <5>	 6/28/94	CMY		Updated sprite calls to not explicitly reference palette.
		 <4>	 5/31/94	SAH		Killed SegaMisc.h
		 <3>	 5/29/94	KON		Call GetDBGraphics to get data.
		 <2>	 5/28/94	SAH		Updated to allow multiple cursors.
		 <5>	 5/23/94	SAH		Call GetSegaGraphics to get the cursor BitMap.
		 <4>	 5/20/94	SAH		Added hotSpot. Fixed some cursor positioning bugs.
		 <3>	 5/19/94	SAH		Made it work more.
		 <2>	 5/18/94	SAH		Added LoadCursorFromVRAM.
		 <1>	 5/17/94	SAH		first checked in

	To Do:
*/



#include "SegaOS.h"
#include "Cursor.h"
#include "animation.h"
#include "Sprites.h"
#include "CursorPriv.h"
#include "Reliability.h"
#include "Heaps.h"
#include "Errors.h"
#include "Database.h"
#include "DBTypes.h"
#include "Errors.h"
#include "Time.h"
#include "GraphicsDB.h"


#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif



/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
CursorGlobals cursor;
#endif

CursorPtr	_CreateSegaCursor ( void );
short		_DisposeSegaCursor ( CursorPtr cursor );
short		_MoveSegaCursor ( CursorPtr cursor, short x, short y );
short		_HideSegaCursor ( CursorPtr cursor );
short		_ShowSegaCursor ( CursorPtr cursor );
short		_GetSegaCursorPos ( CursorPtr cursor, short *x, short *y );
short		_SetSegaCursorImage ( CursorPtr cursor, short id );
short		_LoadCursorFromVRAM ( CursorPtr cursor, segaCharRect * rect, unsigned short patBase, Point hotSpot );
void		_DrawSegaCursor ( CursorPtr cursor );


// local non-vectored
//
long	CursorAnimateProc ( short command, long * time, long data, long * ref );

// local vectored
//
static	short _LoadCursorPattern ( CursorPtr cursor, SegaBitMap * spriteData );

short LoadCursorPattern ( CursorPtr cursor, SegaBitMap * spriteData ) =
		CallDispatchedFunction( kLoadCursorPattern );


long
_CursorControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:			
			/* install our selectors */
			SetDispatchedFunction ( kCreateSegaCursor,		kCursorManager,	_CreateSegaCursor );
			SetDispatchedFunction ( kDisposeSegaCursor,		kCursorManager,	_DisposeSegaCursor );
			SetDispatchedFunction ( kMoveSegaCursor,		kCursorManager,	_MoveSegaCursor );
			SetDispatchedFunction ( kHideSegaCursor,		kCursorManager,	_HideSegaCursor );
			SetDispatchedFunction ( kShowSegaCursor,		kCursorManager,	_ShowSegaCursor );
			SetDispatchedFunction ( kGetSegaCursorPos,		kCursorManager,	_GetSegaCursorPos );
			SetDispatchedFunction ( kSetSegaCursorImage,	kCursorManager,	_SetSegaCursorImage );
			SetDispatchedFunction ( kLoadCursorFromVRAM,	kCursorManager,	_LoadCursorFromVRAM );
			SetDispatchedFunction ( kDrawSegaCursor,		kCursorManager,	_DrawSegaCursor );

			SetDispatchedFunction ( kLoadCursorPattern,		kCursorManager,	_LoadCursorPattern );
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
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}


/*
* allocate the cursor sprite
*/
CursorPtr
_CreateSegaCursor ( void )
{
CursorPtr	cursor;
	
	cursor = NewMemory ( kTemp, sizeof(Cursor) );
	if ( cursor )
		{
		cursor->xPos = 1;
		cursor->yPos = 0;
		cursor->id = 0;
		cursor->cursorVis = 0;
		cursor->blinkInterval = 1;
		cursor->active = false;
		cursor->hotSpot.h = 0;
		cursor->hotSpot.v = 0;
		cursor->cursorSprite = 0L;
		cursor->cursorAnimation = 0L;
		
		cursor->cursorAnimation = CreateAnimation ( CursorAnimateProc, (long) cursor );
		if ( !cursor->cursorAnimation )
			{
			DisposeMemory ( cursor );
			cursor = 0L;
			}
		}
	
	return cursor;
}


short
_DisposeSegaCursor ( CursorPtr cursor )
{	
	if ( cursor->cursorSprite )
		{
		DisposeSprite ( cursor->cursorSprite );
		}
	
	if ( cursor->cursorAnimation )
		{
		DisposeAnimation ( cursor->cursorAnimation );
		}
	
	DisposeMemory ( cursor );
	
	return noErr;
}


short
_MoveSegaCursor ( CursorPtr cursor, short x, short y )
{
short	xPos;
short	yPos;

	cursor->xPos = x;
	cursor->yPos = y;

	if ( cursor->cursorSprite && cursor->cursorVis >= 0 )
		{
		x -= cursor->hotSpot.h - 0x80;
		y -= cursor->hotSpot.v - 0x80;
		MoveSprite ( cursor->cursorSprite, x, y );
		}
		
	return noErr;
}

void
_DrawSegaCursor ( CursorPtr cursor )
{
	DrawSprite ( cursor->cursorSprite );
}

short
_HideSegaCursor ( CursorPtr cursor )
{
	if ( cursor->cursorSprite )
		{
		/* hide the cursor by moving the sprite off the screen */
		if ( cursor->cursorVis == 0 )
			{
			MoveSprite ( cursor->cursorSprite, 1, 0 );
			}
	
		cursor->cursorVis--;
		}
		
	return noErr;
}


short
_ShowSegaCursor ( CursorPtr cursor )
{
short	xPos;
short	yPos;

	if ( cursor->cursorSprite )
		{
		cursor->cursorVis++;
		
		if ( cursor->cursorVis >= 0 )
			{
			/* pin it to zero */
			cursor->cursorVis = 0;
			
			/* as the cursor is now visible, move it back to the screen */
			xPos = cursor->xPos - cursor->hotSpot.h + 0x80;
			yPos = cursor->yPos - cursor->hotSpot.v + 0x80;
			MoveSprite ( cursor->cursorSprite, xPos, yPos );
			}
		}
	
	return noErr;
}


short
_GetSegaCursorPos ( CursorPtr cursor, short *x, short *y )
{
	*x = cursor->xPos;
	*y = cursor->yPos;
	
	return noErr;
}


short
_SetSegaCursorImage ( CursorPtr cursor, short id )
{
CursorData * 	data;
SegaBitMap *	spriteData;
SegaErr			err;

	err = -1;
	
	/* load the apropriate graphic and use it for the sprite */
	data = (CursorData *) DBGetItem ( kCursorType, id );
	ASSERT_MESG( data, "Can't load cursor data" );
	if ( data )
		{
		err = noErr;
		cursor->blinkInterval = data->blinkInterval;
		cursor->hotSpot = data->hotSpot;
		
		spriteData = GetDBGraphics ( data->spriteDataId );
		ASSERT_MESG( spriteData, "Can't load cursor sprite data" );
		if ( !spriteData )
			{
			data = 0L;
			err = cantCreateCursorErr;
			}
			
		if ( err == noErr )
			{
			err = LoadCursorPattern ( cursor, spriteData );
			}
		}

	return err;
}


short
_LoadCursorFromVRAM ( CursorPtr cursor, segaCharRect * rect, unsigned short patBase, Point hotSpot )
{
SegaBitMap *	spriteData;
SegaErr			err;

	err = -1;

	/* someone else does this */
	cursor->blinkInterval = 30;
	
	cursor->hotSpot = hotSpot;
	
	spriteData = CreateSpriteData ( rect, patBase, nil );
	ASSERT_MESG( spriteData, "Can't create sprite data" );
	if ( spriteData )
		{
		err =  LoadCursorPattern ( cursor, spriteData );
		}
	
	return err;
}


static
short _LoadCursorPattern ( CursorPtr cursor, SegaBitMap * spriteData )
{
short	xPos;
short	yPos;

	if ( cursor->active )
		{
		StopAnimation ( cursor->cursorAnimation );
		cursor->active = false;
		}
			
	if ( cursor->cursorSprite )
		{
		DisposeSprite ( cursor->cursorSprite );
		}
			
	cursor->cursorSprite = CreateSpriteInFront ( spriteData, 0 );
	ASSERT_MESG( cursor->cursorSprite, "Can't create cursor sprite" );
	if ( !cursor->cursorSprite )
		{
		return cantCreateCursorErr;
		}

	cursor->active = true;

		if ( cursor->cursorVis >= 0 )
			{
			xPos = cursor->xPos - cursor->hotSpot.h + 0x80;
			yPos = cursor->yPos - cursor->hotSpot.v + 0x80;
			MoveSprite ( cursor->cursorSprite, xPos, yPos );
			}
	
	DrawSprite ( cursor->cursorSprite );
	StartAnimation ( cursor->cursorAnimation );
	
	return noErr;
}


long
CursorAnimateProc ( short command, long * time, long data, long * ref )
{
long			returnVal;
Point			p;
CursorPtr 		cursor;

	returnVal = kAnimationOk;
		
	switch ( command )
		{
		case kInitAnimationProc:
			*ref = data;
			break;
		
		case kDisposeAnimationProc:
			break;
		
		case kStartAnimationProc:
			break;
		
		case kStopAnimationProc:
			break;
		
		case kDoAnimationEvent:
			cursor = (CursorPtr) *ref;
			IncrementSpriteFrame ( cursor->cursorSprite, 1 );
			DrawSprite ( cursor->cursorSprite );
			*time += cursor->blinkInterval;
			break;
		}
	
	return returnVal;
}


