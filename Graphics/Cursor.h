/*
	File:		Cursor.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 7/15/94	dwh		unix-ise.
		 <4>	 7/11/94	CMY		Added DrawSegaCursor
		 <3>	  7/2/94	CMY		Added enumerations for the DB cursors.
		 <2>	 5/28/94	SAH		Updated to allow multiple cursors.
		 <3>	 5/20/94	SAH		Added hotspot
		 <2>	 5/18/94	SAH		Added LoadCursorFromVRAM.
		 <1>	 5/17/94	SAH		first checked in

	To Do:
*/

#ifndef __Cursor__
#define __Cursor__

#ifndef __Sega__
#include "Sega.h"
#endif

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

/* Cursors in the database */
enum {
	kCursor0Cursor,
	kCursor1Cursor,
	kCursor2Cursor,
	kSmallKeyboardCursor,
	kLargeKeyboardCursor,
	kTextEditCaretCursor
};

typedef
struct CursorData
{
	short	spriteDataId;
	short	blinkInterval;
	Point	hotSpot;
} CursorData;

struct CursorInstance;
typedef struct SegaCursor SegaCursor, * CursorPtr;

#ifndef	unix
CursorPtr	CreateSegaCursor ( void ) =
	CallDispatchedFunction( kCreateSegaCursor );
	
short	DisposeSegaCursor ( CursorPtr cursor ) =
	CallDispatchedFunction( kDisposeSegaCursor );
	
short	MoveSegaCursor (  CursorPtr cursor, short x, short y ) =
	CallDispatchedFunction( kMoveSegaCursor );
	
short	HideSegaCursor (  CursorPtr cursor ) =
	CallDispatchedFunction( kHideSegaCursor );
	
short	ShowSegaCursor (  CursorPtr cursor ) =
	CallDispatchedFunction( kShowSegaCursor );
	
short	GetSegaCursorPos (  CursorPtr cursor, short *x, short *y ) =
	CallDispatchedFunction( kGetSegaCursorPos );
	
short	SetSegaCursorImage (  CursorPtr cursor, short id ) =
	CallDispatchedFunction( kSetSegaCursorImage );

short	LoadCursorFromVRAM (  CursorPtr cursor, segaCharRect * rect, unsigned short patBase, Point hotSpot ) =
	CallDispatchedFunction( kLoadCursorFromVRAM );

void	DrawSegaCursor( CursorPtr cursor ) =
	CallDispatchedFunction( kDrawSegaCursor );
#endif	/* ! unix */

#endif	/* __cursor__ */
