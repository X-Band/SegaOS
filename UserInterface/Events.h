/*
	File:		Events.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 7/26/94	HEC		Chicken FUCKING FlushUserEvents
		 <5>	 6/16/94	CMY		Added an event queue for controller reads, removed
									AllowUserEvent.
		 <4>	 6/13/94	SAH		Managerized new functions.
		 <3>	  6/4/94	DJ		WaitForUserButtonPress
		 <2>	 5/30/94	SAH		Managerize.
		 <2>	 5/18/94	KON		Added InitUserEvents and #ifndef __Events__.
		 <1>	 5/17/94	KON		first checked in

	To Do:
*/

#ifndef __Events__
#define __Events__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif


void	InitUserEvents( void ) = 
	CallDispatchedFunction( kInitUserEvents );

void	FlushUserEvents( void ) = 
	CallDispatchedFunction( kFlushUserEvents );

void WaitForUserButtonPress(void) = 
	CallDispatchedFunction( kWaitForUserButtonPress );

Boolean CheckUserButtonPress(void) = 
	CallDispatchedFunction( kCheckUserButtonPress );

short GetNextControllerEvent(short controller) =
	CallDispatchedFunction( kGetNextControllerEvent );

#endif __Events__