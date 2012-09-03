/*
	File:		Progress.h

	Contains:	xxx put contents here xxx

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/21/94	JBH		Added kTimerProgressUseString
		 <8>	 8/20/94	HEC		Added kCenterTimerHoriz.
		 <7>	  8/4/94	JBH		Add option for timerProgress to not draw on the spawn (this
									allows news countdown to set to the appropriate value).
		 <6>	 7/28/94	KON		Make the modeData flags a bit field rather than an enum.
		 <5>	 7/28/94	JBH		Add option for timerProgress to not use a colon (for non-time
									countdowns, ie news countdown timer)
		 <4>	 7/10/94	ADS		Add moreData constants for text progress
		 <3>	  7/9/94	JBH		Add proto for timer progress proc.
		 <2>	  7/8/94	JBH		Change rectangle to top, left, bottom, right format, add
									kCountdownTimerProgress
		 <1>	  7/8/94	JBH		first checked in
	To Do:
*/



#ifndef	__Progress__
#define	__Progress__

#ifndef	__SegaGraphics__
#include "SegaGraphics.h"
#endif

#ifndef	__Animation__
#include "Animation.h"
#endif

#ifndef __DataBase__
#include "DataBase.h"
#endif __DataBase__




/* progress proc types */
enum
{
	kSimpleProgress = 1,
	kCountdownTimerProgress
};

/* moreData codes used BY COUNTDOWNTIMER only */
enum
{
	kNoTextGWorldProgress = 1,			// bit 0: make a GWorld
	kDrawWithAColonProgress = 2,		// bit 1: display as a number, not a time
	kDontDrawOnSpawn = 4,				// bit 2: only draw during SetProgressPosition (useful for news)
	kCenterTimerHoriz = 8				// bit 3: center timer horizontally within dialog
};

#define	kTimerProgressUseString	0xFF00	// call SetProgressPosition with this OR'd with string DBID

/* The basic record for a progress proc. It includes bounds and the total time (full-scale deflection) */
typedef struct ProgressRec 
{
	Rect	boundsRect;
	short	totalTime;
	short	progressProcID;			/* ie kSimpleProgress */
} ProgressRec;

typedef long *ProgressRef;

/*
* user proc to drive progress.
*/
typedef long (*ProgressProc) ( short command, ProgressRec *progData, long moreData, long * ref );

Ptr	GetSimpleProgressProc ( void );

Ptr	GetTimerProgressProc ( void );

short	InitProgressProcs ( void ) =
	CallDispatchedFunction( kInitProgressProcs );
	
ProgressRef	SpawnProgressProc ( ProgressRec *theRecord, long data ) =
	CallDispatchedFunction( kSpawnProgressProc );

short	DisposeProgressProc ( ProgressRef ref ) =
	CallDispatchedFunction( kDisposeProgressProc );
	
short	SetProgressPosition ( ProgressRef ref, short newPosition ) =
	CallDispatchedFunction( kSetProgressPosition );
	
short	ProgressIdle ( ProgressRef ref ) =
	CallDispatchedFunction( kProgressIdle );

#endif
