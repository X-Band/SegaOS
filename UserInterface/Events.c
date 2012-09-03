/*
	File:		Events.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<18>	 8/24/94	CMY		Fixed test for kModemTryingToConnect.
		<17>	 7/25/94	HEC		Added _FlushUserEvents to kill controller type-ahead at end of
									screens' setup.
		<16>	 7/21/94	ADS		Remove temp use of OSManagersTEMP
		<15>	 7/19/94	ADS		Vector Mania
		<14>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<13>	  7/4/94	SAH		Added kGetNextCommand.
		<12>	  7/2/94	CMY		Added autorepeat to controller buttons.
		<11>	 6/16/94	CMY		Added an event queue for controller reads.
		<10>	 6/16/94	BET		No it doesn't.
		 <9>	 6/15/94	HEC		SegaOSIdle takes a param now
		 <8>	 6/13/94	SAH		Managerized uiser button crap.
		 <7>	 6/12/94	DJ		WaitForUserButtonPress knows about the stinky
									kModemTryingToConnect hack.
		 <6>	 6/10/94	KON		Update to use GetNextCommand instead of ReadControllers.
		 <5>	  6/5/94	KON		Added VDPIdle call to WaitForUserButtonPress.
		 <4>	  6/4/94	DJ		WaitForUserButtonPress
		 <3>	 5/30/94	SAH		Fuck.
		 <2>	 5/30/94	SAH		Managerize.
		 <2>	 5/18/94	KON		Add InitUserEvents.
		 <1>	 5/17/94	KON		first checked in

	To Do:
*/

#include "Events.h"
#include "EventsPriv.h"
#include "Time.h"
#include "Errors.h"
#include "SegaIn.h"
#include "vdp.h"
#include "Utils.h"
#include "Controls.h"

#ifndef SIMULATOR
EventGlobals event;
#endif

#ifdef SIMULATOR
#include <StdIO.h>
#endif

void	_InitUserEvents( void );
void	_FlushUserEvents( void );
void 	_WaitForUserButtonPress( void );
Boolean	_CheckUserButtonPress( void );
short	_GetNextControllerEvent(short controller);


//
// local vectorized functions
//
static short	_QueueGet(short *queueSize, short *queue);
static void		_QueueInsert(short *queueSize, short *queue, short data);

short	QueueGet(short *queueSize, short *queue) =
			CallDispatchedFunction( kQueueGet );

void	QueueInsert(short *queueSize, short *queue, short data)	=
			CallDispatchedFunction( kQueueInsert );


//			
// not vectorized (called by time manager)
//
long		CheckForControllerEvent(long time, long data);






long
_EventsControl ( short command, long data )
{
long offset;
short error;
EventGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(event,offset);
			
			error = AllocateGlobalSpace ( kEventsManager, offset, sizeof(EventGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create event globals" );
			if ( error != noErr )
				{
				return error;
				}
			
			/* install our selectors */
			SetDispatchedFunction ( kInitUserEvents,			kEventsManager,	_InitUserEvents );
			SetDispatchedFunction ( kFlushUserEvents,			kEventsManager,	_FlushUserEvents );
			SetDispatchedFunction ( kWaitForUserButtonPress,	kEventsManager,	_WaitForUserButtonPress );
			SetDispatchedFunction ( kCheckUserButtonPress,		kEventsManager,	_CheckUserButtonPress );
			SetDispatchedFunction ( kGetNextControllerEvent,	kEventsManager,	_GetNextControllerEvent );
			SetDispatchedFunction ( kGetNextCommand,			kEventsManager,	_GetNextCommand );

			SetDispatchedFunction ( kQueueGet,					kEventsManager,	_QueueGet );
			SetDispatchedFunction ( kQueueInsert,				kEventsManager,	_QueueInsert );
			break;
		
		case kSoftInialize:
			error = noErr;
			/* init our globals for this time around */
			REFGLOBAL(event,lastUserEvent) = 0;
			REFGLOBAL(event,controller1QueueSize) = 0;
			REFGLOBAL(event,controller2QueueSize) = 0;
			REFGLOBAL(event,controller1LastState) = 0;
			REFGLOBAL(event,controller2LastState) = 0;
			GetManagerGlobals(kEventsManager, (Ptr *)&globals, nil);
			REFGLOBAL(event,controllerTimeProc) = AddTimeRequest(CheckForControllerEvent, (long)globals);
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return erro