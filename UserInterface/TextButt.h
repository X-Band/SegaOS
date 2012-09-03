/*
	File:		TextButt.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<10>	 8/22/94	CMY		Don't cache sprites for deselected buttons.
		 <9>	 8/18/94	CMY		Multiple chortle procs.
		 <8>	 8/17/94	CMY		Support chortling text. Removed palette. Added flags.
		 <7>	 7/30/94	CMY		Added code to control caching of the decompressor dictionary.
		 <6>	 7/22/94	JOE		Button animations can now have attached sound effects.
		 <5>	 7/21/94	CMY		Button animations are now driven by kButtonSequence database
									items.
		 <4>	 7/19/94	CMY		TextButts now draw buttonGraphics, not SEGA graphics.
		 <3>	 5/30/94	KON		Fix up header includes.
		 <2>	 5/27/94	KON		Removed duplicate include of DitlMgr.h.
		 <5>	 5/23/94	SAH		Added text x and y offsets. Made TextButtonDesc fields bytes.
		 <4>	 5/21/94	SAH		Added animation data in place of bitmap ptr.
		 <3>	 5/20/94	SAH		Added the font and palette parameters. Also replaced foreColor
									and backColor in favour of color0..3.
		 <2>	 5/19/94	KON		Fix the TextButtonDesc that projector crapped on.
		 <1>	 5/18/94	KON		first checked in

	To Do:
*/

#ifndef __TextButt__
#define __TextButt__

#ifndef __SegaBM__
#include "SegaBM.h"
#endif __SegaBM__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__

#ifndef	__Animation__
#include "Animation.h"
#endif __Animation__

#ifndef	__Animators__
#include "Animators.h"
#endif __Animators__

#ifndef __Time__
#include "Time.h"
#endif

typedef void (*ChortleProc)( struct TextButtPriv * );

typedef struct TextButtPriv
{
	long			textPattern;
	segaCharRect	textRect;
	short			xLeft;
	short			curXLeft;
	short			finalY;
	short			yTop;
	SpriteRef		buttonSprite;
	TimeProcRef		animatorProc;
	char			buttonID;
	char			currentFrame;
	short			frameCount;
	void			*toKeepDictCached;
	char			*frameSequence;
	FXSequence		*fxSequence;
	short			theta;
	short			spriteCount;
	short			characterCount;
	char			*theString;
	SpriteRef		*characterSprites;
	short			xPos;
	Boolean			textStep;
	ChortleProc		chortleProc;
	struct TextButtonDesc	*theTextButt;
} TextButtPriv;

typedef struct TextButtonDesc
{
	FrameDelayData	animationData;
	unsigned char	textXCenter;
	unsigned char	textYTop;
	unsigned char	pixelWidth;
	unsigned char	color0;
	unsigned char	color1;
	unsigned char	color2;
	unsigned char	color3;
	unsigned char	flags;
	char			buttonID;
	short			font;
	char			*myCString;
} TextButtonDesc;

typedef struct TextButtonSetup
{
	FrameDelayDataSetup	animationData;
	unsigned char		textXCenter;
	unsigned char		textYTop;
	unsigned char		pixelWidth;
	unsigned char		color0;
	unsigned char		color1;
	unsigned char		color2;
	unsigned char		color3;
	unsigned char		flags;
	short				font;
	DBID				myCStringID;
} TextButtonSetup;

doDITLItemProc GetTextButtonPtr( void );


enum
{
	kSineChortleProc = 1,
	kPalsyChortleProc,
	kAccordianChortleProc
};

Ptr GetSecretChortleProc( void );
Ptr GetSineChortleProc( void );

void InstallChortleProcs( void );

#endif __TextButt__
