/*
	File:		Sprites.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<20>	 8/21/94	KON		DrawSpriteInFront now takes a hueShift.
		<19>	 8/19/94	ADS		Add new spark flags
		<18>	 8/15/94	KON		Use sync-o-tron ref rather than xband ref since sync-o is a
									little more complicated now.
		<17>	 8/13/94	KON		Added sync-o-tron animation.
		<16>	 8/11/94	JBH		Added kDrawSparkInFront.
		<15>	 8/11/94	JBH		Added DrawXBandLogo, DisposeXBandLogoRef,
									DisposeXBandLogoSparks.
		<14>	  8/1/94	CMY		Added SetSpritePalette
		<13>	 7/26/94	CMY		Added CreateSpriteHigh to allocate sprites in high pattern mems.
		<12>	 7/24/94	JBH		Added sparks
		<11>	 7/24/94	CMY		Modified CreateSpriteData to take clut information
		<10>	 7/19/94	CMY		Added SetSpriteImage to switch the image for animation
		 <9>	  7/2/94	DJ		added ifdef __SERVER__
		 <8>	  7/2/94	JBH		Added SetSpriteTilePosition.
		 <7>	 6/29/94	CMY		Added SetSpriteGrayFlag.
		 <6>	 6/28/94	CMY		Removed palette parameters from sprite calls.
		 <5>	 6/21/94	CMY		Added ExplodeSprite.
		 <4>	 6/20/94	CMY		CreateTextSpriteFromBitmap to support animated dialogs.
		 <3>	 6/13/94	CMY		Added "front plane" sprites.
		 <2>	 6/13/94	CMY		Added TextSprites.
		 <4>	 5/19/94	SAH		Updated to take SegaBitMapPtrs rather than old SpriteDataTable.
		 <3>	 5/18/94	SAH		Added CreateSpriteData.
		 <2>	 5/14/94	SAH		Compressed sprite data structure. Removed stuff from old Synch.h
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/



#ifndef __Sprites__
#define __Sprites__

#ifndef __Sega__
#include "Sega.h"
#endif

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifndef __SegaGraphics__
#include "SegaGraphics.h"
#endif


/* our public hidden sprite types */
typedef long * SpriteRef;
typedef long * SparkRef;
typedef long * XBandLogoRef;
typedef long * SyncoTronRef;

enum	{							// add these to NewSpark( color ) for following effects:
	kDrawSparkInFront	= 0x8000,		/* to make it in front */
	kSparkOKInDialogs	= 0x4000,		/* to run during dialogs */
	kSparkOneShot		= 0x2000		/* to run once per StartSpark() */
};


#ifndef __SERVER__

SpriteRef	CreateSprite ( SegaBitMap * data ) =
	CallDispatchedFunction( kCreateSprite );

SpriteRef	CreateSpriteInFront ( SegaBitMap * data, unsigned char hueShift ) =
	CallDispatchedFunction( kCreateSpriteInFront );

SpriteRef	CreateSpriteHigh ( SegaBitMap * data ) =
	CallDispatchedFunction( kCreateSpriteHigh );

short	DisposeSprite ( SpriteRef sprite ) =
	CallDispatchedFunction( kDisposeSprite );

short	MoveSprite ( SpriteRef sprite, short x, short y ) =
	CallDispatchedFunction( kMoveSprite );

short	DrawSprite ( SpriteRef sprite ) =
	CallDispatchedFunction( kDrawSprite );

short	ExplodeSprite ( SpriteRef sprite ) =
	CallDispatchedFunction( kExplodeSprite );

short	IncrementSpriteFrame ( SpriteRef sprite, short increment ) =
	CallDispatchedFunction( kIncrementSpriteFrame );

short	SetSpriteFrame ( SpriteRef sprite, short frame ) =
	CallDispatchedFunction( kSetSpriteFrame );

short	GetSpriteFrame ( SpriteRef sprite, short * frame ) =
	CallDispatchedFunction( kGetSpriteFrame );

short	FlipSprite ( SpriteRef sprite, Boolean hFlip, Boolean vFlip ) =
	CallDispatchedFunction( kFlipSprite );

SegaBitMap * CreateSpriteData ( segaCharRect *myRect, unsigned short basePattern, short clut[16] ) =
	CallDispatchedFunction( kCreateSpriteData );

SpriteRef	CreateTextSprite( segaCharRect *textRect, long patternStart ) =
	CallDispatchedFunction( kCreateTextSprite );

SpriteRef CreateTextSpriteFromBitmap( SegaBitMap *bitmapData ) =
	CallDispatchedFunction( kCreateTextSpriteFromBitmap );
	
short SetSpriteGrayFlag( SpriteRef sprite, Boolean grayFlag ) =
	CallDispatchedFunction( kSetSpriteGrayFlag );
	
short SetSpriteTilePosition(SpriteRef sprite, short tile, short x, short y) =
	CallDispatchedFunction( kSetSpriteTilePosition );
	
short SetSpriteImage( SpriteRef sprite, SegaBitMap * image ) =
	CallDispatchedFunction( kSetSpriteImage );

short SetSpritePalette( Spri