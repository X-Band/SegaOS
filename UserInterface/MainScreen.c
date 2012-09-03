/*
	File:		MainScreen.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<33>	 8/26/94	CMY		Don't orphan a blinky when the net register timeout dialog comes
									up.
		<32>	 8/24/94	KON		Dialog box which tells you how long you've been connected now
									says how much longer we'll be searching rather than the total
									search time.
		<31>	 8/24/94	CMY		Take out the delay code for the blinkies. What's so nasty about
									green?
		<30>	 8/20/94	KON		Only do blinky sprites when registered. Set the font before
									measuring the size of that nasty green "Your are registered"
									string and then comment it out.
		<29>	 8/19/94	CMY		Call LongDivide rather than the library functions         since
									we are inside a db item.
		<29>	 8/19/94	CMY		Added a string to indicate when the box is registered.
		<28>	 8/12/94	KON		Call LongDivide rather than the library functions since we are
									inside a db item.
		<27>	  8/5/94	JOE		Check for secret sequence to enable/disable Jizzlation
		<26>	  8/5/94	CMY		Use the blinky count konstant so we don't ask for non-existant
									blinies.
		<25>	  8/4/94	JBH		De-seeded RandomShort calls.
		<24>	  8/3/94	CMY		..
		<23>	  8/2/94	CMY		Dispose any active blinky in the screen kill.
		<22>	  8/2/94	CMY		Don't call _Write randomly.
		<21>	  8/2/94	CMY		The background animates when the user is idle for awhile!
		<20>	 7/30/94	CMY		Omit the fly-in animation on Simulator. Only do it the first
									time on Sega.
		<19>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<18>	 7/26/94	CMY		Added the fly-in button animations.
		<17>	 7/19/94	CMY		Ifdef'ed out code that was setting the mailbox icon, need to
									revisit this.
		<16>	 7/13/94	KON		Removed reference to kWaitingForGame box state.
		<15>	 7/11/94	KON		Set challenge request to random opponent before connecting to
									network.
		<14>	 7/11/94	KON		Remove redundant font setup.
		<13>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<12>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<11>	  7/6/94	KON		Don't allow going back to choose a player during network
									registration.
		<10>	  7/4/94	ADS		New interface for screen control
		 <9>	 6/20/94	KON		Get in box graphic now takes a screen parameter.
		 <8>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <7>	 6/13/94	KON		Make mail icon reflect amount of mail.
		 <6>	 6/13/94	KON		Make selection parameter to screens a long.
		 <5>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <4>	 6/12/94	SAH		Use backdrop manager and make pattern happy.
		 <3>	 6/11/94	KON		Remember cursor position.
		 <2>	 6/10/94	KON		Updated to support new screen dispatcher calls for preflight and
									reset.
		 <1>	 6/10/94	KON		first checked in

	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "DitlMgr.h"
#include "PlayerDB.h"
#include "SegaScrn.h"
#include "Pattern.h"
#include "DITLItemSetup.h"
#include "SegaText.h"
#include "StringDB.h"
#include "VDP.h"
#include "Heaps.h"
#include "Errors.h"
#include "SegaIn.h"
#include "Time.h"
#include "MailView.h"
#include "TextUtls.h"
#include "GraphicsDB.h"
#include "Secrets.h"
#include "MainScreenPriv.h"
#include "DBTypes.h"
#include "Screens.h"
#include "MungeCode.h"
#include "Backdrops.h"
#include "TextButt.h"
#include "DialogMgr.h"
#include "BoxSer.h"
#include "NetRegister.h"
#include "Challnge.h"
#include "Utils.h"
#include "SegaSound.h"
#include "Math.h"

static long MainScreenDispatch( short action, long command, long *refCon, ScreenParams *params );
static long MainScreenPreflight( long initValue );
static void MainScreenReset( long refCon );
static long MainScreenSetup( ScreenParams *params );
static long MainScreenCommand( long command, long refCon, ScreenParams *params );
static void MainScreenKill( long refCon );

static long MainScreenDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = MainScreenPreflight( params->initValue );
			break;

		case kDoScreenReset:
			MainScreenReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = MainScreenSetup( params );
			break;

		case kDoScreenCommand:
			result = MainScreenCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			MainScreenKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Screen Mail Dispatch" );
			break;

		}

	return result;
}

static void MainScreenReset( long refCon )
{
MainScreenRefCon	*mainScreenGlobals;

	mainScreenGlobals = (MainScreenRefCon *) refCon;
	ClearDITLDone( mainScreenGlobals->theDitlList );
}

static long MainScreenPreflight( long initValue )
{
	return 0;
}

static void DisplayRegisteredString( MainScreenRefCon *mainScreenGlobals )
{
#if 0
segaCharRect patternBlock;
short stringWidth;
char *string;
	
	ASSERT( mainScreenGlobals->registeredPatterns == 0 );
	
	SetCurFont( kMainFont );

	string = GetWriteableString( kYouAreRegisteredString );
	stringWidth = MeasureSegaText( string );
		
	patternBlock.top = 13;
	patternBlock.bottom = patternBlock.top + ( (GetCurFontHeight()+7+4) >> 3);
	stringWidth >>= 4;
	patternBlock.left = 19-stringWidth;
	patternBlock.right = 20+stringWidth;

	mainScreenGlobals->patternRect = patternBlock;
	mainScreenGlobals->registeredPatterns = LinearizeScreenArea( &patternBlock, 0 );
	SetupTextGDevice( &patternBlock, mainScreenGlobals->registeredPatterns );
	SetFontColors( 0, 9, 8, 0);
	
	CenterSegaText( 4, string );
#endif
}

static void RemoveRegisteredString( MainScreenRefCon *mainScreenGlobals )
{
#if 0
	FreePatternBlock( mainScreenGlobals->registeredPatterns );
	mainScreenGlobals->registeredPatterns = 0;
	
	if (mainScreenGlobals->clearPattern == 0)
		mainScreenGlobals->clearPattern = NewPatternBlock( 1 );
	
	FillNameTable( kScrollA, &mainScreenGlobals->patternRect,
		PATOFFSET(mainScreenGlobals->clearPattern) );
#endif
}

static long MainScreenSetup( ScreenParams *params )
{
MainScreenRefCon	*mainScreenGlobals;
DBID				myMailGraphicID;
AnimationRef		sliders[6];
long				index;
long				time;

	mainScreenGlobals = (MainScreenRefCon *) NewMemory( kTemp, sizeof(MainScreenRefCon) );

	EraseGDevice ( kScrollA );
	SetBackdropID ( kBackdrop, true, 0 );
	
#ifndef SIMULATOR
	if ( params->selection == 0 )
	{
	for (index=0; index<6; ++index)
	{
		sliders[index] = SpawnAnimation( kButtonAnimate, (long) DBGetItem( kButtonAnimationType, index ));
		StartAnimation( sliders[index] );
	}

	time = GetCurrentTime();
#ifdef SIMULATOR
	time += 120;
#else
		time += 95;
#endif

	while (GetCurrentTime() < time)
	{
		SegaOSIdle( kNonSpecificCode );
		VDPIdle();
	}
	}
#endif
	
	mainScreenGlobals->theDitlList = SetupDITLItemList( kTopLevelInterfaceScreen );	
	mainScreenGlobals->myControlTable = SetupControlTable( kTopLevelInterfaceScreen );

	ASSERT( mainScreenGlobals->theDitlList );
	ASSERT( mainScreenGlobals->myControlTable );
	
	mainScreenGlobals->myControlTable->selection = params->selection;	//initValue, curSelection
	
	NewDITL( mainScreenGlobals->myControlTable, mainScreenGlobals->theDitlList );

#ifndef SIMULATOR
	if ( params->selection == 0 )
	{
		for (index=0; index<6; ++index)
		{
			DisposeAnimation( sliders[index] );
		}
	}
#endif

	mainScreenGlobals->blinky = nil;
	mainScreenGlobals->blinkyTime = GetCurrentTime() + 60;
	
	mainScreenGlobals->jizzleDisableSeq = (SecretSequence *)DBGetItem( kSecretSequenceType, kJizzlationDisableSequence );
	
	mainScreenGlobals->registeredPatterns = 0;
	mainScreenGlobals->clearPattern = 0;
	
	return (long) mainScreenGlobals;
}


static long MainScreenCommand( long command, long refCon, ScreenParams *params )
{
MainScreenRefCon	*mainScreenGlobals;
long				result;
Boolean				done;
long				boxState;
long				totalWait;
char				totalWaitStr[10];
long				waitSoFar;
char				waitSoFarStr[10];
DBID				dialogID;
userIdentification	*myUserID;

	mainScreenGlobals = (MainScreenRefCon *) refCon;
	result = 0;					// default to keep looping

	params->i