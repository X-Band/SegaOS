/*
	File:		paths.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 5/16/94	SAH		first checked in

	To Do:
*/



typedef struct PathData * PathRef;


short	InitPathManager ( void ) =
	CallDispatchedFunction( kInitPathManager );

PathRef	CreatePath ( short type, Ptr data ) =
	CallDispatchedFunction( kCreatePath );

short	DisposePath ( PathRef path ) = 
	CallDispatchedFunction ( kDisposePath );
	
short	GetPathPoint ( PathRef path, long time, Point *p ) = 
	CallDispatchedFunction ( kGetPathPoint );
	
short	SetPathPoints ( PathRef path, Point startPoint, Point endPoint ) = 
	CallDispatchedFunction ( kSetPathPoints );
	
short	SetPathFrames ( PathRef path, long startFrame, long endFrame ) = 
	CallDispatchedFunction ( kSetPathFrames );
	
short	SetPathVelocity ( PathRef path, long velocity ) = 
	CallDispatchedFunction ( kSetPathVelocity );
	
long	DistBetweenPoints ( Point start, Point stop ) = 
	CallDispatchedFunction ( kDistBetweenPoints );
	