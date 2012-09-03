/*
	File:		ProgressPriv.h

	Contains:	

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  7/8/94	JBH		first checked in
	To Do:
*/


/* error codes returned by a progress proc */
enum
{
	kProgressOK = 0,
	kProgressInitError
};

/* commands to send to a progress proc */
enum
{
	kInitProgressProcCmd,		// sets position to 0 */
	kDisposeProgressProcCmd,
	kSetProgressPositionCmd,
	kIdleProgressCmd
};

typedef
struct ProgressGlobals
{
	long		lastIdleTime;		/* Last time the idler did anything */
} ProgressGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE ProgressGlobals
#else

extern ProgressGlobals gProgress;

#endif

typedef struct Progress
{
	ProgressProc	proc;
	long			procRef;
	long			data;
} Progress;

