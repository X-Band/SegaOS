/*
	File:		animationPriv.h

	Contains:	

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 6/20/94	CMY		Added priorities to animations, so we can selectively enable
									animations.
		 <2>	 6/18/94	CMY		Added animationsEnabled flag.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/




typedef
struct AnimationGlobals
{
	short		minimumPriority;		/* Only allow animations at or above this priority level */
} AnimationGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE AnimationGlobals
#else

extern AnimationGlobals animate;

#endif



typedef
struct Animation
{
	AnimateProc	proc;
	long		data;
	long		procRef;
	TimeProcRef	timer;
	short		priority;
} Animation;
