/*
	File:		animation.h

	Contains:

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 7/25/94	CMY		Added GetAnimationSuspendLevel.
		 <7>	 7/16/94	dwh		fix include capitalization.
		 <6>	  7/6/94	SAH		Added kAnimationError.
		 <5>	  7/3/94	SAH		Added SpawnDBAnimation.
		 <4>	 6/29/94	CMY		Added SetAnimationGrayFlag.
		 <3>	 6/20/94	CMY		Added priorities to animations.
		 <2>	 6/18/94	CMY		Added calls to Suspend and Resume animations
		 <2>	 5/21/94	SAH		Added InitAnimateProcs and DrawAnimationFrame.
		 <1>	 5/12/94	SAH		first checked in

	To Do:
*/


#ifndef	__Animation__
#define	__Animation__

#ifndef __DataBase__
#include "Database.h"
#endif


/*
* user proc to drive animation. when playing the animation, it gets passed the current
* time in "time". it returns when it next wants to be called again in "time".
*/
typedef long (*AnimateProc) ( short command, long * time, long data, long * ref );

typedef long * AnimationRef;

/* animation proc commands */
enum
{
	kInitAnimationProc,
	kDisposeAnimationProc,
	kStartAnimationProc,
	kStopAnimationProc,
	kSetAnimationGrayProc,				// Make the animation grayscale/use the default palette
	kClearAnimationGrayProc,			// Make it color again/use custom clut if appropriate
	kDoAnimationEvent
};

/* animation priority levels */
enum
{
	kDefaultAnimationPriority = 0,		// New animations get this by default
	kDialogAnimationPriority,			// Dialogs have a higher priority
	kLEDAnimationPriority				// LEDs are the top, of course!
};

/* animation proc return values */
enum
{
	kAnimationOk,		// all is ok, call us again at "time"
	kAnimationError,	// the animation proc died
	kAnimationDone		// animation is over, dispose it
};


/* animation structure used to store whole animations in the dbase */
typedef
struct DBAnimation
{
	short	animationProc;
	char	data[1];
} DBAnimation;


#ifndef	__SERVER__
short	InitAnimateProcs ( void ) =
	CallDispatchedFunction( kInitAnimateProcs );

AnimationRef	SpawnAnimation ( short animatorID, long data ) =
	CallDispatchedFunction( kSpawnAnimation );

AnimationRef	SpawnDBAnimation ( DBID animationID ) =
	CallDispatchedFunction( kSpawnDBAnimation );

AnimationRef	CreateAnimation ( AnimateProc proc, long data ) =
	CallDispatchedFunction( kCreateAnimation );

short	DisposeAnimation ( AnimationRef animation ) =
	CallDispatchedFunction( kDisposeAnimation );

short	DrawAnimationFrame ( AnimationRef animation )=
	CallDispatchedFunction( kDrawAnimationFrame );

short	StartAnimation ( AnimationRef animation ) =
	CallDispatchedFunction( kStartAnimation );

short	StopAnimation ( AnimationRef animation ) =
	CallDispatchedFunction( kStopAnimation );

void	SuspendAnimations ( short minimumPriority ) =
	CallDispatchedFunction( kSuspendAnimations );

void	SetAnimationPriority ( AnimationRef animation, short priority ) =
	CallDispatchedFunction( kSetAnimationPriority );
	
void	SetAnimationGrayFlag ( AnimationRef animation, Boolean grayFlag ) =
	CallDispatchedFunction( kSetAnimationGrayFlag );
	
short	GetAnimationSuspendLevel( void ) =
	CallDispatchedFunction( kGetAnimationSuspendLevel );
	
#endif	/* ! __SERVER__ */

#endif
