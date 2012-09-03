/*
	File:		MasterDictDecompress.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<14>	  8/7/94	CMY		Use the first pattern cell for zero.
		<13>	  8/7/94	CMY		Fix decompression of the X buttons.
		<12>	  8/2/94	CMY		Added support for allocating patterns high.
		<11>	 7/31/94	CMY		Added missing break to a switch statement.
		<10>	 7/30/94	KON		Fix merge problem.
		 <9>	 7/30/94	KON		Add support for inFront decompressor option.
		 <8>	 7/30/94	CMY		Moved caching strategy out of the decompressor and into the
									caller's control.
		 <7>	 7/29/94	CMY		Added the pixelMapping case for the decompressor.
		 <6>	 7/29/94	KON		Add support for setting priority bit.
		 <5>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		 <4>	 7/20/94	CMY		When decompressing to a scroll plane, we were trashing 1 cell
									past the end of our pattern mems
		 <3>	 7/19/94	CMY		Now supports the kSetDecompressorImageData opcode for animation
		 <2>	 7/18/94	CMY		RLE compression for the pattern data. Fixed bugs in the
									decompress to bitmap case.
		 <1>	 7/17/94	CMY		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "SegaOS.h"
#include "Decompress.h"
#include "Decompressors.h"
#include "Heaps.h"
#include "Reliability.h"
#include "PatchDBHeaps.h"
#include "Errors.h"
#include "SegaVDP.h"
#include "SegaScrn.h"
#include "Sprites.h"
#include "Pattern.h"
#include "MungeCode.h"
#include "CompressDictionary.h"
#include "DBTypes.h"
#include "SegaBM.h"

#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif


/*
* MasterDict decompressor types
*/


typedef
struct MasterDictDecompressState
{
	DictionaryBitMap *compressedData;
	short			palette;
	short			curFrame;
	long			dstPatternID;
	short			tileWidth;
	short			tileHeight;
	short			top;
	short			left;
	Boolean			inFront;
	Boolean			omitTransparentCells;
	char			*pixelMapping;
	short			*patternDictionary;
} MasterDictDecompressState;


static	long	MasterDictDecompress ( short command, long data, long * ref );
static	short	DoMDDecompress ( MasterDictDecompressState * state );
static	short	MDDecompressToSprite ( MasterDictDecompressState * state );
static	short	MDDecompressToBitmap ( MasterDictDecompressState * state );


static
long MasterDictDecompress ( short command, long data, long * ref )
{
short err;
MasterDictDecompressState *state;

	err = commandSelectorUnknown;
	
	switch ( command )
		{
		case kInitDecompressor:
			state = NewMemory ( false, sizeof(MasterDictDecompressState) );
			if ( state )
				{
				err = noErr;
				*ref = (long) state;
				
				state->compressedData = (DictionaryBitMap *) data;
				state->palette = 0;
				state->curFrame = 0;
				state->dstPatternID = 0;
				state->tileWidth = state->compressedData->bitMap.width;
				state->tileHeight = state->compressedData->bitMap.height;
				state->top = 0;
				state->left = 0;
				state->inFront = 0;
				state->omitTransparentCells = 0;
				state->pixelMapping = nil;
				state->patternDictionary = nil;
				}
			break;
		
		case kSetDecompressorImageData:
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			state->compressedData = (DictionaryBitMap *) data;
			break;
			
		case kSetDecompressorPalette:
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			state->palette = data;
			break;

		case kSetDecompressorDstPattern:
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			if ( data == 0 || data == -1)
				{
				Boolean high = data == -1;
				
				data = state->compressedData->uniqueCells;
					
				if( data == 0 )
					{
					data = state->compressedData->bitMap.width * state->compressedData->bitMap.height;
					}
				
				ASSERT(data);
				if (high)
					data = NewPatternBlockHigh( data );
				else
					data = NewPatternBlock( data );			// should this just warn?
				}
				
			state->dstPatternID = data;
			
			/* return the pattern id that we use */
			return data;
			break;
			
		case kSetDecompressorFrame:
			err = noErr;			
			state = (MasterDictDecompressState *) *ref;
			state->curFrame = data;
			break;

		case kDecompressAFrame:
			state = (MasterDictDecompressState *) *ref;
			err = DoMDDecompress ( state );
			break;
			
		case kFlushFrameState:
			err = noErr;
			break;
		
		case kSetDecompressorTiling:
			err = noErr;			
			state = (MasterDictDecompressState *) *ref;
			state->tileWidth = data & 0xffff;
			state->tileHeight = data >> 16;
			break;
		
		case kSetDecompressorOrigin:
			err = noErr;			
			state = (MasterDictDecompressState *) *ref;
			state->left = data & 0xffff;
			state->top = data >> 16;
			break;
		
		case kGetDecompressorClut:
		{
			unsigned char *clutDictionary;
			short *clutSource, *dest;
			short index;
			
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			if (state->compressedData->clutRef == -1)
				return 1;		/* Default clut */
			
			clutDictionary = (unsigned char *)DBGetItem( kCLUTDataType, colorTableDictionaryDBID );
			clutSource = (short *)&clutDictionary[state->compressedData->clutRef*32];
			dest = (short *)data;
			
			for (index=0; index<16; ++index)
			{
				*dest++ = *clutSource++;
			}
			return 0;
			break;
		}
		
		case kSetDecompressorOptions:
			err = noErr;			
			state = (MasterDictDecompressState *) *ref;
			state->omitTransparentCells |= ((data & kOmitTransparentCells) != 0);
			state->inFront |= ((data & kDrawGraphicInFront) != 0);
			break;

		case kResetDecompressorOptions:
			err = noErr;			
			state = (MasterDictDecompressState *) *ref;
			state->omitTransparentCells &= ((data & kOmitTransparentCells) != 0 );
			state->inFront &= ((data & kDrawGraphicInFront) != 0);
			break;

		case kGetDecompressorImageCache:
		{
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			GetDictionaryCache( state->compressedData->patternDictionaryRef );
			return (long) state->compressedData->patternDictionaryRef;
			break;
		}
		
		case kReleaseDecompressorImageCache:
		{
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			ReleaseDictionaryCache( (short)data );
			break;
		}
		
		case kSetDecompressorPixelMapping:
		{
			short ind;
			
			state = (MasterDictDecompressState *) *ref;
			if (data)
			{
				state->pixelMapping = NewMemory(kTemp, 16);
				for (ind=0; ind<16; ++ind)
				{
					state->pixelMapping[ind] = ((char *)data)[ind];
				}
			}
			else if (state->pixelMapping)
			{
				DisposeMemory( (Ptr) state->pixelMapping);
				state->pixelMapping = nil;
			}
			break;
		}
		
		case kDisposeDecompressorPatterns:
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			FreePatternBlock ( state->dstPatternID );
			break;
			
		case kCloseDecompressor:
			err = noErr;
			state = (MasterDictDecompressState *) *ref;
			if (state->pixelMapping)
				DisposeMemory ( (Ptr) state->pixelMapping );
			DisposeMemory ( (Ptr) state );
			break;
		}
	
	return err;
}


static
short DoMDDecompress ( MasterDictDecompressState * state )
{
DictionaryBitMap	*image;
SegaErr				err;

	err = noErr;

	image = state->compressedData;
	state->patternDictionary = GetDictionaryCache( image->patternDictionaryRef );
		
	/*** If we ever support multiple frames, stick in the code to find the frame here ***/
	
	/* are we going to a sprite or a bitmap? */
	/* if we're going to a bitmap we can just place the data out to the vdp */
	/* otherwise we need to remap it to suit a sprite */
	
	if ( state->tileWidth != image->bitMap.width
	||	 state->tileHeight != image->bitMap.height )
		{
		err = MDDecompressToSprite ( state );
		}
	else
		{
		err = MDDecompressToBitmap ( state );
		}
	
	ReleaseDictionaryCache( image->patternDictionaryRef );
	return err;
}

#define MapAWordOfPixels(srcWord) 	(	((unsigned short)state->pixelMapping[((srcWord)>>12)&0xF] << 12) |	\
										((unsigned short)state->pixelMapping[((srcWord)>>8)&0xF] << 8) |	\
										((unsigned short)state->pixelMapping[((srcWord)>>4)&0xF] << 4) |	\
										((unsigned short)state->pixelMapping[((srcWord))&0xF]) )
static
short MDDecompressToSprite ( MasterDictDecompressState * state )
{
Ptr 				sourcePtr;
short * 			srcWordPtr;
DictionaryBitMap	*image;
Ptr					frameSrc;
Ptr					srcTileRow;
short				hTiles;
short				vTiles;
short				hTileMax;
short				vTileMax;
long				dstPtr;
short				tileRow;
short				tileCol;
short				words;
short				height;
short				patternColumnBump;
short				tileColumnBump;
short *				cellMap;
Ptr					patternMap;
short				patVCount;
short				uniqueChars;
short				transparentCell = -1;
Boolean				pixelMappingFlag = false;
short				totalChars;
short				zeroRun = 0;
short				*expandedNameTable;

	cellMap = 0L;
	
	if (state->pixelMapping)
		pixelMappingFlag = true;
	
	/* since we need to remap it, the best way to go is to walk the cell data */
	/* and from that determine which patterns to move down */
	
	image = state->compressedData;		
	height = image->bitMap.height;

	expandedNameTable = (short *)NewMemory(kTemp, image->bitMap.height * image->bitMap.width * sizeof(short) );
	if (expandedNameTable == nil)
		return -1;
				
	/* the data is linearly mapped vertically, which doesn't necessarily map */
	/* to how sprites are laid out */
	
	/* how many patterns per block move */
	totalChars = ( state->tileHeight + 1 ) * 16;
	
	/* assume we weren't compressed */
	srcTileRow = (Ptr) image->nameTable;
		
	dstPtr = PATOFFSET(state->dstPatternID);
	dstPtr <<= 5;
	
	OwnVDP();
	
	SetVDPIncrement ( 2 );
	SetVDPVRAMAddress ( dstPtr );
	
	uniqueChars = image->uniqueCells;
	
	/* if we have a cell map (uniqueCells != 0), then we need to walk the map to write */
	/* the patterns, otherwise we walk the pattern map itself */
	if ( uniqueChars )
		{
		/* only write one pattern at a time now */
		totalChars = 16;
		
		tileRow = height * 2;

		}
	else
		{
		tileRow = height * 32;
		patternColumnBump = totalChars * 2;
		tileColumnBump = tileRow * ( state->tileWidth + 1);
		}
	
	patternMap = (Ptr)state->patternDictionary;
	cellMap = (short *) srcTileRow;
	
	{
		short *nameWalker = expandedNameTable;
		short index = image->bitMap.width * image->bitMap.height;
		
		while (index)
		{
			if ((*cellMap & 0xC000) == 0xC000)
			{
				short zeroRun = *cellMap++ & 0x3FFF;
				
				while (zeroRun--)
				{
					*nameWalker++ = 0;
					--index;
				}
			}
			else if ((*cellMap & 0xC000) == 0x8000)
			{
				*nameWalker++ = image->nameTable[(*cellMap++ & 0x3FFF)];
				--index;
			}
			else
			{
				*nameWalker++ = *cellMap++;
				--index;
			}
		}
		
		srcTileRow = (Ptr)expandedNameTable;
		cellMap = expandedNameTable;
		ASSERT(nameWalker-expandedNameTable == image->bitMap.width*image->bitMap.height);
	}
	
	/* Avoid a divide in the main loop */
	hTileMax = image->bitMap.width / ( state->tileWidth + 1)  - 1;

#if DO_OMIT_CELLS	
	if( state->omitTransparentCells ){
		for ( vTiles = height / ( state->tileHeight + 1 ) - 1; vTiles >= 0; --vTiles )
			{
			sourcePtr = srcTileRow;
			for ( hTiles = hTileMax; hTiles >= 0; --hTiles )
				{
				for ( tileCol = state->tileWidth; tileCol >= 0; --tileCol )
					{
					cellMap = (short *) sourcePtr;
					for ( patVCount = state->tileHeight; patVCount >= 0; --patVCount )
						{
						/* point at the pattern for this cell */
						srcWordPtr = (short *) ( patternMap + *cellMap * 32 );
						
						SetVDPVRAMAddress(dstPtr);
						if (transparentCell == -1) {
							short *transparentTest = srcWordPtr;
							
							if (*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++ ||
								*transparentTest++ || *transparentTest++)
							{
								;
							} else {
								transparentCell = *cellMap;
							}
						}
						if (*cellMap == transparentCell)
						{
							dstPtr += totalChars*2;
						} else
							{
							for ( words = totalChars - 1; words >= 0; --words )
								{ short data = *srcWordPtr;
								
								if (pixelMappingFlag)
									{
									WriteWordToVDP( MapAWordOfPix