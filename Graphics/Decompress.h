/*
	File:		Decompress.h

	Contains:	

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 7/30/94	CMY		Added Get/Release DecompressorImageCache
		 <6>	 7/24/94	JBH		Added JoshDecompress, to shrink the ROM size
		 <5>	 7/19/94	CMY		Added SetDecompressorImage to change the image on the fly for
									animation.
		 <4>	 7/18/94	CMY		GetImageClut to support custom cluts in funky formats. Added a
									cache of the decompressed pattern table for the dictionary
									compressor.
		 <3>	 6/29/94	CMY		Added SetDecompressorPixelMapping and SetDecompressorPalette.
		 <2>	 6/20/94	CMY		Added SetDecompressorOptions
		 <2>	 6/20/94	CMY		SetDecompressorOptions
		 <4>	 5/23/94	SAH		Made CreateDecompressor look at compressType rather than taking
									it as a parameter.
		 <3>	 5/19/94	SAH		Made the pattern id a long. Added DisposeImagePatterns.
		 <2>	 5/14/94	SAH		Added SetImageTiling.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/


#ifndef __Decompress__
#define __Decompress__

#ifndef __Sega__
#include "Sega.h"
#endif

#ifndef __CompressDictionary__
#include "CompressDictionary.h"
#endif

#define kMaxDictionaries 16

/* an external reference to a decompressor instance */
typedef long * Decompressor;

/*
* Decompression manager api
*/

short	InitDecompression ( void ) =
	CallDispatchedFunction( kInitDecompression );

Decompressor CreateDecompressor ( SegaBitMap * data, short palette, short startFrame ) =
	CallDispatchedFunction( kCreateDecompressor );

short	DisposeDecompressor ( Decompressor decompress ) =
	CallDispatchedFunction( kDisposeDecompressor );
	
short	DisposeImagePatterns ( Decompressor decompress ) =
	CallDispatchedFunction( kDisposeImagePatterns );

long	SetDecompressDstPattern ( Decompressor decompress, long dstPatternID ) =
	CallDispatchedFunction( kSetDstPattern );

short	SetImageTiling ( Decompressor decompress, short tileWidth, short tileHeight ) =
	CallDispatchedFunction( kSetImageTiling );

short	SetImageOrigin ( Decompressor decompress, short xPos, short yPos ) =
	CallDispatchedFunction( kSetImageOrigin );

short	GetImageClut ( Decompressor decompress, short clutStorage[16] ) =
	CallDispatchedFunction( kGetImageClut );

short	*GetDictionaryCache( short dictionary ) =
	CallDispatchedFunction( kGetDictionaryCache );
	
void	ReleaseDictionaryCache( short dictionary ) =
	CallDispatchedFunction( kReleaseDictionaryCache );
	
short	DecompressFrame ( Decompressor decompress, short frame ) =
	CallDispatchedFunction( kDecompressFrame );

short	SetDecompressorOptions ( Decompressor decompress, long options ) =
	CallDispatchedFunction( kSetDecompressorOptionsSelector );

short	SetDecompressorPixelMapping ( Decompressor decompress, char *pixelMapping ) =
	CallDispatchedFunction( kSetDecompressorPixelMappingSelector );
	
short	SetDecompressorPalette ( Decompressor decompress, short palette ) =
	CallDispatchedFunction( kSetDecompressorPaletteSelector );

short	SetDecompressorImage ( Decompressor decompress, SegaBitMap * image ) =
	CallDispatchedFunction( kSetDecompressorImage );
	
void	JoshDecompress( JoshCompressHeader *header, unsigned char *dest ) =
	CallDispatchedFunction( kJoshDecompress );
	
void	*GetDecompressorCache( Decompressor decompress ) =
	CallDispatchedFunction( kGetDecompressorCache );

void	ReleaseDecompressorCache( Decompressor decompress, void *cache ) =
	CallDispatchedFunction( kReleaseDecompressorCache );

#endif