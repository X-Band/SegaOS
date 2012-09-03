/*
	File:		PatchDBPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/21/94	ADS		New lomem layout
		 <1>	 8/19/94	JBH		first checked in. Enjoy it Shannon!
	To Do:
*/

#ifndef __GamePatch__
#include "GamePatch.h"
#endif



#define		kPatchesKeptAround	3

typedef struct PatchDBGlobals
{
	long		gameIDs[kPatchesKeptAround];
	GameProc	gameEntryPoint;
} PatchDBGlobals;

#ifdef SIMULATOR
	#define MANAGERGLOBALTYPE PatchDBGlobals
#else
	extern PatchDBGlobals patchDB;
#endif
