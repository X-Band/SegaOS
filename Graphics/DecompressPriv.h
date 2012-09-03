/*
	File:		DecompressPriv.h

	Contains:	

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/18/94	CMY		
		 <2>	 7/18/94	CMY		Added a cache of the expanded pattern table for the dictionary
									compressor.
		 <4>	 5/23/94	SAH		Save and restore current device around frame decompression.
		 <3>	 5/19/94	SAH		Made the pattern data a SegaBitMap.
		 <2>	 5/14/94	SAH		Removed DecompressorRef.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/


#ifndef __Decompress__
#include "Decompress.h"
#endif


typedef
struct DecompressGlobals
{
	short 	*dictionaryCaches[kMaxDictionaries];
	char	cacheUsers[kMaxDictionaries];
} DecompressGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE DecompressGlobals
#else

extern DecompressGlobals decompress;

#endif


/*
* Decompressor data structure
*/

typedef
struct DecompressData
{
	DecompressProc	proc;
	SegaBitMap *	data;
	long			ref;
	short			device;
} DecompressData;


short *ExpandPatternDictionary( short dictionary ) =
	CallDispatchedFunction( kExpandPatternDictionary );
