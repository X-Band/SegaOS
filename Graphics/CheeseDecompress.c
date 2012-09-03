/*
	File:		CheeseDecompress.c

	Contains:	This proc decompresses cheese data into raw data, then calls
				the raw decompressor to write the data to VDP memory.
				It's compressType is '2' since we sometimes get better compression
				just by passing the original '0' format.

	Written by:	Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/12/94	HEC		Renamed errors.
		<14>	  8/2/94	CMY		Added support for allocating patterns high.
		<13>	 7/30/94	CMY		Added (empty) cases for Get/Release DecompressorImageCache.
		<12>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		<11>	 7/26/94	KON		Use constant for kCustomPaletteCompressor.
		<10>	 7/21/94	CMY		Make it support pixelMappings
		 <9>	 7/18/94	CMY		It now asks the decompressor what clut the image wants instead
									of figuring it out by itself.
		 <8>	 7/13/94	ADS		ASSERT a problem case
		 <7>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		 <6>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <5>	  7/3/94	JBH		(Hopefully) sped up the decompressor.
		 <4>	  7/2/94	SAH		Make sure to include all of the decompressor in the db. Also
									fixed a long multiply problem with the sega build.
		 <3>	  7/2/94	JBH		Fixed bugs, fixed memory leak. Should still check memory size
									request.
		 <2>	 6/29/94	HEC		Works better.
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

#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif

typedef unsigned char uchar;

typedef
struct RawData
{
	SegaBitMap	header;
	union {
		struct {
			short		uniqueChars;
			short		charData[1];	// cell data goes here, in-line
			short		frameData[1];	// pattern data goes here
		} defaultPalette;
		struct {
			short		uniqueChars;
			short		palette[16];	// color palette
			short		charData[1];	// cell data goes here, in-line
			short		frameData[1];	// pattern data goes here
		} customPalette;
	} data;
} RawData;

typedef
struct CheeseDecompressState
{
	Ptr				sp;					// source pointer - located at top to make ReadBits faster
	RawData *		sega;
	char			pixelMapping[16];
	short			palette;
	short			startFrame;
	long			dstPatternID;
	short			tileWidth;
	short			tileHeight;
	short			top;
	short			left;
	long			options;
	Decompressor	raw;				// conenction to raw decompressor
	Ptr				rawBuffer;			// temp memory that holds decompressed data
	short			bit;
} CheeseDecompressState;

#define kCellHeight			8
#define kCellWidth			8
#define kCellPixMapSize 	32
#define kCellIndexSize	 	2

static long	CheeseDecompress ( short command, long data, long * ref );
static short DoCheese( CheeseDecompressState * state );
static long	ReadBits( CheeseDecompressState *state, long size, long rightMask );
static void	AlignBits( CheeseDecompressState *state);
static short DoDecompress ( CheeseDecompressState * state );
static long BitsToEncode(long val);
//static unsigned long * GetRightMask ( void );
static long Read1Bit( CheeseDecompressState *state );


static
long CheeseDecompress ( short command, long data, long * ref )
{
short err;
CheeseDecompressState *state;

	err = commandSelectorUnknown;
	
	switch ( command )
		{
		case kInitDecompressor:
			state = NewMemory ( kTemp, sizeof(CheeseDecompressState) );
			if ( state )
			{
				err = noErr;
				*ref = (long) state;
				
				state->sega = (RawData *) data;
				state->palette = 0;
				state->startFrame = 0;
				state->dstPatternID = 0;
				state->tileWidth = state->sega->header.width;
				state->tileHeight = state->sega->header.height;
				state->top = 0;
				state->left = 0;
				state->options = 0;
				state->raw = 0;
				state->rawBuffer = 0;
				state->pixelMapping[0] = -1;
			} else err = kOutOfMemoryErr;
			break;
		
		case kSetDecompressorPalette:
			err = noErr;
			state = (CheeseDecompressState *) *ref;
			state->palette = data;
			break;
			
		case kSetDecompressorDstPattern:
			err = noErr;
			state = (CheeseDecompressState *) *ref;
			if ( data == 0 || data == -1)
				{
				Boolean high = data == -1;
				
				/* BRAIN DAMAGE: this is my hack to force the decompressor to allocate */
				/* it's own patterns. this means we can't allocate from zero...? */
				data = state->sega->data.defaultPalette.uniqueChars;
				if( data == 0 )
					{
					data = state->sega->header.width * state->sega->header.height;
					}
				
				if (high)
					data = NewPatternBlockHigh( data );
				else
					data = NewPatternBlock( data );			// should this just warn?
				}

			ASSERT(data);			// should this just be a warning?
			state->dstPatternID = data;
			
			/* return the pattern id that we use */
			return data;
			break;
			
		case kSetDecompressorFrame:
			err = noErr;			
			state = (CheeseDecompressState *) *ref;
			state->startFrame = data;
			break;
			
		case kDecompressAFrame:
			state = (CheeseDecompressState *) *ref;
			err = DoDecompress ( state );
			break;
			
		case kFlushFrameState:
			err = noErr;
			break;
		
		case kSetDecompressorTiling:
			state = (CheeseDecompressState *) *ref;
			state->tileWidth = data & 0xffff;
			state->tileHeight = data >> 16;
			break;
		
		case kSetDecompressorOrigin:
			state = (CheeseDecompressState *) *ref;
			state->left = data & 0xffff;
			state->top = data >> 16;
			break;

		case kGetDecompressorClut:
		{
			short index;
			short *walker;
			short *destClut;
			
			err = noErr;			
			state = (CheeseDecompressState *) *ref;
			
			/* Does the image have a custom clut? */
			if (state->sega->header.compressType & kCustomPaletteCompressor)
			{
				/* Yes, copy it to the caller */
				walker = state->sega->data.customPalette.palette;
				destClut = (short *)data;
				for (index=0; index<16; ++index)
					*destClut++ = *walker++;
				
				return 0;
			}
			else
				return 1;	/* Default clut */
			break;
		}

		case kSetDecompressorOptions:
			state = (CheeseDecompressState *) *ref;
			state->options = data;
			break;

		case kResetDecompressorOptions:
			state = (CheeseDecompressState *) *ref;
			state->options &= (~data);
			break;

		case kGetDecompressorImageCache:
			err = noErr;
			break;
			
		case kReleaseDecompressorImageCache:
			err = noErr;
			break;

		case kSetDecompressorPixelMapping:
		{
			short index;
			
			state = (CheeseDecompressState *) *ref;
			
			if (data)
				for (index=0; index<16; ++index)
				{
					state->pixelMapping[index] = ((char *)data)[index];
				}
			else
				state->pixelMapping[0] = -1;
			break;
		}

		case kDisposeDecompressorPatterns:
			err = noErr;
			state = (CheeseDecompressState *) *ref;
			FreePatternBlock ( state->dstPatternID );
			break;
			
		case kCloseDecompressor:
			err = noErr;
			state = (CheeseDecompressState *) *ref;
			if (state->raw)
				err = DisposeDecompressor( state->raw );
			DisposeMemory ( (Ptr) state );
			break;
		}
	return err;
}


static
short DoDecompress ( CheeseDecompressState * state )
{
long			offset;
long			totalChars;
RawData	 *		image;
SegaErr			err;
short			frame;
short *			frameUniqueChars;
long			frameSize;
Decompressor	raw;

	if ( !state->rawBuffer)
	{
		// decompress all frames once
		if (err = DoCheese( state ))
			return err;
	}
	//---Create raw decompressor instance if none created yet
	//---This can be used again for animations, repositioning, etc.
	if ( !state->raw )
	{
		state->raw = CreateDecompressor ( (SegaBitMap *) state->rawBuffer, state->palette, state->startFrame );
		ASSERT_MESG ( state->raw, "Can't allocate raw decompressor for cheese decompressor" );
		if ( !state->raw )
			return -1;
	}
	
	state->dstPatternID = SetDecompressDstPattern ( state->raw, state->dstPatternID );
	err = SetImageTiling ( state->raw, state->tileWidth, state->tileHeight );
	if (err)
		return err;
	err = SetImageOrigin ( state->raw, state->left, state->top );
	if (err)
		return err;
	err = SetDecompressorOptions ( state->raw, state->options );
	if (err)
		return err;
	if (state->pixelMapping[0] != -1)
	{
		err = SetDecompressorPixelMapping ( state->raw, state->pixelMapping );
		if (err)
			return err;
	}
	
	err = DecompressFrame ( state->raw, state->startFrame );
	
	/* trash the space used to store the raw buffer (created in DoCheese) */
	DisposeMemory(state->rawBuffer);
	
	return err;
}

static short DoCheese(CheeseDecompressState * state)
{
short cell, frame, ii;
short cellCount, colorCount, cellSize;
short uniqueChars;
uchar colorMap[8];
register Ptr dp;
SegaBitMap *header;
long size;
unsigned short width;
	
	
	// Decompress a Cheesey image, then call the raw decompressor to load the VDP.

	state->bit = 8;

	header = &state->sega->header;
	width = header->width;
	width *= header->height * ( kCellIndexSize + kCellPixMapSize );
	size = sizeof(RawData) + width;
	
	// Allocate temporary buffer large enough to hold decompressed data (raw format)
	// This is disposed at a higher level (ie not in DoCheese, but in DoDecompress)
	state->rawBuffer = NewMemory( kTemp, size );
	if ( !state->rawBuffer )
		return kOutOfMemoryErr;

	// Copy header
	SegaBlockMove((Ptr)state->sega, (Ptr)state->rawBuffer, sizeof(SegaBitMap) );
	((SegaBitMap *)state->rawBuffer)->compressType = kRaw | (header->compressType & kCustomPaletteCompressor);
	
	// Set up source and destination pointers
	dp = state->rawBuffer + sizeof(SegaBitMap);
	state->sp = (Ptr)state->sega + sizeof(SegaBitMap);
	
	for (frame = 0; frame < state->sega->header.frames; frame++)
	{
		// Copy uniqueChars short
		uniqueChars = *(short *)state->sp;
		state->sp+=2;
		*(short *)dp = uniqueChars;
		dp+=2;
		
		// Copy palette if it exists
		if (header->compressType & kCustomPaletteCompressor)
		{
			for (ii = 16; ii; ii--) {
				*(short *)dp = *(short *)state->sp;
				dp+=2; state->sp+=2;
			}
		}
	
		// Skip size of decompressed data
		state->sp += 2;

		// Decompress cell index list?
		if (uniqueChars == 0)
		{
			cellCount = header->height * header->width;
		} else {
			long bitsMask;
			cellCount = uniqueChars;
			size = BitsToEncode(uniqueChars);
			
			bitsMask = (1 << size) - 1;
			
			for (ii=header->height * header->width; ii; ii--)
			{
				short cellNum = ReadBits(state, size, bitsMask);
				*(short *)dp = cellNum;
				dp+=2;
			}
		}
		// Decompress entire image (1) or cell-by-cell (0)?
		cellSize = 32;
		if (Read1Bit(state))
		{
			cellSize *= cellCount;
			cellCount = 1;
		}
		for (cell = cellCount; cell > 0; cell--)
		{
			// Is chunk compressed?
			if (Read1Bit(state))
			{
				// for each cell, decode color list
				colorCount = ReadBits(state,3, 0x07)+1;
				for (ii = 0; ii < colorCount; ii++)
				{
					colorMap[ii] = ReadBits(state,4, 0x0F);
				}
				if (colorCount > 1)
				{
					short someBits;
					long mask, halfMask;
					short halfSize;
					size = (colorCount == 2) ? 2 : ((colorCount < 5) ? 4 : 6);
					halfSize = size >> 1;
					
					mask = (1 << size) - 1;
					halfMask = (1 << halfSize) - 1;
					
					// now decode pattern data
					for (ii = cellSize; ii; ii--)
					{
						someBits = ReadBits(state, size, mask);		/* read 2 pixels at once  */
						*dp++ = (colorMap[someBits >> halfSize] << 4) | (colorMap[someBits & halfMask]);
					}
				} else {
					// every pixel is same color
					size = (colorMap[0]<<4) | colorMap[0];
					for (ii = cellSize; ii; ii--)
					{
						*dp++ = size;
					}
				}
			} else {
				// read raw pixel data since it could not be compressed
				
				if (state->bit == 8) {			/* this is a little nasty - if the data is aligned, just read it */
					for (ii = kCellPixMapSize; ii; ii--) {
						*dp++ = *state->sp++;
					}
				} else {
					for (ii = kCellPixMapSize; ii; ii--)
					{
						*dp++ = ReadBits(state,8, 0xff);
					}
				}
			}
		}
		AlignBits(state);
	} // frame loop
	return noErr;
}

static void AlignBits(register CheeseDecompressState *state)
{
	if (state->bit != 8)
	{
		state->sp += 2;
		*(long *)&state->sp &= ~1;
		state->bit = 8;
	}
}

static long ReadBits(register CheeseDecompressState *state, register long length, long rightMask)
{
	register long size = length;
	register unsigned long data = 0;
	register short bit = state->bit;
	if (size > bit)
	{
		size -= bit;
		data = *(unsigned char *)state->sp++ << (size);
		bit = 8;
	}
	bit -= size;
	data |= (*(unsigned char *)state->sp) >> (bit);
	if (!bit)
	{
		state->bit = 8;
		state->sp++;
	} else {
		state->bit = bit;
	}
	return data & rightMask;
}

static long Read1Bit(register CheeseDecompressState *state)
{
	register short bit = state->bit;
	register unsigned long data;
	
	bit--;
	data = *(unsigned char *)state->sp >> bit;
	data &= 1;
	
	if (bit) {
		state->bit = bit;
	} else {
		state->bit = 8;
		state->sp++;
	}
	return data;
}

static long BitsToEncode(long val)
{
	short i;
	for (i=15; i>=0; i--)
		if (val&(1<<i))
			return i+1;
}

#if 0		/* we're not currently using this */
static unsigned long * GetRightMask ( void )
{

	asm {
		lea		@rightMask,a0
		move.l	a0,d0
		rts

@rightMask
		dc.l	0x00
		dc.l	0x01
		dc.l	0x03
		dc.l	0x07
		dc.l	0x0f
		dc.l	0x1f
		dc.l	0x3f
		dc.l	0x7f
		dc.l	0xff
		dc.l	0x1ff
		dc.l	0x3ff
		dc.l	0x7ff
		dc.l	0xfff
		dc.l	0x1fff
		dc.l	0x3fff
		dc.l	0x7fff
		dc.l	0xffff
		dc.l	0x1ffff
		dc.l	0x3ffff
		dc.l	0x7ffff
		dc.l	0xfffff
		dc.l	0x1fffff
		dc.l	0x3fffff
		dc.l	0x7fffff
		dc.l	0xffffff
		dc.l	0x1ffffff
		dc.l	0x3ffffff
		dc.l	0x7ffffff
		dc.l	0xfffffff
		dc.l	0x1fffffff
		dc.l	0x3fffffff
		dc.l	0x7fffffff
		dc.l	0xffffffff
	}
}
#endif

/*
* This is a proc only used for the ROM install. Later we'll know how big things are
* when we send them over the wire.
*/
Ptr GetCheeseDecompressPtr ( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) CheeseDecompress + 2));
	size = (*(long *) ((Ptr) GetCheeseDecompressPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Cheese Compressor proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) CheeseDecompress;
#else
		CheckCodeForJTRefs((void *)CheeseDecompress, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
