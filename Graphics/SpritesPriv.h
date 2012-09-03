/*
	File:		SpritesPriv.h

	Contains:	Structures for sprites manager

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<14>	 8/22/94	SAH		Added drawnZeroDelta to SyncOData.
		<13>	 8/19/94	ADS		Add new spark flags
		<12>	 8/15/94	KON		Add syncotron data structure.
		<11>	 8/11/94	JBH		Added DrawXBandLogo stuff
		<10>	  8/8/94	JBH		Sped up sparks by not having them calculate graphics during
									timeproc, but rather at setup.
		 <9>	 7/24/94	JBH		Added sparks to sprite manager.
		 <8>	 7/24/94	CMY		Added custom clut information to the sprite device decompressor.
		 <7>	 7/18/94	ADS		Dispatcher cleanup
		 <6>	 7/14/94	ADS		Remove placeholder
		 <5>	 7/10/94	ADS		more sprites!  made hardwareSprite signed for error handlign
		 <4>	 6/29/94	CMY		Added a pointer to clut data in sprite so we can swap between
									grayscale and custom cluts without doing tons of lookup.
		 <3>	 6/28/94	CMY		Added a clut reference to sprite so we can release custom cluts
									when finished.
		 <2>	 6/13/94	CMY		Added "front plane" sprites.
		 <7>	 5/24/94	SAH		Fixed compile problems for the non-simulator version.
		 <6>	 5/22/94	SAH		Made HardwareSprite fields be unsigned.
		 <5>	 5/19/94	SAH		Made use SegaBitMapPtrs and new patterns.
		 <4>	 5/18/94	SAH		Added stuff for CreateSpriteData.
		 <3>	 5/17/94	SAH		Implemented a sprite memory manager.
		 <2>	 5/14/94	SAH		Updated to new tiling model
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/

#ifndef __SpritesPriv__
#define __SpritesPriv__

#ifndef __SegaScrn__
#include "SegaScrn.h"
#endif

#ifndef __Decompress__
#include "Decompress.h"
#endif

#ifndef __Time__
#include "Time.h"
#endif

#ifndef	__Animators__
#include "Animators.h"
#endif

#define	kMaxSprite			80		// legal if we're in 40 cell mode


/*
* this type mirrors a sprite data in vram
*/
typedef
struct HardwareSprite
{
	unsigned short	empty1		: 6;
	unsigned short	vPos		: 10;
	unsigned short	empty2		: 4;
	unsigned short	hSize		: 2;
	unsigned short	vSize		: 2;
	unsigned short	empty3		: 1;
	unsigned short	link		: 7;
	unsigned short	priority	: 1;
	unsigned short	palette		: 2;
	unsigned short	vFlip		: 1;
	unsigned short	hFlip		: 1;
	unsigned short	patternBase	: 11;
	unsigned short	empty4		: 7;
	unsigned short	hPos		: 9;
} HardwareSprite;

/*
* this is an actual sprite that maps to a physical hardware sprite and to an instance of a
* decompressor. ie, this is a tile in the overall image.
*/
typedef
struct SpriteInstance
{
	HardwareSprite			pSprite;			// physical sprite data to write to vram
	short					hardwareSprite;		// physical hardware sprite we relate to
	short					spriteInvalid;		// actual hardware sprite info is invalid
} SpriteInstance;

/*
* this is the overall sprite image. it may have several SpriteInstances
*/
typedef
struct Sprite
{
	short			numTiles;					// number of tiles in this sprite
	long			patternBase;				// initial pattern allocated to us
	short			frames;						// how many overall frames the animation has
	short			curFrame;					// current overall frame
	short			patternInvalid;				// the sprite pattern is invalid ( frame advance )
	short			frontPlane;					// 1=front plane, 0=rear plane
	short			clut;						// 0 for default, 1..3 for custom clut
	short			*customClutData;			// Ptr to the actual clut data
	Decompressor	decompressor;				// decompressor we're using
	struct Sprite *	prevSprite;					// sprite allocated before us
	struct Sprite *	nextSprite;					// sprite allocated after us
	SpriteInstance	tile[1];					// array of physical sprites;
} Sprite;


typedef
struct SpriteGlobals
{
	short				spriteTableBase;
	Sprite *			firstSprite;
	Sprite *			lastSprite;
	
	/* a map of free sprites. we usually run in 40 cell mode so 80 is probably too */
	/* many */
	short				numSprites;
	unsigned char		spriteMap[ kMaxSprite >> 3 ];
} SpriteGlobals;

typedef struct Spark
{
	Sprite *		sparkSprite;
	long			sparkPattern;
	TimeProcRef		sparkProc;
	short			sparkFrame;
	short			frame0[16];
	short			frame1[16];
	short			frame2[16];
	short			frame3[16];
	short			flags;
} Spark;

#define	kXBandLogoSprites	13
#define	kXBandLogoSparks	4
#define	kXBandLogoFirstAnimationID	10

typedef struct XBandLogo
{
	SpriteRef			sprites[kXBandLogoSprites];			/* The XBAND™ graphics */
	SparkRef			sparks[kXBandLogoSparks];			/* The sparks */
	TheAnimatorData		theAnimatorData[kXBandLogoSparks];	/* TheAnimator data for sparks */
	AnimationRef		animations[kXBandLogoSparks];		/* Spark animationrefs */
} XBandLogo;

typedef struct SyncOTronData
{
	SpriteRef			sprites[kXBandLogoSprites];			/* The XBAND™ graphics */
	SparkRef			sparks[kXBandLogoSparks];			/* The sparks */
	TheAnimatorData		theAnimatorData[kXBandLogoSparks];	/* TheAnimator data for sparks */
	AnimationRef		animations[kXBandLogoSparks];		/* Spark animationrefs */
	long				topTextPatternMems;
	long				bottomTextPatternMems;
	Boolean				drawnZeroDelta;
} SyncOTronData;

typedef
struct SpriteDeviceData
{
	SegaBitMap		bitmap;
	unsigned short	patternBase;
	short			clut[16];
} SpriteDeviceData;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE SpriteGlobals
#else
extern SpriteGlobals sprites;
#endif




// internal functions, vectorized for patching


short				WriteSpriteToVDP ( SpriteInstance * sprite ) =
	CallDispatchedFunction( kWriteSpriteToVDP );

short				FigureTileSize ( short * width ) =
	CallDispatchedFunction( kFigureTileSize );

short				AllocateSprite ( void ) =
	CallDispatchedFunction( kAllocateSprite );

short				FreeSprite ( short sprite ) =
	CallDispatchedFunction( kFreeSprite );

SpriteInstance *	GetSpriteLastTile ( Sprite * sprite ) =
	CallDispatchedFunction( kGetSpriteLastTile );

SpriteInstance *	GetSpriteFirstTile ( Sprite * sprite ) =
	CallDispatchedFunction( kGetSpriteFirstTile );



#endif
