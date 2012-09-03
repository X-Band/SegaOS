/*
	File:		Decompressors.h

	Contains:	Decompressors we know about

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 7/30/94	CMY		Added Get/Release DecompressorImageCache.
		<11>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		<10>	 7/21/94	CMY		Remove the misleading kGrayscaleDecompress options
		 <9>	 7/19/94	CMY		Added SetDecompressorImageData to support changing the image on
									the fly for animation
		 <8>	 7/18/94	CMY		Added GetDecompressorClut so the decompressors can store a clut
									in a proprietary format.
		 <7>	 7/17/94	CMY		Add Dictionary compression. Jinx.
		 <6>	 7/16/94	KON		Add Dictionary compression.
		 <5>	 6/29/94	HEC		Added GetCheeseDecompressPtr
		 <4>	 6/29/94	CMY		Added the "decompressGrayscale" option to the decompressor
									options.
		 <3>	 6/20/94	CMY		Added the omitTransparentCells option.
		 <2>	  6/3/94	HEC		Added cheese decompressor type
		 <4>	 5/19/94	SAH		Added kDisposeDecompressorPatterns.
		 <3>	 5/18/94	SAH		Added the kVRamData decompressor.
		 <2>	 5/14/94	SAH		Added kSetDecompressorTiling.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/

#ifndef __Decompressors__
#define	__Decompressors__

/*
* Decompressor proc type
*/

typedef long (*DecompressProc) ( short command, long data, long * ref );


/*
* types of compression
*/

enum
{
	kRaw,
	kVRamData,
	kCheese,
	kUseDictionary
};


/*
* Decompressor commands
*/

enum
{
	kInitDecompressor,
	kSetDecompressorPalette,
	kSetDecompressorDstPattern,
	kSetDecompressorFrame,
	kDecompressAFrame,
	kFlushFrameState,
	kSetDecompressorTiling,
	kSetDecompressorOrigin,
	kGetDecompressorClut,
	kDisposeDecompressorPatterns,
	kSetDecompressorOptions,
	kResetDecompressorOptions,
	kSetDecompressorPixelMapping,
	kSetDecompressorImageData,
	kGetDecompressorImageCache,
	kReleaseDecompressorImageCache,
	kCloseDecompressor
};


/*
* proc ptrs to routines to allocate a decompressor in memory so we can put it in
* the database. they return a temp ptr which the caller disposes of
*/

Ptr GetRawDecompressPtr ( void );
Ptr GetCheeseDecompressPtr ( void );
Ptr GetSpriteDeviceDecompressPtr ( void );
Ptr GetMasterDictionaryDecompressPtr( void );

#endif
