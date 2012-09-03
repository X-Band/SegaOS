/*
	File:		Backdrops.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<11>	  8/9/94	CMY		Added a flag to Show/Hide Backdrop
		<10>	  8/9/94	KON		Add SetBackdropIDAndAuxBackgroundGraphic call. Remove delete
									auxgraphic call.
		 <9>	  8/8/94	KON		Move clearBackdrop  back from Priv.
		 <8>	  8/8/94	KON		Added calls to support aux background graphics.
		 <7>	  8/5/94	CMY		Changed sprite count for blinkies.
		 <6>	  8/3/94	CMY		Parameterized the number of blinky sprites.
		 <5>	  8/1/94	CMY		Added blinky sprites.
		 <4>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <3>	  7/3/94	JBH		Added GetBackdropID.
		 <2>	 6/15/94	SAH		Added fading.
		 <1>	 6/12/94	SAH		first checked in

	To Do:
*/



#ifndef __Backdrops__
#define __Backdrops__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifndef __SegaGraphics__
#include "SegaGraphics.h"
#endif

#ifndef __Database__
#include "Database.h"
#endif

#ifndef __Sprites__
#include "Sprites.h"
#endif

#define kNumBlinkies 11

OSErr	SetBackdropID ( DBID id, Boolean fade, long decompressorOptions ) =
	CallDispatchedFunction( kSetBackdropID );

OSErr	SetBackdropBitmap ( SegaBitMapPtr bitmap, Boolean fade, long decompressorOptions ) =
	CallDispatchedFunction( kSetBackdropBitmap );

void	HideBackdrop ( Boolean auxOnly ) =
	CallDispatchedFunction( kHideBackdrop );

short SetBackdropIDAndAuxBackgroundGraphic( DBID id, Boolean fade, long backGroundDecompOptions, 
											DBID graphicID, short xpos, short ypos, long graphicDecompOptions ) =
	CallDispatchedFunction( kSetAuxBackgroundGraphic );

void	ShowBackdrop ( Boolean auxOnly ) =
	CallDispatchedFunction( kShowBackdrop );
	
SpriteRef GetBlinkySprite( short index ) =
	CallDispatchedFunction( kGetBlinkySprite );

void	ClearBackdrop ( Boolean clearNow ) =
	CallDispatchedFunction( kClearBackdrop );

#endif