/*
	File:		PathProcs.h

	Contains:	Definitions for standard paths.

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/10/94	KON		Add vector paths.
		 <1>	 5/16/94	SAH		first checked in

	To Do:
*/




/*
* a bunch of known animation path procs. add new ones here.
*/

/*
* Path proc type
*/

typedef short (*PathProc) ( short command, long data1, long data2, long * ref );


/*
* types of paths
*/

enum
{
	kLinePath,
	kCurvePath,
	kPointsPath,
	kVectorPath
};


/*
* Decompressor commands
*/

enum
{
	kInitPathProc,
	kSetPathProcPoints,
	kSetPathProcFrames,
	kSetPathProcVelocity,
	kGetPathProcPoint,
	kClosePathProc
};


/*
* proc ptrs to routines to allocate paths in memory so we can put it in
* the database. they return a temp ptr which the caller disposes of
*/

Ptr GetLinePathPtr ( void );
Ptr GetCurvePathPtr ( void );
Ptr GetPointsPathPtr ( void );
Ptr GetVectorPathPtr ( void );
