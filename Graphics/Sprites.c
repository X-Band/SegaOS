/*
	File:		Sprites.c

	Contains:	Sprite Manager

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<64>	 8/26/94	KON		Handle nil sprites everywhere.
		<63>	 8/26/94	CMY		Change Sync-o-tron to use XBAND Bold 12.
		<62>	 8/22/94	SAH		Only draw the SyncO final screen once.
		<61>	 8/21/94	KON		CreateSpriteInFront now takes a hueShift.
		<60>	 8/19/94	ADS		Add spark flags
		<59>	 8/15/94	KON		Add sync-o-tron text and patent pending wstrings.
		<58>	 8/13/94	KON		Added sync-o-tron animation.
		<57>	 8/11/94	JBH		Added option for sparks to be drawn inFront.
		<56>	 8/11/94	JBH		Added DrawXBandLogo, DisposeXBandLogoRef,
									DisposeXBandLogoSparks.
		<55>	 8/10/94	KON		Check for nil in DisposeSprite.
		<54>	  8/8/94	JBH		Sped up sparks by not having them calculate graphics during
									timeproc, but rather at setup.
		<53>	  8/4/94	JBH		De-seeded RandomShort calls.
		<52>	  8/1/94	CMY		Added SetSpriteImage
		<51>	 7/30/94	CMY		Moved caching strategy out of here and into the caller's
									control.
		<50>	 7/29/94	CMY		Make the sprite grayFlag work with the decompressor clut.
		<49>	 7/29/94	KON		Add decompress options to support priority bit.
		<48>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<47>	 7/26/94	CMY		Replaced tricky code with code that works for creating sprites
									in high pattern mems.
		<46>	 7/26/94	CMY		Added CreateSpriteHigh to allocate sprites in high pattern mems.
		<45>	 7/26/94	JBH		Fixed pattern memory leak in sparks (1 cell per spark!)
		<44>	 7/25/94	CMY		Fixed a memory leak in the SpriteDeviceDecompressor and removed
									an earlier attempt at fixing it.
		<43>	 7/24/94	JBH		Added 'sparks
		<42>	 7/24/94	CMY		Modified the sprite device decompressor to handle custom cluts.
		<41>	 7/21/94	CMY		Fixes for grayscale sprites.
		<40>	 7/19/94	CMY		Added SetSpriteImage to change image data for animation.
		<39>	 7/18/94	CMY		It now asks the decompressor what clut the image wants instead
									of figuring it out by itself.
		<38>	 7/17/94	CMY		Added Dictionary decompressor
		<37>	 7/14/94	ADS		Remove placeholder
		<36>	 7/12/94	ADS		Fix edge conditions (first or last sprite)
		<35>	 7/10/94	ADS		Fix CreateSprite flicker
		<34>	 7/10/94	ADS		Allow multiple "frontmost".  Cleanup error handling
		<33>	 7/10/94	JOE		play FX indirectly
		<32>	  7/9/94	SAH		Don't do the soft init stuff for the ROMDB build.
		<31>	  7/8/94	SAH		Fixed bug with decompressor on sega.
		<30>	  7/8/94	SAH		Fix sega compile problems with the sprite decompressor.
		<29>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		<28>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<27>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<26>	  7/3/94	JBH		Made _SetSpriteTilePosition not invalidate the sprite if it did
									not move.
		<25>	  7/2/94	JBH		Added _SetSpriteTilePosition, for dialog explosions.
		<24>	  7/1/94	KON		Fix memory leak in CreateTextSprite.
		<23>	 6/29/94	CMY		Added SetSpriteGrayFlag to support grayscale sprites.
		<22>	 6/28/94	CMY		Updated to use palette manager and removed palette parameter
									from sprite calls.
		<21>	 6/27/94	SAH		Make SoftInit return noErr.
		<20>	 6/22/94	CMY		Changed sound code to use default pitch and duration. Seems to
									fix the sounds.
		<19>	 6/21/94	CMY		(Wik Joe) Add sounds to ExplodeSprite.
		<18>	 6/21/94	CMY		Added ExplodeSprite.
		<17>	 6/20/94	CMY		CreateTextSpriteFromBitmap to support animated dialogs.
		<16>	 6/16/94	CMY		Made flicker debugging code optional.
		<15>	 6/15/94	SAH		Fixed sprite flicker problems.
		<14>	 6/15/94	CMY		Support custom palettes for sprites.
		<13>	 6/14/94	CMY		Return nil in CreateTextSprite if we're out of pattern mems.
		<12>	 6/14/94	CMY		Fixed a nasty memory trasher in DrawSprite. Also fixed
									TextSprite to calculate window sizes correctly.
		<11>	 6/13/94	CMY		Fixed a bug in Sprite tiling calculations, a bug in TextGDevice,
									and a bug in front plane sprites.
		<10>	 6/13/94	CMY		Added "front plane" sprites.
		 <9>	 6/13/94	CMY		Added TextSprites.
		 <8>	 6/12/94	CMY		Changed sprite tiling to be horizontal rather than vertical.
		 <7>	 6/12/94	SAH		Fixed bug with disposing the very last sprite.
		 <6>	 5/30/94	SAH		Init globals at soft init time.
		 <5>	 5/28/94	CMY		Added calls to CheckCodeForJTRefs when adding procs to DB.
		 <4>	 5/26/94	SAH		Globals are set up for us now at soft initialize.
		 <3>	 5/26/94	SAH		New VDP stuff.
		 <2>	 5/26/94	SAH		Get the sprite table base address at soft init time.
		<10>	 5/25/94	HEC		new memory manager
		 <9>	 5/23/94	SAH		Look at real sprite base address. Fix a stupid sprite list bug.
									Use new CreateDecompressor.
		 <8>	 5/22/94	SAH		Make sure to update sprite list when disposing sprites!
		 <7>	 5/21/94	SAH		Made sure to allocate enough patterns.
		 <6>	 5/19/94	SAH		Made work with new pattern manager. Updated sprite vram
									decompressor.
		 <5>	 5/18/94	HEC		long pattern types
		 <4>	 5/18/94	SAH		Added CreateSpriteData. Fixed bugs.
		 <3>	 5/17/94	SAH		Implemented a sprite memory manager.
		 <2>	 5/14/94	SAH		Updated to new decompression scheme.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/


#include "SegaOS.h"
#include "Sprites.h"
#include "DBTypes.h"
#include "Decompress.h"
#include "SpritesPriv.h"
#include "Decompressors.h"
#include "Heaps.h"
#include "Reliability.h"
#include "Errors.h"
#include "SegaVDP.h"
#include "SegaText.h"
#include "SegaCluts.h"
#include "Pattern.h"
#include "MungeCode.h"
#include "Time.h"
#include "Utils.h"
#include "SegaSound.h"
#include "CompressDictionary.h"
#include "Animation.h"
#include "SegaBM.h"
#include "GraphicsDB.h"
#include "PathProcs.h"
#include "StringDB.h"

#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif


//#define	SPRITE_DEBUG
//#define FLICKER_DEBUG



/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
SpriteGlobals sprites;
#endif



//
//  public, vectorized private functions
//
Sprite *			_CreateSprite ( SegaBitMap * data );
Sprite *			_CreateSpriteInFront ( SegaBitMap * data, unsigned char hueShift );
Sprite *			_CreateSpriteHigh ( SegaBitMap * data );
short				_DisposeSprite ( Sprite * sprite );
short				_MoveSprite ( Sprite * sprite, short x, short y );
short				_DrawSprite ( Sprite * sprite );
short				_IncrementSpriteFrame ( Sprite * sprite, short increment );
short				_SetSpriteFrame ( Sprite * sprite, short frame );
short				_GetSpriteFrame ( Sprite * sprite, short * frame );
short				_FlipSprite ( Sprite * sprite, Boolean hFlip, Boolean vFlip );
SegaBitMap *		_CreateSpriteData ( segaCharRect *myRect, unsigned short basePattern, short * );
Sprite *			_CreateTextSprite( segaCharRect *textRect, long patternStart );
Sprite *			_CreateTextSpriteFromBitmap( SegaBitMap *bitmapData );
short				_ExplodeSprite( Sprite *sprite );
short				_SetSpriteGrayFlag( Sprite *sprite, Boolean grayFlag );
short				_SetSpriteTilePosition(Sprite *sprite, short tile, short x, short y);
short				_SetSpriteImage(Sprite *sprite, SegaBitMap *image);
Spark *				_NewSpark( short color );
Sprite *			_GetSparkSprite( Spark *spark );
void 				_DisposeSpark( Spark *spark );
void 				_StartSpark( Spark *spark );
void 				_StopSpark( Spark *spark );
short				_SetSpritePalette( Sprite *sprite, short palette );
XBandLogo *			_DrawXBandLogo( void );
void				_DisposeXBandLogoRef( XBandLogo *ref );
void				_DisposeXBandLogoSparks( XBandLogo *ref );
SyncOTronData * 	_SyncOTron( SyncOTronData * theLogo, short delta );

//
//  local, vectorized private functions
//
short				_WriteSpriteToVDP ( SpriteInstance * sprite );
short				_FigureTileSize ( short * width );
short				_AllocateSprite ( void );
short				_FreeSprite ( short sprite );
SpriteInstance *	_GetSpriteLastTile ( Sprite * sprite );
SpriteInstance *	_GetSpriteFirstTile ( Sprite * sprite );



//
//  local, non-vectorized private functions
//
static Sprite* 		CreateSpriteCommon ( SegaBitMap * data, Boolean inFront, Boolean high, unsigned char hueShift );



long
_SpriteControl ( short command, long data )
{
long			offset;
short			error;
SpriteGlobals *	globals;
short			ii;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(sprites,offset);
			
			error = AllocateGlobalSpace ( kSpriteManager, offset, sizeof(SpriteGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create sprite globals" );
			if ( error != noErr )
				{
				return error;
				}
				
			/* install our selectors */
			SetDispatchedFunction ( kCreateSprite,			kSpriteManager,		_CreateSprite );
			SetDispatchedFunction ( kCreateSpriteInFront,	kSpriteManager,		_CreateSpriteInFront );
			SetDispatchedFunction ( kCreateSpriteHigh,		kSpriteManager,		_CreateSpriteHigh );
			SetDispatchedFunction ( kDisposeSprite,			kSpriteManager,		_DisposeSprite );
			SetDispatchedFunction ( kMoveSprite,			kSpriteManager,		_MoveSprite );
			SetDispatchedFunction ( kDrawSprite,			kSpriteManager,		_DrawSprite );
			SetDispatchedFunction ( kIncrementSpriteFrame,	kSpriteManager,		_IncrementSpriteFrame );
			SetDispatchedFunction ( kSetSpriteFrame,		kSpriteManager,		_SetSpriteFrame );
			SetDispatchedFunction ( kGetSpriteFrame,		kSpriteManager,		_GetSpriteFrame );
			SetDispatchedFunction ( kFlipSprite,			kSpriteManager,		_FlipSprite );
			SetDispatchedFunction ( kCreateSpriteData,		kSpriteManager,		_CreateSpriteData );
			SetDispatchedFunction ( kCreateTextSprite,		kSpriteManager, 	_CreateTextSprite );
			SetDispatchedFunction ( kCreateTextSpriteFromBitmap,kSpriteManager,	_CreateTextSpriteFromBitmap );
			SetDispatchedFunction ( kExplodeSprite,			kSpriteManager,		_ExplodeSprite );
			SetDispatchedFunction ( kSetSpriteGrayFlag,		kSpriteManager,		_SetSpriteGrayFlag );
			SetDispatchedFunction ( kSetSpriteTilePosition,	kSpriteManager,		_SetSpriteTilePosition );
			SetDispatchedFunction ( kSetSpriteImage,		kSpriteManager,		_SetSpriteImage );
			SetDispatchedFunction ( kSetSpritePalette,		kSpriteManager,		_SetSpritePalette );

			SetDispatchedFunction ( kWriteSpriteToVDP,			kSpriteManager,		_WriteSpriteToVDP );
			SetDispatchedFunction ( kFigureTileSize,			kSpriteManager,		_FigureTileSize );
			SetDispatchedFunction ( kAllocateSprite,			kSpriteManager,		_AllocateSprite );
			SetDispatchedFunction ( kFreeSprite,				kSpriteManager,		_FreeSprite );
			SetDispatchedFunction ( kGetSpriteLastTile,			kSpriteManager,		_GetSpriteLastTile );
			SetDispatchedFunction ( kGetSpriteFirstTile,		kSpriteManager,		_GetSpriteFirstTile );
			SetDispatchedFunction ( kNewSpark,					kSpriteManager,		_NewSpark );
			SetDispatchedFunction ( kGetSparkSprite,			kSpriteManager,		_GetSparkSprite );
			SetDispatchedFunction ( kDisposeSpark,				kSpriteManager,		_DisposeSpark );
			SetDispatchedFunction ( kStartSpark,				kSpriteManager,		_StartSpark );
			SetDispatchedFunction ( kStopSpark,					kSpriteManager,		_StopSpark );
			SetDispatchedFunction ( kDrawXBandLogo,				kSpriteManager,		_DrawXBandLogo );
			SetDispatchedFunction ( kDisposeXBandLogoRef,		kSpriteManager,		_DisposeXBandLogoRef );
			SetDispatchedFunction ( kDisposeXBandLogoSparks,	kSpriteManager,		_DisposeXBandLogoSparks );
			SetDispatchedFunction ( kSyncOTron,					kSpriteManager,		_SyncOTron );
			break;
		
		case kSoftInialize:
#ifndef ROMDB
			/* init our globals here */
			REFGLOBAL(sprites,spriteTableBase) = (long) GetSegaScreenBaseAddr ( kSprite );
			REFGLOBAL(sprites,lastSprite) = 0L;
			REFGLOBAL(sprites,firstSprite) = 0L;
			REFGLOBAL(sprites,numSprites) = 64;
				
			/* mark all sprites as free. */
			for ( ii = ( kMaxSprite >> 3 ) - 1; ii >= 0; --ii )
				{
				REFGLOBAL(sprites,spriteMap)[ ii ] = 0;
				}
#endif
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


Sprite *
_CreateSpriteHigh ( SegaBitMap * data )
{
	CreateSpriteCommon(data, false, true, 0);
}

/*
* create a sprite from a SpriteDataTable. The caller owns the memory to the data table
* (this allows ROM ptrs to be passed in. if it is real RAM, it's their responsibility to
* dispose of it after they're done with the sprite.
*
* Sprites are tiled horizontally as such:
*			0	1	2	3	4
*			5	6	7	8	9
*/
Sprite *
_CreateSprite ( SegaBitMap * data )
{
	CreateSpriteCommon(data, false, false, 0);
}

/*
 * Same as above, but allocates in the "front" sprite plane. Of course, there's no such
 * thing as a front sprite plane, but we use the link data to make sure that all sprites
 * allocated in the "front" plane appear in front of all other sprites. Sprites allocated
 * in the front plane are prioritized in the order they are created. Thus, if we create
 * sprites A, B, then C in the front plane, A will be above B which will be above C.
 */
Sprite *
_CreateSpriteInFront ( SegaBitMap * data, unsigned char hueShift )
{
	CreateSpriteCommon( data, true, false, hueShift );
}

/*
 * Common code called by CreateSprite and CreateFrontSprite. It does the actual sprite
 * allocation work.
 */
static Sprite *CreateSpriteCommon ( SegaBitMap * data, Boolean inFront, Boolean high, unsigned char hueShiftAmount )
{
Sprite *		sprite;
Ptr				spriteData;
Decompressor	dec;
short			numTiles;
unsigned short	size;
unsigned short	tilePatternSize;
SegaErr			err;
short			ii;
SpriteInstance *tile;
SpriteInstance *prevTile;
SpriteInstance *linkTile = 0;
short			linkNumber;
short			width;
short			height;
short			hTiles;
short			vTiles;
long			patternBase;
short			spriteNum;
unsigned short	terminalLink = 0;
short			palette = 0;
short			*clutData = nil;
short			iii;

	err = noErr;
	
	/* create the decompressor */
	dec = CreateDecompressor ( data, palette, 0 );
	ASSERT_MESG ( dec, "Can't allocate decompressor" );
	if ( !dec )
		{
		return 0L;
		}
		
	{
		clutData = NewMemory( kTemp, 16*sizeof(short) );
		
		if ( GetImageClut(dec, clutData) == 0)
		{
			if( hueShiftAmount != 0 )
			{
				for( iii = 0; iii < 16; iii++ )
				{
					clutData[iii] = HueShift( clutData[iii], hueShiftAmount );
				}
			}
			palette = RequestClut( clutData );
			SetDecompressorPalette( dec, palette );
		}
		else
		{
			DisposeMemory( clutData );
			clutData = nil;	
		}
	}



	/* figure out the tiling for this sprite */
	width = data->width;
	hTiles = FigureTileSize ( &width );

	height = data->height;
	vTiles = FigureTileSize ( &height );
	
	SetImageTiling ( dec, width, height );
	
	numTiles = hTiles * vTiles;
	
	/* create a sprite record of the right size */
	size = sizeof(Sprite);
	size += ( numTiles - 1 ) * sizeof(SpriteInstance);

	tilePatternSize = ( width + 1 ) * ( height + 1 );
	
	sprite = NewMemory ( kTemp, size );
	ASSERT_MESG ( sprite, "Can't allocate sprite" );
	
	if ( sprite )
		{
		/* run through all the tiles in this sprite and set there hardwareSprite num to -1 */
		/* so we can tell which sprites were actually allocated if we fail */
		tile = sprite->tile;
		for ( ii = numTiles - 1; ii >= 0 ; --ii )
			{
			tile->hardwareSprite = -1;
			tile++;
			}
					
		sprite->numTiles = numTiles;
		sprite->decompressor = dec;
		sprite->frames = data->frames;
		sprite->curFrame = 0;
		sprite->patternInvalid = true;
		sprite->frontPlane = inFront;
		sprite->clut = palette;
		sprite->customClutData = clutData;
		
		//  setup my own links.  DON'T install in anyone else's links
		//  until we have completely created the sprite successfully
		
		if (inFront) {
			sprite->nextSprite = REFGLOBAL(sprites,firstSprite);
			sprite->prevSprite = 0L;
		} else {
			sprite->nextSprite = 0L;
			sprite->prevSprite = REFGLOBAL(sprites,lastSprite);
			}
		
		/* allocate patterns for the whole sprite. if we're dealing with the */
		/* sprite device, we don't need to */
		if ( data->compressType == kVRamData )
			{
			/* turn the sprite address into a pattern id */
			patternBase = ( (SpriteDeviceData *) data)->patternBase;
			}
		else
			{
				if (high)
					patternBase = NewPatternBlockHigh( numTiles * tilePatternSize );
				if (!high || (patternBase == 0) )
					patternBase = NewPatternBlock ( numTiles * tilePatternSize );
			}

		ASSERT_MESG(patternBase, "No patterns for sprite");
		sprite->patternBase = patternBase;
		
		SetDecompressDstPattern ( dec, patternBase );

		prevTile = 0L;

		/* now walk through and actually create all the tiles */
		tile = sprite->tile;
		for ( ii = 0; ii < numTiles; ++ii )
			{
			tile->spriteInvalid = true;
			
			/* allocate a physical hardware sprite */
			tile->hardwareSprite = AllocateSprite ();
			ASSERT_MESG ( tile->hardwareSprite >= 0, "All out of hardware sprites" );
			
			/* were we able to get one? */
			if ( tile->hardwareSprite < 0 )
				{
				/* no, kill the sprite */
				err = outOfSpritesErr;
				break;
				}

			/* make the previous tile (if there was one) point at this sprite */
			if ( prevTile )
				{				// do this most times through
#ifdef SPRITE_DEBUG			
				printf ( "linking sprite %d to sprite %d\n", prevTile->hardwareSprite, 
					tile->hardwareSprite );
#endif
					
				prevTile->pSprite.link = tile->hardwareSprite;
				}
			else
				{				// do this first time only
				if ( ! inFront )
					{
					if (sprite->prevSprite)
						{
						linkTile = GetSpriteLastTile ( sprite->prevSprite );
						linkNumber = tile->hardwareSprite;
						}
					}
				}
				
			prevTile = tile;

#ifdef SPRITE_DEBUG			
			printf ( "sprite %lx: ", sprite );
#endif			
			/* set up the physical sprite */
			tile->pSprite.hPos = ( ii % hTiles ) * 8 * ( width + 1);
			tile->pSprite.vPos = ( ii / hTiles ) * 8 *  ( height + 1);
			tile->pSprite.hSize = width;
			tile->pSprite.vSize = height;
			tile->pSprite.link = 0;			// next iteration will probably replace this
	
			tile->pSprite.priority = 0;		//sprites are already in front!
			if( inFront )
				tile->pSprite.priority = 1;		//sprites are already in front!
			
			tile->pSprite.palette = palette;
			tile->pSprite.hFlip = 0;
			tile->pSprite.vFlip = 0;
			tile->pSprite.empty1 = 0;
			tile->pSprite.empty2 = 0;
			tile->pSprite.empty3 = 0;
			tile->pSprite.empty4 = 0;
			
			/* set this sprites pattern base */
			tile->pSprite.patternBase = patternBase;
			patternBase += tilePatternSize;
			
			WriteSpriteToVDP( tile );		// install this tile in the VDP
						
			tile++;
			}
		
		//  WHEN successful sprite created, THEN link it into the various lists

		if (err == noErr)	{
		
				//	1.  Install into singly-linked hardware sprite list
				
											// assumes prevTile points to last tile worked on
			if (inFront)					// point final tile at first tile of next sprite
				{
				if (sprite->nextSprite)
					prevTile->pSprite.link = GetSpriteFirstTile(sprite->nextSprite)->hardwareSprite;
					
				// else, we are the first and ONLY sprite.  our last link should be zero to terminate
				// the hardware sprite engine.  It's already zero (see the loop, above) so we just
				// don't do anything.
				}
			else	{
				if (linkTile)
					{
					linkTile->pSprite.link = linkNumber;
					WriteSpriteToVDP( linkTile );		// point last tile of prev sprite at my first tile
					}
			}

				
				//	2.  install doubly-linked logical sprites linked list
			
			if (inFront)	{
				if (sprite->nextSprite)
					sprite->nextSprite->prevSprite = sprite;
				REFGLOBAL(sprites,firstSprite) = sprite;
				
				if (! REFGLOBAL(sprites,lastSprite))		// special case for empty list
					REFGLOBAL(sprites,lastSprite) = sprite;
			}
			else	{
				if (sprite->prevSprite)
					sprite->prevSprite->nextSprite = sprite;
				REFGLOBAL(sprites,lastSprite) = sprite;
				
				if (! REFGLOBAL(sprites,firstSprite))		// special case for empty list
					REFGLOBAL(sprites,firstSprite) = sprite;
			}
			}
		}
	
	if ( err != noErr )
		{
		if ( dec )
			{
			DisposeDecompressor ( dec );
			}
		
		if ( sprite )
			{
			/* dispose of all the hardware sprites we allocated here */
			tile = sprite->tile;
			for ( ii = numTiles - 1; ii >= 0 ; --ii )
				{
				if ( tile->hardwareSprite >= 0 )
					{
					FreeSprite ( tile->hardwareSprite );
					}
	
				tile++;
				}
		
			DisposeMemory ( sprite );
			sprite = 0L;
			}
		}

	return sprite;
}


short
_ExplodeSprite( Sprite *sprite )
{
short *xVelocity;
short *yVelocity;
SpriteInstance *tile;
short index;
short frameIndex;
	
	if( sprite == 0 )
		return noErr;
		
	xVelocity = NewMemory(kTemp, sprite->numTiles * 2 * sizeof(short));
	yVelocity = xVelocity+sprite->numTiles;
	
	if (xVelocity == nil)
		return -1;
	
	PlayDBFX ( kDialogExplodeSnd, 0, 0 );

	for (index=0; index<sprite->numTiles; ++index) {
		xVelocity[index] = (RandomShort() % 8) + 6;
		yVelocity[index] = (RandomShort() % 8) + 6;
		if (RandomShort() & 2)
			xVelocity[index] *= -1;
		if (RandomShort() & 4)
			yVelocity[index] *= -1;
	}
	
	for (frameIndex = 0; frameIndex<24; ++frameIndex)
	{
		long time = GetCurrentTime()+2;
		
		tile = &sprite->tile[0];
		for ( index = 0; index < sprite->numTiles; ++index )
		{
#ifdef SIMULATOR
			// This gets around an apparent bug in the VDP simulator that trashes the heap
			// when the sprite is at extreme x or y coordinates
			
			if (tile->pSprite.hPos > 96 && tile->pSprite.hPos < 448 &&
				tile->pSprite.vPos > 96 && tile->pSprite.vPos < 350)
#endif
			{
			tile->pSprite.hPos += xVelocity[index];
			tile->pSprite.vPos += yVelocity[index];
			tile->spriteInvalid = true;
			}
			++tile;
		}
		DrawSprite((SpriteRef)sprite);
			
		while (time > GetCurrentTime())
			SegaOSIdle( kNonSpecificCode );

	}
	
	DisposeMemory( xVelocity );
	
	return noErr;
}


short
_DisposeSprite ( Sprite * sprite )
{
short	ii;
short	firstTile;
SpriteInstance * tile;
SpriteInstance * prevTile;
SpriteInstance * nextTile;

	if( sprite == 0 )
		return noErr;
		
#ifdef SPRITE_DEBUG			
	printf ( "disposing sprite starting at %d\n", sprite->tile[0].hardwareSprite );
#endif
	
	/* dispose of patterns if they were allocated through the pattern manager */
	if ( PATSIZE(sprite->patternBase) > 0 )
		{
		FreePatternBlock ( sprite->patternBase );
		}
	
	/* If this sprite is using a custom clut, release its reference to the clut */
	if ( sprite->clut > 0 )
		{
		ReleaseClut( sprite->clut );
		}
	if ( sprite->customClutData )
		{
		DisposeMemory( sprite->customClutData );
		}
	
	if ( REFGLOBAL(sprites,lastSprite) == sprite )
		{
#ifdef SPRITE_DEBUG			
		printf ( "killing the last allocated sprite\n" );
#endif
		REFGLOBAL(sprites,lastSprite) = sprite->prevSprite;
		}
		
	DisposeDecompressor ( sprite->decompressor );

	/* free up all the hardware sprites */
	tile = sprite->tile;
	for ( ii = sprite->numTiles - 1; ii >= 0 ; --ii )
		{
#ifdef SPRITE_DEBUG			
		printf ( "\tdisposing sprite tile %d\n", tile->hardwareSprite );
#endif
		FreeSprite ( tile->hardwareSprite );
		tile++;
		}
	
	/* fix up the hardware sprite list */
	nextTile = 0;
	if (sprite->nextSprite)
		nextTile = GetSpriteFirstTile ( sprite->nextSprite );
		
	/* link the sprites before and after us in vdp space */
	if ( sprite->prevSprite )
		{
		prevTile = GetSpriteLastTile ( sprite->prevSprite );
		
		/* assume we're at the end of the list, otherwise grab the next hardware sprite */
		prevTile->pSprite.link = 0;
		if ( nextTile )
			{
#ifdef SPRITE_DEBUG			
			printf ( "for sprite %lx:\n  collapsing sprite %lx to sprite %lx", sprite,
				sprite->prevSprite, sprite->nextSprite );
			printf ( "linking previous sprite %d to %d\n", prevTile->hardwareSprite,
				nextTile->hardwareSprite );
#endif
			prevTile->pSprite.link = nextTile->hardwareSprite;
			}
		WriteSpriteToVDP ( prevTile );
		}

	/* assume that we will kill all tiles in the sprite below */
	firstTile = 0;

	/* update the head of the list if we were it */
	if ( REFGLOBAL(sprites,firstSprite) == sprite )
		{
		/* we're at the head of the list, so we need to update sprite 0 to point at the next */
		/* sprite in the sprite list */
		/* reuse tile 0 in our old sprite */
		tile = &sprite->tile [ 0 ];
		
		/* put the sprite in the upper left corner to hide it */
		tile->hardwareSprite = 0;
		tile->pSprite.hPos = 1;
		tile->pSprite.vPos = 0;
		tile->pSprite.link = 0;
		
		/* if we have a next tile, point to it, otherwise point at the end of the list */
		if ( nextTile )
			{
			tile->pSprite.link = nextTile->hardwareSprite;
			}
		WriteSpriteToVDP ( tile );
		
#ifdef SPRITE_DEBUG			
		printf ( "setting head of sprite list to %lx\n", sprite->nextSprite );
#endif
		REFGLOBAL(sprites,firstSprite) = sprite->nextSprite;
		
		/* zero out all tiles after the first one in our sprite */
		firstTile = 1;
		}
	
	/* now go through and clear all tiles from firstTile in this sprite */
	tile = &sprite->tile [ firstTile ];
	for ( ; firstTile < sprite->numTiles; ++firstTile )
		{
		tile->pSprite.hPos = 1;
		tile->pSprite.vPos = 0;
		tile->pSprite.link = 0;
		tile->pSprite.priority = 0;
		tile->pSprite.palette = 0;
		tile->pSprite.hFlip = 0;
		tile->pSprite.vFlip = 0;
		tile->pSprite.patternBase = 0;
		WriteSpriteToVDP ( tile );
		tile++;
		}
		
	/* remove this sprite from the list */
	if ( sprite->prevSprite )
		{
		sprite->prevSprite->nextSprite = sprite->nextSprite;
		}
	
	if ( sprite->nextSprite )
		{
		sprite->nextSprite->prevSprite = sprite->prevSprite;
		}
		
	DisposeMemory ( sprite );
	
	return noErr;
}

Sprite *_CreateTextSprite( segaCharRect *textRect, long patternStart )
{
SegaBitMap	*bitmapData;
Sprite		*textSprite;
short		height = textRect->bottom - textRect->top;
short		spriteHSize, spriteVSize;
	
	if (patternStart == 0)
		patternStart = NewPatternBlock((textRect->right - textRect->left)*height);
		
	if (patternStart == 0)	{
		WARNING_MESG("CreateTextSprite failing, no patt mem");
		return nil;
	}
		
	bitmapData = CreateSpriteData( textRect, PATOFFSET(patternStart), nil );
	textSprite = (Sprite *)CreateSpriteInFront( bitmapData, 0 );
	textSprite->patternBase = patternStart;
	FigureTileSize(&height);
	
	spriteHSize = textSprite->tile[0].pSprite.hSize;
	spriteVSize = textSprite->tile[0].pSprite.vSize;
	textRect->right =	textRect->left +								
						(textRect->right-textRect->left+spriteHSize) /
						(spriteHSize+1)*(spriteHSize+1);
	textRect->bottom = 	textRect->top + 
						(textRect->bottom-textRect->top+spriteVSize) /
						(spriteVSize+1)*(spriteVSize+1);	
	SetupTextSpriteGDevice( textRect, patternStart, height+1);
	
	return textSprite;
}


Sprite *_CreateTextSpriteFromBitmap( SegaBitMap *bitmapData )
{
	Sprite			*textSprite;
	short			height = bitmapData->height;
	short			width = bitmapData->width;
	short			spriteHSize, spriteVSize;
	segaCharRect	textDeviceRect;
	
	
	textSprite = (Sprite *)CreateSpriteInFront( bitmapData, 0 );
	SetDecompressorOptions( textSprite->decompressor, kOmitTransparentCells );
	
	spriteHSize = textSprite->tile[0].pSprite.hSize;
	spriteVSize = textSprite->tile[0].pSprite.vSize;
	textDeviceRect.left = textDeviceRect.top = 0;
	textDeviceRect.right = textDeviceRect.left + (width+spriteHSize) / (spriteHSize+1)*(spriteHSize+1);
	textDeviceRect.bottom = textDeviceRect.top + (height+spriteVSize) / (spriteVSize+1)*(spriteVSize+1);	

	FigureTileSize(&height);
	SetupTextSpriteGDevice( &textDeviceRect, textSprite->patternBase, height+1);
	
	return textSprite;
}

short
_MoveSprite ( Sprite * sprite, short x, short y )
{
short	ii;
SpriteInstance * tile;
short	xBase;
short	yBase;

	if( sprite != 0 )
	{
		tile = sprite->tile;
		xBase = tile->pSprite.hPos;
		yBase = tile->pSprite.vPos;
		
		for ( ii = sprite->numTiles - 1; ii >= 0; --ii )
		{
			tile->pSprite.hPos = x + ( tile->pSprite.hPos - xBase );
			tile->pSprite.vPos = y + ( tile->pSprite.vPos - yBase );
			tile->spriteInvalid = true;
			tile++;
		}
	}
	
	return noErr;
}


short
_DrawSprite ( Sprite * sprite )
{
short	ii;
SpriteInstance * tile;

	if( sprite != 0 )
	{
		/* if the sprite pattern data is invalid, decompress the next frame */
		if ( sprite->patternInvalid )
			{
			DecompressFrame ( sprite->decompressor, sprite->curFrame );
			sprite->patternInvalid = false;
			}
	
		tile = &sprite->tile[ sprite->numTiles - 1 ];
		for ( ii = 0; ii < sprite->numTiles; ++ii )
			{
	
			/* if the hardware sprite info is invalid, write it out */
			if ( tile->spriteInvalid )
				{
				tile->spriteInvalid = false;
				WriteSpriteToVDP ( tile );
				}
			--tile;
			}
		
		if ((REFGLOBAL(sprites,firstSprite) == sprite) && (sprite->tile[0].hardwareSprite != 0))
		{
			SpriteInstance bogus = sprite->tile[0];
			
			bogus.hardwareSprite = 0;
			bogus.pSprite.vPos = 0;
			bogus.pSprite.hPos = 0;
			bogus.pSprite.link = sprite->tile[0].hardwareSprite;
			WriteSpriteToVDP ( &bogus );
		}
		
		VDPIdle();
	}
	return noErr;
}

short
_SetSpriteImage(Sprite *sprite, SegaBitMap *image)
{
	if( sprite != 0 )
	{
		SetDecompressorImage( sprite->decompressor, image);
		sprite->patternInvalid = true;
	}
	return noErr;
}

short
_IncrementSpriteFrame ( Sprite * sprite, short increment )
{
short oldFrame;

	if( sprite != 0 )
	{
		oldFrame = sprite->curFrame;
		sprite->curFrame = ( oldFrame + increment ) % sprite->frames;
		if ( oldFrame != sprite->curFrame )
		{
			sprite->patternInvalid = true;
		}
	}
	return noErr;
}


short
_SetSpriteFrame ( Sprite * sprite, short frame )
{
short oldFrame;

	if( sprite != 0 )
	{
		oldFrame = sprite->curFrame;
		sprite->curFrame = frame % sprite->frames;
		if ( oldFrame != sprite->curFrame )
		{
			sprite->patternInvalid = true;
		}
	}
	
	return noErr;
}


short
_GetSpriteFrame ( Sprite * sprite, short * frame )
{
	if( sprite != 0 )
		*frame = sprite->curFrame;
	else
		*frame = 0;
		
	return noErr;
}


short
_FlipSprite ( Sprite * sprite, Boolean hFlip, Boolean vFlip )
{
short	ii;
SpriteInstance * tile;

	if( sprite != 0 )
	{
		tile = sprite->tile;
		for ( ii = sprite->numTiles - 1; ii >= 0; --ii )
		{
			/* do the h flip thing */
			if ( hFlip )
			{
				tile->pSprite.hFlip = 1;
			}
			else
			{
				tile->pSprite.hFlip = 0;
			}
			
			/* do the vflip thing */
			if ( vFlip )
			{
				tile->pSprite.vFlip = 1;
			}
			else
			{
				tile->pSprite.vFlip = 0;
			}
	
			tile->spriteInvalid = true;
			tile++;
		}
	}
		
	return noErr;
}


SegaBitMap *
_CreateSpriteData ( segaCharRect *myRect, unsigned short basePattern, short *clut )
{
SpriteDeviceData * 	data;

	data = NewMemory ( kTemp, sizeof(SpriteDeviceData) );
	ASSERT_MESG( data, "Can't create sprite data" );
	if ( data )
		{
		data->bitmap.width = myRect->right - myRect->left;
		data->bitmap.height = myRect->bottom - myRect->top;
		data->bitmap.frames = 1;
		data->bitmap.compressType = kVRamData;
		data->patternBase = basePattern;
		
		if (clut)
		{
			short index;
			
			for (index=0; index<16; ++index)
				data->clut[index] = clut[index];
		}
		else
			data->clut[0] = -1;
		}
	
	return (SegaBitMap *) data;
}


/*
* copy one of our physical sprites to vram
*/
short	_WriteSpriteToVDP ( SpriteInstance * sprite )
{
Ptr					address;
	
	/* get the vram address to write to */
	address = (Ptr) (unsigned short)REFGLOBAL(sprites,spriteTableBase);
	address += sprite->hardwareSprite * sizeof(HardwareSprite);
	
	CopyToVRAM ( (Ptr) &sprite->pSprite, address, sizeof(HardwareSprite) >> 1 );
	
#ifdef FLICKER_DEBUG
	VDPIdle();
#endif

	return noErr;
}


short	_FigureTileSize ( short * width )
{
short tileSize;

	/* this returns the apropriate values for the sega hardware, which are zero based */
	if ( *width >= 4 )
		{
		tileSize = ( *width + 3 ) >> 2;
		*width = 3;  
		}
	else
	if ( *width >= 3 )
		{
		tileSize = ( *width + 2 ) / 3;
		*width = 2;  
		}
	else
	if ( *width >= 2 )
		{
		tileSize = ( *width + 1 ) >> 1;
		*width = 1;  
		}
	else
		{
		tileSize = 1;
		*width = 0;
		}
		
	return tileSize;
}


/*
* allocate the next free sprite. NEVER allocate sprite #0
*/
short	_AllocateSprite ( )
{
unsigned char	byte;
unsigned char	bitMask;
short			sprite;
unsigned char *	map;
	
	sprite = 1;			// skipping sprite #0  (always reserved as a head pointer)
	bitMask = 0x40;
	
	map = &REFGLOBAL(sprites,spriteMap) [ 0 ];
	byte = *map;

	while ( sprite < kMaxSprite )
		{
		/* check for a free sprite */
		if ( ! ( bitMask & byte ) )
			{
			/* we found one */
			break;
			}
			
		bitMask >>= 1;
		if ( bitMask == 0 )
			{
			/* read the next byte (actually the previous byte) */
			byte = *++map;
			bitMask = 0x80;
			}
			
		++sprite;
		}
	
	/* if we found one, mark it as used */
	if ( sprite < kMaxSprite )
		{
		*map |= bitMask;
		}
	else
		sprite = -1;			// -1 means failure
	
	return sprite;
}


/*
* free sprite number "sprite". it's the caller's responsibility to fix up the sprite link
* list.
*/
short	_FreeSprite ( short sprite )
{
short			byte;
unsigned char	bitMask;

	byte = sprite >> 3;
	bitMask = 0x80 >> ( sprite & 0x7 );
	
	REFGLOBAL(sprites,spriteMap)[ byte ] &= ~bitMask;
	
	return noErr;
}


SpriteInstance * _GetSpriteLastTile ( Sprite * sprite )
{
SpriteInstance * instance;

	ASSERT(sprite);
	instance = 0;
	if( sprite != 0 )
		instance = &sprite->tile[ sprite->numTiles - 1 ];
	
	return instance;
}


SpriteInstance * _GetSpriteFirstTile ( Sprite * sprite )
{
SpriteInstance * instance;

	ASSERT(sprite);
	instance = 0;
	if( sprite != 0 )
		instance = &sprite->tile[ 0 ];

	return instance;
}

short _SetSpritePalette( Sprite *sprite, short palette )
{
SpriteInstance *tile;
short index;
	
	if( sprite != 0 )
	{
		tile = sprite->tile;
		sprite->clut = palette;
		
		for ( index = sprite->numTiles - 1; index >= 0; --index )
		{
			tile->pSprite.palette = palette;
			tile++;
		}
	}
	
}


/* This tells a sprite to draw itself in grayscale using the default clut. This is used both
 * as an effect and also to preserve cluts when many custom cluts are in use. Any sprite,
 * regardless of which clut it uses, can be drawn in grayscale with the default clut.
 */
short _SetSpriteGrayFlag( Sprite *sprite, Boolean grayFlag )
{
char grayMapping[16];
	
	if( sprite == 0 )
		return noErr;

	if (grayFlag)
	{				
		/* If the sprite is using a custom clut, make it use the default clut */
		if (sprite->customClutData)
		{
			SpriteInstance *tile;
			short ii;
			
			ReleaseClut(sprite->clut);								// Free the custom clut
			sprite->clut = 0;										// Use the default
			SetDecompressorPalette(sprite->decompressor, 0);
			GenerateGrayMap(sprite->customClutData, grayMapping);	// Figure out the grayscale stuff
			SetDecompressorPixelMapping(sprite->decompressor, grayMapping);
			
			/* Update the hardware sprites to refer to the default palette */
			tile = &sprite->tile[ sprite->numTiles - 1 ];
			for ( ii = 0; ii < sprite->numTiles; ++ii )
			{
				tile->pSprite.palette = 0;			// default
				tile->spriteInvalid = true;
				--tile;
			}
		}
		else
		{
			short *defaultClut;
			
			/* The sprite is already using the default clut, just figure out the grayscale stuff */
			defaultClut = (short *) DBGetItem ( kClutType, kZeusClut );
			GenerateGrayMap(defaultClut, grayMapping);
			SetDecompressorPixelMapping(sprite->decompressor, grayMapping);
		}
	}
	else
	{
		/* Tell the decompressor to draw in color */
		SetDecompressorOptions( sprite->decompressor, 0 );
		
		/* If we're already in color, just bail */
		if (sprite->clut)
			return noErr;
			
		/* If the sprite was using a custom clut, restore its clut data */
		if (sprite->customClutData)
		{
			SpriteInstance *tile;
			short ii;
			
			sprite->clut = RequestClut(sprite->customClutData);				// get a custom clut
			SetDecompressorPalette(sprite->decompressor, sprite->clut);
			SetDecompressorPixelMapping(sprite->decompressor, nil);
			
			/* Update the hardware sprites to refer to the custom clut */
			tile = &sprite->tile[ sprite->numTiles - 1 ];
			for ( ii = 0; ii < sprite->numTiles; ++ii )
			{
				tile->pSprite.palette = sprite->clut;
				tile->spriteInvalid = true;
				--tile;
			}
		}
		else
		{
			/* The sprite was using the default palette, just kill the grayscale mapping */
			SetDecompressorPixelMapping(sprite->decompressor, nil);
		}
	}
	
	sprite->patternInvalid = true;
	return noErr;
}

/* This sets a cell's position. It's used pretty heavily by dialog dismissal */
short _SetSpriteTilePosition( Sprite *sprite, short tile, short x, short y )
{
SpriteInstance *tilePtr;

	ASSERT_MESG ( tile < sprite->numTiles, "Illegal tile number." );
	
	if( sprite != 0 )
	{
		tilePtr = &sprite->tile[tile];		/* find the tile we're interested in */
		if (tilePtr->pSprite.hPos != x || tilePtr->pSprite.vPos != y) 
		{
			tilePtr->pSprite.hPos = x;			/* move it */
			tilePtr->pSprite.vPos = y;
			tilePtr->spriteInvalid = true;		/* so it redraws */
		}
	}
	return noErr;
}

static long UpdateSparkImage( long time, long data )
{
Spark *theSpark	= (Spark *)data;
short iii;
register short *imageWalker;

	if( data == 0 )
		return GetCurrentTime()+10;

 	// don't animate if animations are on hold

	if (! (theSpark->flags & kSparkOKInDialogs))
		if (GetAnimationSuspendLevel() >= kDialogAnimationPriority)
			return GetCurrentTime()+10;
			
	// if we're a one-shot, kill ourselves AFTER displaying last frame
			
	if ( theSpark->flags & kSparkOneShot )
	{
		if ( theSpark->sparkFrame == 7 )	// one-shots go 0..6 and stop on 7
		{
			MoveSprite( (SpriteRef) theSpark->sparkSprite, 100, 100 );		// hide it
			DrawSprite( (SpriteRef) theSpark->sparkSprite );
			StopSpark( (SparkRef) theSpark );				// stop animate
			return GetCurrentTime()+10;
		}
	}
	else
		theSpark->sparkFrame %= 6;	// free-running sparks loop from 0 thru 5
	
	switch ( theSpark->sparkFrame )
	{
		case 0 :
		case 6 :							// 6 only reached on one-shots
			imageWalker = theSpark->frame0;
			break;
		case 1 :
		case 5 :
			imageWalker = theSpark->frame1;
			break;
		case 2 :
		case 4 :
			imageWalker = theSpark->frame2;
			break;
		case 3 :
			imageWalker = theSpark->frame3;
			break;
	}

	OwnVDP();
	
	SetVDPIncrement( 2 );
	SetVDPVRAMAddress( (unsigned long) PATOFFSET(theSpark->sparkPattern) << 5 );
	
	for (iii=0;iii<16;iii++)
		WriteWordToVDP( *imageWalker++ );
	
	FreeVDP();

	theSpark->sparkFrame += 1;		// advance to next frame
	
//	if (theSpark->sparkFrame & 1)
		VDPIdle();

	
	return GetCurrentTime() + 10;
}

Spark *_NewSpark( short color )
{
Spark *theSpark;
long	patternMem;

	patternMem = NewPatternBlock( 1 );

	if( patternMem == 0 )
		return 0;

	theSpark = NewMemory( kTemp, sizeof(Spark) );
	ASSERT(theSpark);

	theSpark->sparkPattern = patternMem;
	theSpark->flags = color;
	color &= 15;

	{
	segaCharRect	sparkRect;
	SegaBitMap		*sparkBM;
	short c;
	short c0;
	short c00;
	short c000;
	short C;
	short C0;
	short C00;
	short C000;
	register short *imageWalker;


		/* draw out the frames used by the spark */
		C = color;		/* light color */
		C0 = C << 4;
		C00 = C0 << 4;
		C000 = C00 << 4;
	
		c = color + 1;	/* dark color */
		c0 = c << 4;
		c00 = c0 << 4;
		c000 = c00 << 4;
		
		imageWalker = theSpark->frame0;

		/* frame 0 */
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;
		*imageWalker++ = c;					*imageWalker++ = 0x0000;
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = c;					*imageWalker++ = 0x0000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;			
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;

		/* frame 1 */
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;
		*imageWalker++ = c;					*imageWalker++ = 0x0000;
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = c00+C0+C;			*imageWalker++ = C000 + c00;
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = c;					*imageWalker++ = 0x0000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;

		/* frame 2 */
		*imageWalker++ = c;					*imageWalker++ = 0x0000;
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = c00+C0+C;			*imageWalker++ = C000 + c00;
		*imageWalker++ = c000+C00+C0+1;		*imageWalker++ = C000+C00+c0;
		*imageWalker++ = c00+C0+C;			*imageWalker++ = C000 + c00;
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = c;					*imageWalker++ = 0x0000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;

		/* frame 3 */
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = c00+C0+C;			*imageWalker++ = C000 + c00;
		*imageWalker++ = c000+C00+C0+1;		*imageWalker++ = C000+C00+c0;
		*imageWalker++ = C000+C00+0x10+1;	*imageWalker++ = 0x1000+C00+C0;
		*imageWalker++ = c000+C00+C0+1;		*imageWalker++ = C000+C00+c0;
		*imageWalker++ = c00+C0+C;			*imageWalker++ = C000 + c00;
		*imageWalker++ = c0+C;				*imageWalker++ = c000;
		*imageWalker++ = 0x0000;			*imageWalker++ = 0x0000;
	
		sparkRect.top = sparkRect.left = 0;
		sparkRect.bottom = 1;
		sparkRect.right = 1;

		sparkBM = CreateSpriteData( &sparkRect, theSpark->sparkPattern, nil );
		
		if ( theSpark->flags & kDrawSparkInFront )
			theSpark->sparkSprite = (Sprite *) CreateSpriteInFront( sparkBM, 0 );
		else
			theSpark->sparkSprite = (Sprite *) CreateSprite( sparkBM );
		theSpark->sparkFrame = 0;
		UpdateSparkImage( 0, (long)theSpark );
		theSpark->sparkProc = AddTimeRequest( UpdateSparkImage, (long)theSpark );
	}
	
	return (theSpark);
}

void _DisposeSpark( Spark *spark )
{		
	ASSERT_MESG(spark, "there is no more spark left!");

	if( spark != 0 )
	{
		if (spark->sparkProc) 
		{
			RemoveTimeRequest( spark->sparkProc );
		}
		DisposeSprite( (SpriteRef) spark->sparkSprite );
		FreePatternBlock( spark->sparkPattern );
		DisposeMemory( spark );
	}
}

Sprite *_GetSparkSprite( Spark *spark )
{
	ASSERT_MESG(spark, "there is no more spark left!");
	if( spark == 0 )
		return 0;
	else
		return (spark->sparkSprite);
}

void _StartSpark( Spark *spark )
{
	if( spark == 0 )
		return;
		
	ASSERT_MESG(spark, "there is no more spark left!");
	
	spark->sparkFrame = 0;		// reset the animation

	if (spark->sparkProc == nil) 
	{
		UpdateSparkImage( 0, (long)spark );
		spark->sparkProc = AddTimeRequest( UpdateSparkImage, (long)spark );		
	}
}

void _StopSpark( Spark *spark )
{
	ASSERT_MESG(spark, "there is no more spark left!");

	if( spark == 0 )
		return;
		
	if (spark->sparkProc) 
	{
		RemoveTimeRequest( spark->sparkProc );
		spark->sparkProc = nil;
	}
}

SyncOTronData * _SyncOTron( SyncOTronData * theLogo, short delta )
{
SyncOTronData	*result;
SegaBitMapPtr	myGraphic;
short			iii;
short			xBase = 237;
short			yBase = 172;
SpriteInstance * tile;
segaCharRect 	myRect;
long			patternStart;
char			*myText;

//
// check if it's the beginning of SyncOTron
//
	if( theLogo != 0 )
		result = theLogo;
	else
	{
		result = NewMemory( kTemp, sizeof( SyncOTronData ) );
		result->drawnZeroDelta = false;
		
		/* read in the X */
		for (iii=0; iii<8; iii++)
		{
			myGraphic = GetDBGraphics( kFirstXFrame + iii );
			result->sprites[iii] = CreateSpriteInFront( myGraphic, 0 );
			MoveSprite( result->sprites[iii], xBase + myGraphic->xPos, yBase + myGraphic->yPos );
		}

		SetCurFont( kMainFont );
		myRect.left = 4;
		myRect.top = 4;
		myRect.right = 36;
		myRect.bottom = 6;
		
		SetCurrentDevice( kScrollA );
		patternStart = LinearizeScreenArea( &myRect, 0 );
		SetupTextGDevice( &myRect, patternStart );
		SetFontColors( 0, 13, 12, 0 );

		result->topTextPatternMems = patternStart;
		myText = GetWriteableString( kSyncoTronString );
		CenterSegaText( 0, myText );

		myRect.left = 4;
		myRect.top = 21;
		myRect.right = 36;
		myRect.bottom = 24;

		patternStart = LinearizeScreenArea( &myRect, 0 );
		SetupTextGDevice( &myRect, patternStart );
		result->bottomTextPatternMems = patternStart;
		myText = GetWriteableString( kPatentPendingString );
		SetFontColors( 0, 13, 12, 0 );
		CenterSegaText( 6, myText );

	}	

	if( delta == -1 )
	{
		/* trash the sprites */
		for ( iii=0; iii < 13; iii++)
			DisposeSprite( result->sprites[iii] );
		
		FreePatternBlock( result->bottomTextPatternMems );
		FreePatternBlock( result->topTextPatternMems );

		DisposeMemory( theLogo );

		result = 0;
	}
	else
	{
		for( iii = 0; iii < 8; iii++ )
		{
			tile = ((Sprite *) result->sprites[iii])->tile;
			xBase = tile->pSprite.hPos;
			myGraphic = GetDBGraphics( kFirstXFrame + iii );
			yBase = myGraphic->yPos + 172;
			if( iii & 0x02 )
				MoveSprite( result->sprites[iii], xBase, yBase + delta );
			else
				MoveSprite( result->sprites[iii], xBase, yBase - delta );

		}

		for ( iii = 0; iii < 8; iii++ )
			DrawSprite( result->sprites[iii] );
	}
	
	if( delta == 0 )
	{
		/* make sure we haven't done this already */
		if ( !result->drawnZeroDelta )
		{
			/* Read in the BAND™ */
			for (iii=0;iii<5;iii++)
			{
				DBID	theID;
				
				if ( iii != 1 )
					theID = iii + kLetterB;
				else
					theID = kFirstAFrame + 15;
				
				myGraphic = GetDBGraphics( theID );
				result->sprites[iii+8] = CreateSpriteInFront( myGraphic, 0 );
				MoveSprite( result->sprites[iii+8], myGraphic->xPos, myGraphic->yPos );
				DrawSprite( result->sprites[iii+8] );
			}
			result->drawnZeroDelta = true;
		}
	}

	return result;		

}

XBandLogo *_DrawXBandLogo( void )
{
XBandLogo		*result;
SegaBitMapPtr	myGraphic;
short			iii;
short			sparkColor;
SparkRef		theSparkRef;
short			xBase = 237;
short			yBase = 172;

	result = NewMemory( kTemp, sizeof( XBandLogo ) );

	/* read in the X */
	for (iii=0; iii<8; iii++)
	{
		myGraphic = GetDBGraphics( kFirstXFrame + iii );
		result->sprites[iii] = CreateSpriteInFront( myGraphic, 0 );
		MoveSprite( result->sprites[iii], xBase + myGraphic->xPos, yBase + myGraphic->yPos );
	}
	
	/* Read in the BAND™ */
	for (iii=0;iii<5;iii++)
	{
		DBID	theID;
		
		if ( iii != 1 )
			theID = iii + kLetterB;
		else
			theID = kFirstAFrame + 15;
		
		myGraphic = GetDBGraphics( theID );
		result->sprites[iii+8] = CreateSpriteInFront( myGraphic, 0 );
		MoveSprite( result->sprites[iii+8], myGraphic->xPos, myGraphic->yPos );
	}
	
	for ( iii=0;iii<kXBandLogoSprites;iii++ )
		DrawSprite( result->sprites[iii] );
	
	/* setup spark graphics */
	sparkColor = 8;

	for (iii=0;iii<kXBandLogoSparks;iii++)
	{
		theSparkRef = NewSpark( sparkColor );
		
		result->theAnimatorData[iii].bitmap = 0;
		result->theAnimatorData[iii].path = kVectorPath;
		result->theAnimatorData[iii].bmFrameDelay = 0x7fffffff;
		result->theAnimatorData[iii].pathFrameDelay = 3;
		result->theAnimatorData[iii].startPoint.h = 128;
		result->theAnimatorData[iii].startPoint.v = 128;
		result->theAnimatorData[iii].stopPoint.h = 128;
		result->theAnimatorData[iii].stopPoint.v = 128;
		
		sparkColor += 2;
		
		if (sparkColor > 14)
			sparkColor = 8;
		
		if (theSparkRef)
		{
			result->sparks[iii] = theSparkRef;
			result->theAnimatorData[iii].animatorData = (long) GetSparkSprite( theSparkRef );
			result->theAnimatorData[iii].pathData = (2 << 8) + (iii + kXBandLogoFirstAnimationID);
			result->animations[iii] = SpawnAnimation( kTheAnimator, (long) &result->theAnimatorData[iii] );
			
			if ( result->animations[iii] ) {
				StartAnimation( result->animations[iii] );
			} else {
				DisposeSpark( result->sparks[iii] );
				result->sparks[iii] = 0;
			}
			
		}
		else
		{
			result->sparks[iii] = 0;
			result->animations[iii] = 0;
		}
	}

	return result;
}

void _DisposeXBandLogoSparks( XBandLogo *mems )
{
short iii;

	if ( mems )
	{
		/* trash the spark animations and sparks */
		for ( iii=0; iii<kXBandLogoSparks; iii++ )
		{
			if ( mems->animations[iii] )
			{
				DisposeAnimation( mems->animations[iii] );
				mems->animations[iii] = (AnimationRef) 0L;
			}
			if ( mems->sparks[iii] )
			{
				DisposeSpark( mems->sparks[iii] );
				mems->sparks[iii] = (SparkRef) 0L;
			}
		}
	}
}

void _DisposeXBandLogoRef( XBandLogo *mems )
{
short iii;

	if ( mems )
	{
		DisposeXBandLogoSparks( (XBandLogoRef) mems );

		/* trash the sprites */
		for ( iii=0; iii<kXBandLogoSprites; iii++)
			DisposeSprite( mems->sprites[iii] );

		/* trash the memory itself */
		DisposeMemory( (Ptr) mems );
	}
}

#ifdef	ROMDB
#undef	SIMULATOR
#endif


#if (defined(ROMDB) || defined(SIMULATOR))

// not vectorized - goes in ROM DB
static
long SpriteDeviceDecompressor ( short command, long data, long * ref )
{
short			err;

	err = commandSelectorUnknown;
	
	switch ( command )
		{
		case kInitDecompressor:
			err = noErr;
			*ref = data;
			break;
		
		case kSetDecompressorPalette:
			err = noErr;
			break;
			
		case kSetDecompressorDstPattern:
			/* we ignore this message */
			return data;
			break;
			
		case kSetDecompressorFrame:
			/* we ignore this message */
			err = noErr;
			break;
			
		case kDecompressAFrame:
			err = noErr;
			break;
			
		case kFlushFrameState:
			err = noErr;
			break;
		
		case kSetDecompressorTiling:
			/* we don't supprt this yet */
			err = noErr;
			break;

		case kSetDecompressorOptions:
			err = noErr;
			break;

		case kResetDecompressorOptions:
			err = noErr;
			break;

		case kDisposeDecompressorPatterns:
			err = noErr;
			break;
			
		case kGetDecompressorImageCache:
			err = noErr;
			break;
			
		case kReleaseDecompressorImageCache:
			err = noErr;
			break;

		case kGetDecompressorClut:
		{
			SpriteDeviceData *sprite = (SpriteDeviceData *) *ref;
			
			if (sprite->clut[0] != -1)
			{
				short index;
				
				for (index=0; index<16; ++index)
					((short *)data)[index] = sprite->clut[index];
				
				err = noErr;
			}
			
			break;
		}
		
			
		case kCloseDecompressor:
			{
			SpriteDeviceData *data = (SpriteDeviceData *)*ref;
			
			DisposeMemory( (Ptr) data );
			err = noErr;
			break;
			}
	
		}
	
	return err;
}



//  NOT vectorized because ONLY called from simulator

Ptr
GetSpriteDeviceDecompressPtr ( void )
{
long size;
Ptr data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) SpriteDeviceDecompressor + 2));
	size = (*(long *) ((Ptr) GetSpriteDeviceDecompressPtr + 2)) - srcFunction;

	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for sprite device decompressor proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) SpriteDeviceDecompressor;
#else
#ifdef ROMDB
		CheckCodeForJTRefs((void *)SpriteDeviceDecompressor, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
#endif
		}
	
	return data;
}

#endif			// (defined(ROMDB) || defined(SIMULATOR))



