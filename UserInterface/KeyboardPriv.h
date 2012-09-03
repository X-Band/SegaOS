/*
	File:		KeyboardPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<16>	 8/27/94	CMY		Add some state so we know if there's a hardware keyboard.
		<15>	 8/24/94	CMY		Added a sprite for the disabled cancel key.
		<14>	 8/16/94	CMY		Added a field to the state to keep the measureProc, if it
									exists.
		<13>	 8/13/94	CMY		Added a sprite for the disabled switch key.
		<12>	  8/5/94	CMY		Added offsets for the partial keyboards.
		<11>	  8/2/94	CMY		Cache both keyboard graphics.
		<10>	 7/31/94	CMY		Put caching back in dammit!
		 <9>	 7/31/94	KON		Remove the keyboard dictionary cache.
		 <8>	 7/30/94	CMY		Cache the decompressor dictionary to make switching caps faster.
		 <7>	 7/26/94	CMY		Added code to support the QWERTY/ABCD flag.
		 <6>	 7/24/94	CMY		Added jizzle state to keyboard state.
		 <5>	 7/14/94	ADS		Remove placeholder
		 <4>	 7/13/94	ADS		Vectorize EVERYTHING
		 <3>	 7/12/94	CMY		Constants for special keys on keyboard
		 <2>	  7/8/94	CMY		Added option key and shift key state.
		 <1>	 6/30/94	CMY		first checked in

	To Do:
*/


#ifndef __KeyboardPriv__
#define __KeyboardPriv__

#ifndef __SegaGraphics__
#include "SegaGraphics.h"
#endif __SegaGraphics__

#ifndef __Sprites__
#include "Sprites.h"
#endif __Sprites__

typedef struct KeyboardEntryGlobals
{
	short			dummy;
} KeyboardEntryGlobals;

#ifdef SIMULATOR
	#define MANAGERGLOBALTYPE KeyboardEntryGlobals
#else
	extern KeyboardEntryGlobals keyEntry;
#endif

#define kKeyboardLeft		0
#define kKeyboardRight		14
#define kKeyboardTop		0
#define kKeyboardBottom		3

#define kKeyXSize			13
#define kKeyYSize			13
#define kKeyboardXOffset	54
#define kKeyboardYOffset	0

#define kKeyboardUpperOffsetX	56
#define kKeyboardUpperOffsetY	8

#define keyDelete				1
#define keyCancel				2
#define	keyShift				3
#define keyReturn				4
#define keySwitch				5
#define keyDone					6

typedef struct {
	GraphicReference		upperGraphic;
	GraphicReference		lowerGraphic;
	SpriteRef				switchKeySprite;
	SpriteRef				cancelKeySprite;
	KeyboardEntryLayout		*keyLayout;			/* The layout */
	MeasureProc				measureProc;		/* A client-supplied proc to see if text fits */
	short					x;					/* Keyboard cursor x,y */
	short					y;
	short					graphicID;			/* The keyboard graphic DBID */
	Boolean					shiftState;			/* Is shift pressed? */
	Boolean					hasJizzlers;		/* Did we create jizzlers for this guy? */
	Boolean					hasHWKeyboard;		/* Does the loser have a hardware keyboard? */
	CursorPtr				cursor;				/* The key cursor */
	void					*decompressorCache;	/* The dictionary cache to speed up CAPS switching */
	char					*lowerMap[4];		/* lower-case key map by row */
	char					*upperMap[4];		/* upper-case key map by row */
	TextEditReference		teRefs[1];			/* Text edit state for each field */
} KeyEntryState;

#define JIZZLE_FIELD(s, i) (s)->teRefs[(s)->keyLayout->fieldCount+(i)]

// internal functions, vectorized for patching


void SetFocusField(KeyboardEntryLayout *keyLayout, short fieldIndex) =
	CallDispatchedFunction( kSetFocusField );

void DrawKeyboard(KeyEntryState *state) =
	CallDispatchedFunction( kDrawKeyboard );

short ComputeCursorLineNumber( TextEditReference ref, char *buffer ) =
	CallDispatchedFunction( kComputeCursorLineNumber );

void CacheKeyboardGraphics( KeyEntryState *state ) =
	CallDispatchedFunction( kCacheKeyboardGraphics );
	
void ReleaseKeyboardGraphicsCache( KeyEntryState *state ) =
	CallDispatchedFunction( kReleaseKeyboardGraphicsCache );

#endif __KeyboardPriv__
