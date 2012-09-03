/*
	File:		Animation.c

	Contains:	Animation Manager

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<14>	 8/21/94	JBH		Made routines check for a nil animation. Maybe I should have
									installed EvenBetterBusError!!
		<13>	 7/26/94	CMY		Added button animator proc.
		<12>	 7/25/94	CMY		Added GetAnimationSuspendLevel.
		<11>	 7/15/94	JBH		Added two CLUT animators.
		<10>	  7/6/94	SAH		Added TheAnimator.
		 <9>	  7/3/94	SAH		Added SpawnDBAnimation.
		 <8>	 6/30/94	SAH		Removed database purge values.
		 <7>	 6/29/94	CMY		Added SetAnimationGrayFlag.
		 <6>	 6/20/94	SAH		Only install the animation procs on the stimulator.
		 <5>	 6/20/94	CMY		Animation priorities.
		 <4>	 6/18/94	CMY		Added calls to Suspend and Resume animations
		 <3>	 6/15/94	SGR		added LED animation stuff
		 <2>	 6/13/94	SAH		Don't allocate globals if we don't use them.
		 <5>	 5/25/94	HEC		new memory manager
		 <4>	 5/24/94	SAH		Fixed a copy paste bug.
		 <3>	 5/21/94	SAH		Implemented SpawnAnimation. Added InitAnimationProcs and
									DrawAnimationFrame. Made Start and StopAnimation do some sanity
									checking.
		 <2>	 5/17/94	SAH		Made DisposeAnimation call StopAnimation first if it needs to.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/



#include "SegaOS.h"
#include "Animation.h"
#include "Time.h"
#include "AnimationPriv.h"
#include "Reliability.h"
#include "Heaps.h"
#include "Sprites.h"
#include "Errors.h"
#include "Animators.h"
#include "DBTypes.h"
#include "PatchDBHeaps.h"


#ifdef SIMULATOR
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif



/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
AnimationGlobals animate;
#endif


short			_InitAnimateProcs ( void );
Animation *		_SpawnAnimation ( short animatorID, long data );
Animation *		_SpawnDBAnimation ( DBID animationID );
Animation *		_CreateAnimation ( AnimateProc proc, long data );
short			_DisposeAnimation ( Animation * animation );
short			_DrawAnimationFrame ( Animation * animation );
short			_StartAnimation ( Animation * animation );
short			_StopAnimation ( Animation * animation );
void			_SetAnimationPriority ( Animation * animation, short priority );
void			_SuspendAnimations ( short minimumPriority );
void			_SetAnimationGrayFlag ( Animation * animation, Boolean grayFlag );
static	long	AnimationTimeProc ( long time, Animation * animation );
short			_GetAnimationSuspendLevel( void );


long
_AnimationControl ( short command, long data )
{
long offset;
short error;
AnimationGlobals *globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(animate,offset);
			
			error = AllocateGlobalSpace ( kAnimationManager, offset, sizeof(AnimationGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create animation globals" );
			if ( error != noErr )
				{
				return error;
				}
				
			/* install our selectors */
			SetDispatchedFunction ( kInitAnimateProcs,	kAnimationManager,	_InitAnimateProcs );
			SetDispatchedFunction ( kSpawnAnimation,	kAnimationManager,	_SpawnAnimation );
			SetDispatchedFunction ( kSpawnDBAnimation,	kAnimationManager,	_SpawnDBAnimation );
			SetDispatchedFunction ( kCreateAnimation,	kAnimationManager,	_CreateAnimation );
			SetDispatchedFunction ( kDisposeAnimation,	kAnimationManager,	_DisposeAnimation );
			SetDispatchedFunction ( kDrawAnimationFrame,kAnimationManager,	_DrawAnimationFrame );
			SetDispatchedFunction ( kStartAnimation,	kAnimationManager,	_StartAnimation );
			SetDispatchedFunction ( kStopAnimation,		kAnimationManager,	_StopAnimation );
			SetDispatchedFunction ( kSuspendAnimations,	kAnimationManager,	_SuspendAnimations );
			SetDispatchedFunction ( kSetAnimationPriority,	kAnimationManager,	_SetAnimationPriority );
			SetDispatchedFunction ( kSetAnimationGrayFlag,	kAnimationManager,	_SetAnimationGrayFlag );
			SetDispatchedFunction ( kGetAnimationSuspendLevel,	kAnimationManager,	_GetAnimationSuspendLevel );
			break;
		
		case kSoftInialize:
			REFGLOBAL(animate,minimumPriority) = 0;
			error = noErr;
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
		
	return error;
}


short
_InitAnimateProcs ( void )
{
#ifdef SIMULATOR
Ptr animator;
short err;

	/* this one inits the database for the standard animation procs */
	/* this is used to update new RAM instances. the ROM build will probably */
	/* use this somehow? */
	
	err = -1;
	
	/* install the frame animator */
	animator = GetFrameAnimatorProc();
	ASSERT_MESG( animator, "Can't load frame animator" );
	if ( animator )
		{
		err = DBAddItem( kAnimatorType, kFrameAnimate, animator );
		DisposeMemory( animator );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding animator to DB" );
			return err;
			}
		}
	
	/* install our general purpose animator */
	animator = GetTheAnimatorProc();
	ASSERT_MESG( animator, "Can't load THE animator" );
	if ( animator )
		{
		err = DBAddItem( kAnimatorType, kTheAnimator, animator );
		DisposeMemory( animator );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding animator to DB" );
			return err;
			}
		}
	
	/* install the led animator */
	animator = GetLEDAnimatorProc();
	ASSERT_MESG( animator, "Can't load led animator" );
	if ( animator )
		{
		err = DBAddItem( kAnimatorType, kLEDAnimate, animator );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding animator to DB" );
			return err;
			}
		}

	/* install the clut (table) animator */
	animator = GetTableAnimatorProc();
	ASSERT_MESG( animator, "Can't load table animator" );
	if ( animator )
		{
		err = DBAddItem( kAnimatorType, kTableAnimate, animator );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding animator to DB" );
			return err;
			}
		}

	/* install the clut data-driven animator */
	animator = GetCLUTDataAnimatorProc();
	ASSERT_MESG( animator, "Can't load table data-driven animator" );
	if ( animator )
		{
		err = DBAddItem( kAnimatorType, kCLUTDataAnimate, animator );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding animator to DB" );
			return err;
			}
		}

	/* install the button animator */
	animator = GetButtonAnimatorProc();
	ASSERT_MESG( animator, "Can't load button animator" );
	if ( animator )
		{
		err = DBAddItem( kAnimatorType, kButtonAnimate, animator );
		if(err)
			{
			ASSERT_MESG( !err, "Error adding animator to DB" );
			return err;
			}
		}

#endif
	return noErr;
}


Animation *
_SpawnAnimation ( short animatorID, long data )
{
AnimateProc		proc;
Animation *		animation;

	animation = 0L;
	
	proc = (AnimateProc) DBGetItem ( kAnimatorType, animatorID );
	if ( proc )
		{
		animation = (Animation *) CreateAnimation ( proc, data );
		}
	
	return animation;
}


Animation *
_SpawnDBAnimation ( DBID animationID )
{
DBAnimation *	dbData;
Animation *		animation;

	animation = 0L;
	
	dbData = (DBAnimation *) DBGetItem ( kAnimation, animationID );
	if ( dbData )
		{
		animation = (Animation *) SpawnAnimation ( dbData->animationProc, (long) dbData->data );
		}
	
	return animation;
}


Animation *
_CreateAnimation ( AnimateProc proc, long data )
{
Animation * animation;
SegaErr err;

	animation = NewMemory ( false, sizeof(Animation) );
	ASSERT_MESG ( animation, "Can't allocate animation data" );
	if ( animation )
		{
		animation->proc = proc;
		animation->data = data;
		animation->timer = 0L;
		animation->priority = 0;
		
		err = (*proc) ( kInitAnimationProc, 0L, data, &animation->procRef );
		ASSERT_MESG ( !err, "Error initing animation proc" );
		if ( err != noErr )
			{
			DisposeMemory ( animation );
			animation = 0L;
			}
		}
	
	return animation;
}


short
_DisposeAnimation ( Animation * animation )
{
SegaErr err;
	
	if ( animation )
	{
		if ( animation->timer )
			{
			StopAnimation ( (AnimationRef) animation );
			}
			
		err = (*animation->proc) ( kDisposeAnimationProc, 0L, animation->data, &animation->procRef );
		ASSERT_MESG ( !err, "Error disposing animation p