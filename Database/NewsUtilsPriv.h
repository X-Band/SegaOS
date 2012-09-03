/*
	File:		NewsUtilsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<32>	 8/26/94	CMY		Display a message in the countdown timer when the number hits
									zero.
		<31>	 8/20/94	JBH		Added backdropClut, backdropClutTextOwners, to properly track
									who owns custon CLUT for news text.
		<30>	 8/20/94	KON		Background music track is now a sound effect.
		<29>	 8/15/94	JBH		Added spinning A to XBand News bring-in
		<28>	 8/15/94	JBH		Added background graphic ID to globals to avoid redraw of
									background when it doesn't change between pages.
		<27>	 8/14/94	JBH		Added sounds to xband news fly-in
		<26>	 8/13/94	JBH		Added xband news letter fly-in
		<25>	 8/11/94	JBH		Made news timer use data in news globals, not dbconstant, when
									server needs to slow down timer
		<24>	 8/10/94	KON		Use sprites instead of graphics references for arrows and return
									icon.
		<23>	  8/9/94	KON		Add caches for right, left, and return controls to eliminate
									flicker.
		<22>	  8/9/94	KON		Removed variables related to aux background ref since the
									backgrounds handle that stuff now.
		<21>	  8/8/94	KON		Added background aux ref.
		<20>	  8/4/94	JBH		De-seeded RandomShort calls.
		<19>	  8/2/94	DJ		fixed it
		<18>	  8/2/94	DJ		added ifdef server
		<17>	 7/30/94	KON		Added background ref.
		<16>	 7/28/94	KON		Add countDownText pattern mem ref.
		<15>	 7/28/94	JBH		Added fields to support news countdown timer.
		<14>	 7/25/94	KON		Add stuff to support new UI.
		<13>	 7/25/94	KON		Add server connect semaphore, update for new UI.
		<12>	 7/24/94	KON		Changed news form to move font ID's to the news and add a second
									body of text.
		<11>	 7/14/94	ADS		Remove placeholder
		<10>	 7/13/94	ADS		Vectorize EVERYTHING
		 <9>	 7/10/94	KON		Add news read fields.
		 <8>	  7/6/94	KON		Added background music sound track in place of animation2.
		 <7>	  7/5/94	KON		Add animation to news.
		 <6>	 6/30/94	KON		Add left and right arrow refs.
		 <5>	 6/30/94	KON		Added graphicReference for bitmap to fix memory leaks.
		 <4>	 6/30/94	KON		Add globals to manage pattern memory allocations.
		 <3>	 6/28/94	KON		Added more stuff for handling news during server connect.
		 <2>	 6/27/94	KON		Add newsValid global.
		 <1>	 6/13/94	SAH		first checked in

	To Do:
*/

#ifndef __NewsUtilsPriv__
#define __NewsUtilsPriv__

char *	_FindNextNewsString(char *text, long lineNum);

#ifndef __SERVER__

#ifndef __Animation__
#include "Animation.h"
#endif __Animation__

#ifndef __Sprites__
#include "Sprites.h"
#endif __Sprites__

#ifndef __Time__
#include "Time.h"
#endif __Time__

#ifndef __Utils__
#include "Utils.h"
#endif __Utils__

#ifndef __Progress__
#include "Progress.h"
#endif __Progress__

#define kNumNewsSparks	3
#define kFirstNewsSparkPathID	150

typedef struct NewsSpark {
	SparkRef	theSpark;
	SpriteRef	theSprite;
	TimeProcRef	timeProc;
	Rect		bounds;
	short		xLoc;
	short 		yLoc;
	short		dx;
	short 		dy;
} NewsSpark;

/* Holds info needed to draw the XBandNews headline, and animations associated with it */
typedef struct NewsXBandNews {
	SpriteRef		*sprites;
	short			*xPos;
	short			*yPos;
	short			*frameChanges;
	DBID			*sounds;
	SpriteRef		letterA;
	short			letterAXPos;
	short			letterAYPos;
	short			letterAFirstID;
	short			letterAFrames;
	TimeProcRef		timeProc;
	short			frameNumber;
	short			spriteNumber;
	short			totalSprites;
	short			timeDelta;
	short			xDelta;
} NewsXBandNews;

/* These next 2 structures describe the 'XBND' resource */
typedef struct NewsXBandNewsSprite {
	DBID			bitmapID;
	DBID			soundFXID;
	short			yPos;
	short			finalXPos;
} NewsXBandNewsSprite;

typedef struct NewsXBandNewsData {
	short					numberOfSprites;
	short					timeDelta;
	short					xDelta;
	NewsXBandNewsSprite		entries[1];
} NewsXBandNewsData;

typedef struct NewsGlobals
{
	short	generalNewsRead;	//bitfield which indicates whether each user has read the news
	short	catapultNewsRead;
	short	newsValid;			//used as temporary flag to tell server download code whether it's
							//ok to view the news.
	short	numNewsPages;	//used by news control idle
	short	curNewsPage;	//used by news control idle
	AnimationRef 		animation1;
	GraphicReference	bitmapRef;
	SpriteRef 	rightArrowRef;
	SpriteRef	leftArrowRef;
	SpriteRef	returnIconRef;
	long	headlinePattern;
	long	tagPattern;
	long	pageNumPattern;
	long	datePattern;
	long	body1Pattern;
	long	body2Pattern;
	long	countDownTimerPatterns;
	Boolean	duringServerConnect;
	DBType	whichNews;
	
	short	backdropClut;
	short	backdropClutTextOwners;
	
	ProgressRef progress;
	TimeProcRef progressTimeProc;
	long		progressTextPatterns;
	Rect		progressTextRect;
	char		*timerDoneMessage;
	short timesCalled;
	short maxTimes;
	short timeBase;
	long	newTimeConst;		/* 0 normally, set when server changes things */
	
	NewsSpark *sparks[kNumNewsSparks];
	short				returnIconX;
	short				returnIconY;
	short				rightArrowX;
	short				rightArrowY;
	short				leftArrowX;
	short				leftArrowY;
	DBID 				rightArrowID;
	DBID 				leftArrowID;
	DBID 				formID;
	DBID				formBackgroundID;
	NewsXBandNews		*xbandNewsRef;
} NewsGlobals;

#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	NewsGlobals
#else
extern NewsGlobals news;
#endif


void 	_PackNewsStrings(char *packed, long numStrings, char **strings);

// internal functions, vectorized for patching

void ShowLeftRightPageControls( short flags ) =
	CallDispatchedFunction( kShowLeftRightPageControls );

void DrawNewsReturnIcon( void ) =
	CallDispatchedFunction( kDrawNewsReturnIcon );
	
void DrawXBandNews( Boolean shouldFade ) =
	CallDispatchedFunction( kDrawXBandNews );
	
void DisposeXBandNews( void ) =
	CallDispatchedFunction( kDisposeXBandNews );

#endif __SERVER__
#endif __NewsUtilsPriv__