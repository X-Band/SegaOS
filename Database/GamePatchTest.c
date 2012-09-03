/*
	File:		GamePatchTest.c

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<28>	 8/26/94	HEC		Game patch color-cycles background on sega to tell that it's
									active.
		<27>	 8/24/94	SAH		Use 19 bit packet format.
		<26>	 8/24/94	HEC		Mods to game dialog logic.
		<25>	 8/23/94	HEC		Don't enable game dialogs for now.
		<24>	 8/23/94	SAH		Added game results.
		<23>	 8/23/94	HEC		Deal with simulator game dialogs.
		<22>	 8/21/94	HEC		Fixed more poo related to patch struct change.
		<21>	 8/21/94	HEC		Fixed patch structure based on new Britt style struct.
		<20>	 8/13/94	HEC		Fixed memory size alloc bug.
		<19>	  8/6/94	HEC		Display message selector returns err to GameDoDialog.
		<18>	  8/6/94	HEC		Checked in Shannon's synching and state fixes.
		<17>	  8/6/94	SAH		Latest and greatest.
		<16>	  8/5/94	SAH		Added some dbugging code. Close the session when we're done with
									it.
		<15>	  8/5/94	SAH		Debugging poo.
		<14>	  8/4/94	SAH		Killed resume game. Added kGPReportError.
		<13>	  8/3/94	SAH		Shortened game time and fixed ROMDB problems.
		<12>	  8/2/94	SAH		Added a gametalk session.
		<11>	  8/1/94	HEC		More debugging.
		<10>	 7/31/94	HEC		Added test to call checkline continuously while playing the game
									for call waiting and other problems.
		 <9>	 7/24/94	SAH		More stable for stimulator.
		 <8>	 7/23/94	SAH		Don't call some stuff for the stimulator.
		 <7>	  7/8/94	CMY		Call CheckCodeForJTRefs for the ROMDB build.
		 <6>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <5>	  7/5/94	SAH		When adding to the stimulator database, cannot do the think c
									debugging jump trick.
		 <4>	  7/4/94	SAH		Conditionalized some stuff for simulated game patches.
		 <3>	 6/20/94	SAH		Psyq compatable version for the masses.
		 <2>	 6/18/94	GOD		All transitions working.
		 <1>	 6/18/94	SAH		first checked in

	To Do:
*/




#ifdef	ROMDB
#undef	SIMULATOR
#endif

//#define BUTTON_SPIN_DEBUG 1

#include "SegaOS.h"
#include "OSGlobals.h"
#include "GamePatch.h"
#include "Heaps.h"
#include "Errors.h"
#include "MungeCode.h"
#include "hardDefines.h"
#include "NetErrors.h"
#include "SegaVDP.h"
#include "Patchdb.h"

#ifdef SIMULATOR
#include <stdio.h>
#endif

typedef
struct GamePatchTestState
{
	long	sendData;
	long	startTime;
} GamePatchTestState;


long *	GetStoragePtr ( void );
long	TestPatchIntro ( long command, long data );
Ptr		GetGamePatchPtr ( void );
void	GamePatchHeader ( void );
void	PlayTheFuckingGame( void );
void	DoGameTalkTest( GamePatchTestState * state );
void	GameTestVBL ( void );

/*
*	This is a test game patch. it is written in c because i'm sick.
*/


#ifdef SIMULATOR
extern long	gMacTempHeap;
#else
#define	gMacTempHeap	0xff0000
#endif


/* fake out a game patch header */

void GamePatchHeader ( void )
{
	asm
		{
		dc.l	666
		dc.l	1
		dc.l	1
		dc.l	0
		dc.w	0		// the second half of this long is the rts. it's fixed by the install code
		}
}

long TestPatchIntro ( long command, long data )
{
long *					storage;
GamePatchTestState *	state;

	switch ( command )
		{
		case kGPInitGamePatch:
			state = NewMemory ( kPerm, sizeof(GamePatchTestState) );
			state->sendData = 0;
			state->startTime = gTicks;
			
			storage = GetStoragePtr();
			*storage = (long) state;
			break;
		case kGPStartGamePatch:
#ifdef SIMULATOR
					printf ( "STARTING GAME\n" );
					printf ( "STARTING GAME\n" );
					printf ( "STARTING GAME\n" );
					printf ( "STARTING GAME\n" );
					printf ( "STARTING GAME\n" );
					printf ( "STARTING GAME\n" );
#endif
			(*(GamePatchTestState **) GetStoragePtr())->sendData = 0;
			PlayTheFuckingGame();
			break;
		case kGPDisplayMessage:
#ifdef SIMULATOR
			return kGPCannotDrawDialogErr;	// comment out this line to test game dialogs
		{
			GPDialogRec *gpd = (GPDialogRec *)data;
			
				switch (gpd->command )
				{
					case kGPDDisplayCmd:
						printf("GAME DIALOG TEXT: %s\n", gpd->message);
						if (gpd->respA) printf("GAME DIALOG RESP A: %s\n", gpd->respA);
						if (gpd->respB) printf("GAME DIALOG RESP B: %s\n", gpd->respB);
						if (gpd->respC) printf("GAME DIALOG RESP C: %s\n", gpd->respC);
						break;
					case kGPDRemoveCmd:
						printf("GAME DIALOG REMOVED\n", gpd->message);
						break;
					case kGPDIdleCmd:
						break;
					default:
						break;
			}
			break;
		}
#else
			return kGPCannotDrawDialogErr;	// comment out for sega version to use game dialogs
#endif
			break;
		case kGPStopGame:
			break;
		case kGPReportError:
			break;
		default:
			break;
		}
	
	return 0;
}


void PlayTheFuckingGame( void )
{
long *					ptr;
short					count;
Ptr						transition;
long *					storage;
short *					trapVTable;
short					trap;
short					result;
GTSession * 			session;
GamePatchParamBlock *	params;

	params = (GamePatchParamBlock *) gGamePatchParams;
	session = params->commSession;
	
#define	kMappedRam	0x2a0000
#define	kMappedRom	0x200000

	/* first we erase all of temp mem */
	ptr = (long *) gMacTempHeap;
	for ( count = 0x3fff; count >= 0; --count )
		{
		*ptr++ = 0xFEEDBEEF;
		}

#ifdef NEVER
	/* now comes the really fun part. we want to remap the game shit to be all whacky */
	/* and then enjoy our new space. after that we want to return to "here" and go back to */
	/* the os */
	OSSetLEDs ( 33 );
	
	/* remap ram after rom */
	OSSetSafeRomSrc ( (Ptr) kMappedRom, (Ptr) (kMappedRom + 0x80000) );
	OSSetSafeRamSrc ( (Ptr) kMappedRam, (Ptr) (kMappedRam + 0x10000) );

#if 0
	/* set up our trap v table. it needs to be aligned on a 32 byte boundary */
	storage = GetStoragePtr();
	trapVTable = (short *) *(long *) storage;
	trapVTable = (short *) ( ((long) trapVTable + 128 ) & 0xffffffc0 );

	/* set the vector table address to be our trap table */
	OSSetVectorTblAddr ( (Ptr) trapVTable );
	
	/* fill in our traps */
	trap = 0x4e40;
	for ( count = 15; count >= 0; --count )
		{
		*trapVTable++ = trap++;
		}
#endif

	/* now we set vector 14 to be our transition address through which */
	/* we'll do the hard to soft here swap */
	asm
		{
		lea		@fucker,a0
		move.l	a0,transition
		}
		
	OSSetVector ( 14, transition );

	/* set the ram and rom offset to where we mapped them */
	gRamOffset = kMappedRam;
	gRomOffset = 0;

	asm
		{
		/* point at our register bases */
		move.l	OFFSET(SegaLowMem,controlRegisters),a0
		move.l	OFFSET(SegaLowMem,registerBase),a1

		/* reset the registers ptrs to where they'll be when the world's flipped */
		/* ==BRAIN DAMAGE== i set registerBase to itself because if i don't, it doesn't */
		/* work... */
		move.l	#kSoftOffset,OFFSET(SegaLowMem,controlRegisters)
		move.l	a1,OFFSET(SegaLowMem,registerBase)
		
		/* because rosko says so */
		move.b	#0xC0,kEnableHigh+1(a1)

		/* enable the safe rom region and internal registers */
		move.b	#(kRomHi + kEnSafeRom + kEnFixedInternal),kControlReg+1(a0)
				
		/* we need to clear kill here then do the transition */
		moveq	#0,d0
		move.b	d0,kKillReg+1(a0)

		lea		@fuckerReturn,a0
		add.l	#kMappedRam,a0
		move.l	a0,-(sp)
		lea		OFFSET(SegaLowMem,level1),a0
		move.w	#0x4e75,(a0)
		nop
		nop
@fucker	jmp (a0)
@fuckerReturn
		}	



	/* all we need to do now is come back here... */
	OSSetLEDs ( 0x11 );
#endif

#ifdef NEVER
	/* set our return transition address */
	asm
		{
		lea		@fuck2,a0
		move.l	a0,transition
		}
		
	OSSetVector ( 14, transition );
	
	asm
		{
		move.l	OFFSET(SegaLowMem,controlRegisters),a0
		move.l	OFFSET(SegaLowMem,registerBase),a1

		/* reset the registers ptrs to their here state */
		move.l	#kDefaultControl,OFFSET(SegaLowMem,controlRegisters)
		move.l	#kDefaultInternal,OFFSET(SegaLowMem,registerBase)

		/* we want to be back here */
		move.b	#kHereAssert,kKillReg+1(a0)

		lea		@homeReturn,a0
		sub.l	#kMappedRam,a0
		move.l	a0,-(sp)
@fuck2	rts
@homeReturn
		}
#endif

	/* set the ram and rom offset back */
	gRamOffset = 0;
	gRomOffset = 0;
	
	WriteVDPRegister( 1, 0x64 );	//enable Display, disable DMA, enable V interrupt

	DoGameTalkTest( *(GamePatchTestState **) GetStoragePtr() );
}


void	DoGameTalkTest( GamePatchTestState * state )
{
GamePatchParamBlock *	params;
Err						err;
long					local;
long					remote;
GTSession *				session;
long					ticks;
long					saveVBLHandler;
long					saveGTSession;
Boolean					successfulRead;
short * 				tempMem;
short					count;
#ifdef SIMULATOR
extern long 			gMacTempHeap;
#endif
short					color = 0;

	params = (GamePatchParamBlock *) gGamePatchParams;
	session = params->commSession;
	
	/* set up the game's vbl */
	gVBLHandler = (long) GameTestVBL;

	GTSession_SetGTPacketFormat ( session, k19BitData );
	GTSession_FlushInput ( session );
	GTSession_SetLatency ( session, 4 );

#ifndef SIMULATOR
	/* turn the os's tick counter vbl back on */
	asm { move.w	#0x2300,sr };
#endif
	GTSession_EstablishSynch ( session, 0 );
	GTSession_PrefillFifo ( session, 69 );
	
	do
		{
		/* wait for a vbl */
		ticks = gTicks;
		while ( ticks == gTicks )
			;

#ifndef SIMULATOR
		WriteVDPRegister( 7, color++ );	//background color
		color &= 15;
#endif
		// we have not read this frame's controller yet
		successfulRead = false;
		
		err = GTSession_ReadController ( session, &local, &remote );
		if ( err > 0 )
			{
			err = noErr;
			}
		
		switch ( err )
			{
			case noErr:
				// we got this frame's poo
				successfulRead = true;
				
				if ( local != remote )
					{
#ifdef SIMULATOR
					printf( "Packet synch error: local %ld, remote %ld\n", local, remote );
#endif
					}
				break;
			
			case kNoData:
				/* spin as long as one vbl for the data. if we still don't have it then, we have an error */
				ticks = gTicks;
				while ( ticks == gTicks && err == kNoData )
					{
					err = GTSession_ReadController ( session, &local, &remote );
					if ( err > 0 )
						{
						err = noErr;
						// we got this frame's poo
						successfulRead = true;
						if ( local != remote )
							{
#ifdef SIMULATOR
							printf( "Late Packet synch error: local %ld, remote %ld\n", local, remote );
#endif
							}
						}
					}
				
				/* if we still didn't get the packet try to recover */
				if ( err != noErr )
					{
#ifdef SIMULATOR
					printf ( "No Data error recover\n" );
#endif
					err = GTSession_ErrorRecover ( session, err, 1000 );
#ifdef SIMULATOR
					if ( err != noErr )
						{
						printf ( "Error Recover returned %ld\n", err );
						}
#endif
					}
				
				/* if the other guy closed the link, then we bail too */
				if ( err == kLinkClosed )
					{
					OSGameOver();
					}
					
				/* if we still have an error, then we suck */
				if ( err != noErr )
					{
#ifdef SIMULATOR
					printf ( "Bailing from no data with error %ld\n", err );
#endif
					OSHandleGameError ( err );
#ifdef SIMULATOR
					printf ( "Returning to game\n" );
#endif
					/* if we came back to here, the error is cleared */
					err = noErr;
					}
				break;
			
			default:
#ifdef SIMULATOR
				printf ( "Read controller error %ld\n", err );
#endif
				err = GTSession_ErrorRecover ( session, err, 1000 );
				
#ifdef SIMULATOR
				if ( err != noErr )
					{
					printf ( "Error recover returned %ld\n", err );
					}
#endif

				/* if the other guy closed the link, then we bail too */
				if ( err == kLinkClosed )
					{
					OSGameOver();
					}

				/* other errors go through HandleGameError */
				if ( err != noErr )
					{
#ifdef SIMULATOR
					printf ( "Bailing with error %ld\n", err );
#endif
					OSHandleGameError ( err );
#ifdef SIMULATOR
					printf ( "Returning to game\n" );
#endif
					
					/* if we got back to here, the error is cleared */
					err = noErr;
					}
				break;
			}
		
		/* if we read some data this frame, then send it, otherwise loop around */
		/* and read again */
		if ( successfulRead )
			{
			GTSession_SendController ( session, state->sendData++ );
			successfulRead = false;
			}
		}
	while ( err == noErr && state->sendData < 1000 );

	/* force close the session */
	OSGTSession_CloseSessionSynch ( session, 0 );
	
	/* send some game results */
	
#ifdef SIMULATOR
	tempMem = (short *) gMacTempHeap;
#else
	tempMem = (short *) 0xff0000;
#endif

	params->results.dbIDDataPtr = (long) tempMem;
	params->results.dbIDDataSize = 0x100;
	if ( params->isMaster )
		{
		params->results.localPlayer1Result = 200;
