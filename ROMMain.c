/*
	File:		ROMMain.c

	Contains:	Main entry point for sega rom build.

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<21>	 8/29/94	ADS		Better flagging of early-boot errors
		<20>	 8/27/94	JBH		Converted gBootSuccessful to gBootAttempts, so we can interrupt
									a boot process with a reset.
		<19>	 8/25/94	SAH		Do the sega stuff in ROMLoader.
		<18>	 8/24/94	ADS		Flag when we zap the RAM, and why
		<17>	 8/23/94	SAH		If BootOSVector rebuilds the os on a game restart, jump to the
									main rom boot rather than returning to space.
		<16>	 8/23/94	ADS		Remove no-longer-valid jumpvec check in boot vector
		<15>	 8/21/94	JOE		Add another instruction to ensure that VBLs are disabled at
									startup
		<14>	 8/20/94	JBH		(Joe) add an instruction to insure that HBLs (hell, VBLs too)
									are shut off early in the boot
		<13>	 8/19/94	HEC		Pass param to DoIntroStuff
		<12>	 8/18/94	ADS		Hide gBoxID globals
		<11>	  8/7/94	ADS		Blast boxID checksum if gnarly keys held down
		<10>	  8/6/94	ADS		Much more difficult power-on-clear combos
		 <9>	 7/11/94	SAH		Added BootOS and BootOSVector for external callers.
		 <8>	  7/2/94	SAH		Boot through gOSBootVector.
		 <7>	 6/20/94	SAH		Pass not in game to StartupSega.
		 <6>	 6/18/94	GOD		Reset the os globals stack on boot.
		 <5>	 6/15/94	SAH		Latest poo for new hardware.
		 <4>	 6/14/94	SAH		Added some stuff.
		 <3>	  6/9/94	SAH		Restructured to be a little smarter. Added a means to reset the
									os/database/everything by holding down buttons on reset.
		 <2>	  6/4/94	SAH		Fixed some after game restart problems.
		 <1>	 5/31/94	SAH		first checked in

	To Do:
*/




/*
*
* This is the main boot file for the Sega ROM build.
*
*/

#include "SegaOS.h"
#include "intro.h"
#include "Exceptions.h"
#include "OSGlobals.h"
#include "SegaIn.h"
#include "Controls.h"
#include "RoskoSega.h"
#include "ROMMain.h"
#include "BoxSerPriv.h"		// so we can peek gBoxID for trashing purposes


//-- special stuff for hard-linked controller read

ControllerType	_InitControllers( void );
void			_ReadAllControllers( ControllerType type, short* results, short flags );
 
 // these are completely arbitrary combinations
 // they are simply intended to be HARD to accidentally do!
 
#define kKillRAM1	(kDOWN + kLEFT + kButtonA + kButtonC)
#define kKillRAM2	(kButtonA + kButtonB + kStart)

#define kKillOS1	(kRIGHT + kButtonB + kButtonC)
#define kKillOS2	(kUP + kStart)

#define kKillDB1	(kUP + kRIGHT + kButtonB + kButtonC)
#define kKillDB2	(kDOWN + kButtonA + kButtonC)



extern	void	DispatchToProc ( void );
extern	void	InitTestRamDatabase ( void );
extern	void	LoaderMain ( void );

void	DefaultException ( void );
void	DispatchToProc ( void );


typedef	Boolean	(*StartupProc) ( Boolean gameRestart );


main( short fucker )
{
short	generateLinkFucker;

	asm
		{
// this shit puts the psyq in writeable mode
//		moveq		#0,d0
//		dc.w		0xa008
		
		/* reset the stack to unfuck the sega crap. (sega seems to like to put the stack */
		/* at 0xfffff200). We may want to think about moving ours... */
		
		move.l		#0,sp
		move.l		sp,a6
		move.l		sp,usp
		link		A6,#0x0000
		}

	/* call the real os startup code. we should never return from here... */
	BootStrap ();
			
	/* reference these so they goes in the link map */
	DefaultException();
	DispatchToProc();
}


/*
*	This guy does the real boot.
*/
void
BootStrap ( void )
{
short	generateLinkFucker;

	/* start the os -- game is not active */
	BootOS ( false );
	
	/* start the user interface -- never return from here */
	DoIntroStuff( false );

}



void BootOS ( Boolean gameRestart )
{
short			controls[5];
ControllerType	cntrl;

	asm
		{
		move.w		#0x8004,0xC00004		; ensure HBL ints are disabled
		move.w		#0x8104,0xC00004		; display disabled, VBL disabled, DMA disabled
		}
		
	// NOTE:  These combinations must be done with direct-connected
	// controllers.  This is because we are hitting this exactly at boot
	// time, and complex controllers like multi-taps won't have had
	// any time to reset themselves.
	
	cntrl = _InitControllers();			// setup and do a single controller read
	_ReadAllControllers( cntrl, controls, 0 );
	

	if ( controls[0] == kKillRAM1 && controls[1] == kKillRAM2 )
		{
		gRamTest1 = ~kRamTest1;		/* force a complete re-init of the os */
		gBoxID->checksum ^= 0xA5A5;
		}
	else
	if ( controls[0] == kKillOS1 && controls[1] == kKillOS2 )
		{
		gOSUnstable = true;			/* kill the OS heap */
		}
	else
	if ( controls[0] == kKillDB1 && controls[1] == kKillDB2 )
		{
		gDataBaseUnstable = true;	/* kill the database */
		}
		
	/* make sure that our globals stack is ok before we call anyone */
	/* this is assuming that the os is ok, if not this won't matter */
	gGlobalStack = gGlobalStackBase;
	
	/* now call the real guy to boot through the os vector */
	BootOSVector ( gameRestart );
}

void BootOSVector ( Boolean gameRestart )
{
long			bootVector;
Boolean			osVectorOk;
StartupProc		bootProc;

	osVectorOk = true;
		
	/* since we boot the os through a vector, we need to verify some state */
	/* before we hop off through the boot vector into never never land */
	if ( gRamTest1 != kRamTest1 || gRamTest2 != kRamTest2 )
	{
		osVectorOk = false;
		gBootAttempts = 0;
		SETOSSTATE(kEarlyRamCk);
	}
	
	
	/* if we have rebooted more than 100 times (or this value is uninitialized), we're fucked */
	if ( gBootAttempts > 100 )
	{
		gBootAttempts = 110;		/* some value so that if we reboot, we'll nuke for sure */
		osVectorOk = false;
		SETOSSTATE(kTooManyBoots);
	}
	else
	{
		gBootAttempts++;
	}
	
	/* check the ptr to see if it's ok */
	if ( gOSBootVector & 0x1 )
		{
		osVectorOk = false;
		SETOSSTATE(kBadBootVector);
		}
	
	/* if the address is not in ROM and not in SRAM it's definitely fucked */
	if ( ( !VALIDSRAMADDRESS(gOSBootVector) && !ADDRESSINROM(gOSBootVector) ) )
		{
		osVectorOk = false;
		SETOSSTATE(kBadBootVector);
		}
	
	/* if it's ok so far, dereference it and see if it contains our magic pooh */
	if ( osVectorOk )
		{
		/* get at the vector in C as it's easier than in assembly */
		bootVector = gOSBootVector;
		
		/* we verify that the vector is ok by looking for some magic code at the beginning */
		/* of the boot function */
		asm
			{
			move.l	bootVector,a0
			
	// This stuff is no longer valid because Yerga's tool has a mind
	//  of it's own.
	
//			// because of Yerga's tool, look for a jmp to the real thing
//			move.w	(a0)+,d0
//			cmp.w	#0x4ef9,d0
//			bne.s	@badBoot
//			
//			// now grab the address of the real thing
//			move.l	(a0)+,a0
			
			// look for a link instruction (skip the parameter word)
			move.w	(a0)+,d0
			cmp.w	#0x4E56,d0
			bne.s	@badBoot
			addq	#2,a0				// skip parameter
			
			// now look for our magic sequence
			lea		@magic,a1
			lea		@magicEnd,a2
			sub.l	a1,a2
			move.w	a2,d1
			lsr.w	#1,d1
			subq	#1,d1

@cmp		move.w	(a0)+,d0
			cmp.w	(a1)+,d0
			bne.s	@badBoot			// don't match, reset
			dbra	d1,@cmp
			bra.s	@doneCheck
			
@magic		bra.s	@magicEnd			// skip past our magic
			dc.b	"SHANNON IS A FUCKER"
@magicEnd
		
			// if we get here we know that the vector is bad...	
@badBoot	clr.b	osVectorOk
			}
			SETOSSTATE(kBadBootVector);
			asm {
@doneCheck
			}
		}
	
	/* if the boot vector is bad, reload it. we also mark ram as bad */
	if ( !osVectorOk )
		{
		gRamTest1 = ~kRamTest1;
		gOSBootVector = (long) StartupSega;
		}
	
	/* boot the os through the vector */
	bootProc = (StartupProc) gOSBootVector;
	(*bootProc) ( gameRestart );
	
	/* we need to check if we've wiped the os here. if we have, the globals stack is no longer valid */
	/* and we will return into space. so, the only safe  way to do this is check for an empty stack. */
	/* so, if the stack is empty, we just jump back to the beginning... */
	/* the only restart that cares about this is the game restart. */
	if ( gameRestart && gGlobalStack == gGlobalStackBase )
		{
		/* going to main restores the stack etc */
		main ( 666 );
		}
}
