/*
	File:		Decompress.c

	Contains:	Decompression manager

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/16/94	JBH		NewTempMemHigh -> NewMemoryHigh( kTemp, ---).
		<14>	 8/14/94	JBH		Updated to latest JoshCompress™. Comes free with a 68K assembly
									version of JoshDecompress! Act now.
		<13>	  8/1/94	CMY		Allocate the dictionary caches high in temp mems.
		<12>	 7/30/94	CMY		Added Get/Release DecompressorImageCache
		<11>	 7/24/94	JBH		Added JoshDecompress, to save ROM space.
		<10>	 7/19/94	CMY		Added SetDecompressorImage, to change the image data on the fly
									for animation.
		 <9>	 7/18/94	CMY		Keep a cache of the decompressed pattern table for the
									dictionary compressor.
		 <8>	 7/17/94	CMY		Added the Dictionary compressor.
		 <7>	 6/30/94	SAH		Removed the DB purge crap.
		 <6>	 6/29/94	HEC		Added the cheese decompressor.
		 <5>	 6/29/94	CMY		Added SetDecompressorPixelMapping and SetDecompressorPalette.
		 <4>	 6/20/94	SAH		Only install the decompressors on the stimulator.
		 <3>	 6/20/94	CMY		Added SetDecompressorOptions
		 <2>	 6/15/94	CMY		Custom palettes for bitmaps and sprites.
		 <6>	 5/25/94	HEC		new memory manager
		 <5>	 5/23/94	SAH		Made CreateDecompressor look at compressType rather than taking
									it as a parameter.
		 <4>	 5/19/94	SAH		Made pattern id a long. Added DisposeImagePatterns.
		 <3>	 5/18/94	SAH		Added the kVRamData decompressor.
		 <2>	 5/14/94	SAH		Revamped decompression model. Added SetImageTiling.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/



#include "SegaOS.h"
#include "Decompress.h"
#include "Decompressors.h"
#include "DecompressPriv.h"
#include "Heaps.h"
#include "Reliability.h"
#include "Database.h"
#include "DBTypes.h"
#include "Errors.h"
#include "PatchDBHeaps.h"
#include "sprites.h"
#include "SegaScrn.h"
#include "Utils.h"
#include "CompressDictionary.h"	// for JoshCompressHeader structure


#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif



/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
DecompressGlobals decompress;
#endif


short				_InitDecompression ( void );
DecompressData *	_CreateDecompressor ( SegaBitMap * data, short palette, short startFrame );
short				_DisposeDecompressor ( DecompressData * decompress );
long				_SetDstPattern ( DecompressData * decompress, long dstPatternID );
short				_SetImageTiling ( DecompressData * decompress, short tileWidth, short tileHeight );
short				_SetImageOrigin ( DecompressData * decompress, short xPos, short yPos );
short				_DecompressFrame ( DecompressData * decompress, short frame );
short				_DisposeImagePatterns ( DecompressData * decompress );
short				_SetDecompressorOptions ( DecompressData *decompress, long options );
short				_SetDecompressorPixelMapping ( DecompressData *decompress, char *pixelMapping );
short				_SetDecompressorPalette( DecompressData *decompress, short palette);
short				_GetImageClut ( DecompressData *decompress, short clutStorage[16] );
short			*	_GetDictionaryCache( short dictionary );
void				_ReleaseDictionaryCache( short dictionary );
short				_SetDecompressorImage( DecompressData *decompress, SegaBitMap * data );
void				_JoshDecompress( JoshCompressHeader *header, unsigned char *dest );
void			*	_GetDecompressorCache( DecompressData *decompress );
void				_ReleaseDecompressorCache( DecompressData *decompress, void *cache );

/* Internally vectored routines */
static short	*	_ExapandPatternDictionary( short dictionary );

/* In Utils.c, but managerized through decompression */
extern void _UnpackBytes(char *source, char *dest, short sourceLength);


long
_DecompressControl ( short command, long data )
{
long offset;
short error;
DecompressGlobals *globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(decompress,offset);
			
			error = AllocateGlobalSpace ( kDecompressManager, offset, sizeof(DecompressGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create decompress globals" );
			if ( error != noErr )
			{
				return error;
			}
			
			/* install our selectors */
			SetDispatchedFunction ( kInitDecompression,		kDecompressManager,	_InitDecompression );
			SetDispatchedFunction ( kCreateDecompressor,	kDecompressManager,	_CreateDecompressor );
			SetDispatchedFunction ( kDisposeDecompressor,	kDecompressManager,	_DisposeDecompressor );
			SetDispatchedFunction ( kSetDstPattern,			kDecompressManager,	_SetDstPattern );
			SetDispatchedFunction ( kSetDecompressorPaletteSelector, kDecompressManager,	_SetDecompressorPalette );
			SetDispatchedFunction ( kSetImageTiling,		kDecompressManager,	_SetImageTiling );
			SetDispatchedFunction ( kSetImageOrigin,		kDecompressManager,	_SetImageOrigin );
			SetDispatchedFunction ( kGetImageClut,			kDecompressManager,	_GetImageClut );
			SetDispatchedFunction ( kDisposeImagePatterns,	kDecompressManager,	_DisposeImagePatterns );
			SetDispatchedFunction ( kDecompressFrame,		kDecompressManager,	_DecompressFrame );
			SetDispatchedFunction ( kGetDictionaryCache,	kDecompressManager, _GetDictionaryCache );
			SetDispatchedFunction ( kReleaseDictionaryCache,kDecompressManager, _ReleaseDictionaryCache );
			SetDispatchedFunction ( kSetDecompressorOptionsSelector,kDecompressManager, _SetDecompressorOptions );
			SetDispatchedFunction ( kSetDecompressorPixelMappingSelector,kDecompressManager, _SetDecompressorPixelMapping );
			SetDispatchedFunction ( kExpandPatternDictionary,	kDecompressManager, _ExapandPatternDictionary );
			SetDispatchedFunction ( kUnpackBytes,			kDecompressManager, _UnpackBytes );
			SetDispatchedFunction ( kSetDecompressorImage,	kDecompressManager, _SetDecompressorImage );
			SetDispatchedFunction ( kJoshDecompress,		kDecompressManager, _JoshDecompress );
			SetDispatchedFunction ( kGetDecompressorCache,			kDecompressManager, _GetDecompressorCache );
			SetDispatchedFunction ( kReleaseDecompressorCache,		kDecompressManager, _ReleaseDecompressorCache );
			break;
		
		case kSoftInialize:
		{
			short index;
			
			for (index=0; index<kMaxDictionaries; ++index)
			{
				REFGLOBAL(decompress, dictionaryCaches)[index] = nil;
				REFGLOBAL(decompress, cacheUsers)[index] = 0;
			}
			error = noErr;
			break;
		}
		
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


short
_InitDecompression ( void )
{
#ifdef SIMULATOR
Ptr compressor;
short err;

	/* this one inits the database for the decompression types */
	/* this is used to update new RAM instances. the ROM build will probably */
	/* use this somehow? */
	
	err = -1;
	
	/* install the raw decompressor */
	compressor = GetRawDecompressPtr();
	ASSERT_MESG( compressor, "Can't init raw decompressor" );
	if ( compressor )
		{
		err = DBAddItem( kDecompressorType, kRaw, compressor );
		ASSERT_MESG( !err, "Error adding decompressor to DB" );
	
		DisposeMemory( compressor );
		}
	
	/* install the cheese decompressor */
	compressor = GetCheeseDecompressPtr();
	ASSERT_MESG( compressor, "Can't init cheese decompressor" );
	if ( compressor )
		{
		err = DBAddItem( kDecompressorType, kCheese, compressor );
		ASSERT_MESG( !err, "Error adding cheese decompressor to DB" );
	
		DisposeMemory( compressor );
		}

	/* install the master dictionary based raw decompressor */
	compressor = GetMasterDictionaryDecompressPtr();
	ASSERT_MESG( compressor, "Can't init master dict decompressor" );
	if ( compressor )
		{
		err = DBAddItem( kDecompressorType, kUseDictionary, compressor );
		ASSERT_MESG( !err, "Error adding master dict decompressor to DB" );
	
		DisposeMemory( compressor );
		}
		
	/* install the sprite device decompressor */
	compressor = GetSpriteDeviceDecompressPtr();
	ASSERT_MESG( compressor, "Can't init sprite decompressor" );
	if ( compressor )
		{
		err = DBAddItem( kDecompressorType, kVRamData, compressor );
		ASSERT_ME