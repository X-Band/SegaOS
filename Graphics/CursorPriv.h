/*
	File:		CursorPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 5/28/94	SAH		Oops. Created a dummy CursorGlobals struct for the sega build.
		 <2>	 5/28/94	SAH		Updated to allow multiple cursors.
		 <2>	 5/20/94	SAH		Added hotspot.
		 <1>	 5/17/94	SAH		first checked in

	To Do:
*/


struct SegaCursor
{
	short			xPos;
	short			yPos;
	short			id;
	short			cursorVis;
	short			blinkInterval;
	short			active;
	Point			hotSpot;
	SpriteRef		cursorSprite;
	AnimationRef	cursorAnimation;
};

typedef
struct CursorGlobals
{
	short	dummy;
} CursorGlobals;

#ifdef SIMULATOR
#define MANAGERGLOBALTYPE CursorGlobals
#else

extern CursorGlobals cursor;

#endif

