/*
	File:		pathsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 5/16/94	SAH		first checked in

	To Do:
*/


typedef
struct PathGlobals
{
	short dummy;
} PathGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE PathGlobals
#else

extern PathGlobals path;

#endif


typedef
struct PathData
{
	PathProc	proc;
	long		ref;
} PathData;
