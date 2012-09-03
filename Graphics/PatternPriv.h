/*
	File:		PatternPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 7/27/94	HEC		Removed highPatEnd for Chris.  Changed short globals to unsigned
									shorts.
		 <3>	 7/26/94	CMY		Added highPatEnd to allocate high pattern blocks.
		 <2>	  7/3/94	JBH		Added topPat.
		 <1>	 5/18/94	HEC		first checked in

	To Do:
*/


#include "Pattern.h"

typedef
struct PatternGlobals
{
	unsigned short	max;		// maximum number of patterns
	unsigned short	free;		// number of free patterns
	long			topPat;		// 0 means nothing is "on top", else its the top pat block
	unsigned short	*patMap;	// bitmap of available pattern nodes
} PatternGlobals;

#ifdef SIMULATOR
#define MANAGERGLOBALTYPE PatternGlobals
#else
extern PatternGlobals patterns;
#endif





