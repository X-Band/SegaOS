/*
	File:		BackdropsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 8/19/94	CMY		Only 4 blinky cluts.
		 <5>	  8/8/94	KON		Move clearBackdrop to Backdrops.h to make it public again.
		 <4>	  8/8/94	KON		Added globals to support aux background screen.
		 <3>	  8/1/94	CMY		Added a constant for the number of cluts for bliny sprites.
		 <2>	 6/28/94	CMY		Added graphics references to handle clut and pattern management.
		 <1>	 6/12/94	SAH		first checked in

	To Do:
*/



typedef
struct BackGlobals
{
	GraphicReference	backdropReference;
	GraphicReference	auxGraphicsRef;
	DBID				auxGraphicsDBID;
	short				auxGraphicsXpos;
	short				auxGraphicsYpos;
	long				auxGraphicsDecompressorOptions;
	long				options;
	DBID				id;
	DBID				hideID;
	unsigned char		oldBackgroundColor;
} BackGlobals;

#define kNumBlinkyCluts 4

#ifdef SIMULATOR
#define MANAGERGLOBALTYPE BackGlobals

#else
extern BackGlobals backdrop;
#endif
	