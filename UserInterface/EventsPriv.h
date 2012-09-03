/*
	File:		EventsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	  7/4/94	SAH		Added _GetNextCommand.
		 <3>	  7/2/94	CMY		Autorepeat for controller buttons.
		 <2>	 6/16/94	CMY		Added an event queue for controller reads.
		 <1>	 5/30/94	SAH		first checked in

	To Do:
*/


#ifndef __Time__
	#include "Time.h"
#endif

//
// Constant to delay before doing user actions in a loop
//
#define kTimeConstant 10

//
// Size of event queue
//
#define kEventQueueSize 8	/* Allows 8 presses to be queued for each controller */

//
// Autorepeat values
//
#define kAutoRepeatDelay	30	/* Ticks until we start autorepeating */
#define kAutoRepeatSpeed	5	/* Autorepeat presses are spaced 5 ticks apart */

typedef
struct EventGlobals
{
	long		lastUserEvent;
	short		controller1QueueSize;
	short		controller1Queue[kEventQueueSize];
	short		controller1LastState;
	long		controller1RepeatTime;
	short		controller2QueueSize;
	short		controller2Queue[kEventQueueSize];
	short		controller2LastState;
	long		controller2RepeatTime;
	TimeProcRef	controllerTimeProc;
} EventGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE EventGlobals

#else
extern EventGlobals event;

#endif


/* in SegaIn.c */
void _GetNextCommand( short *num1, short *num2 );
