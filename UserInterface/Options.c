/*
	File:		Options.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<34>	 8/18/94	HEC		Clear "None" card background when drawing credits over it.
		<33>	 8/15/94	HEC		Fixed bug where "None" would not be displayed after card is
									removed.
		<32>	 8/12/94	HEC		Only read smartcard credits once when card is inserted to speed
									interface up.
		<31>	  8/8/94	KON		Draw account icon in scrollB so text doesn't interfere with it.
		<30>	  8/8/94	KON		Remove ClearBackdrop.
		<29>	 7/29/94	KON		Add decompressor flags to SetbackdropID.
		<28>	 7/26/94	CMY		Fixed display of Smart Card credits.
		<27>	 7/25/94	JOE		fuq
		<26>	 7/25/94	JOE		set SmartCard text palette properly
		<25>	 7/25/94	JOE		Gave it the right background
		<24>	 7/22/94	JOE		MeasureSegaText() for SmartCard credits text
		<23>	 7/22/94	JOE		Joey-ified
		<22>	 7/22/94	JOE		My eye!
		<21>	 7/22/94	JOE		SmartCard credits now displays "No Card" if no card is present
		<20>	 7/17/94	JBH		Fixed leak of pattern memory.
		<19>	 7/17/94	JOE		...and I call it twice
		<18>	 7/17/94	JOE		ted changed GetRemainingCredits()
		<17>	 7/17/94	JOE		make smartcard credits live
		<16>	 7/17/94	JOE		update
		<15>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		<14>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<13>	  7/4/94	ADS		New interface for screen control
		<13>	  7/4/94	ADS		New interface for screen control
		<12>	 6/21/94	KON		Don't free pattern block that's not getting allocated since this
									is a CES demo version.
		<11>	 6/20/94	KON		Update for CES options screen.
		<10>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <9>	 6/13/94	KON		Make selection parameter to screens a long.
		 <8>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <7>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		 <6>	 6/11/94	KON		Move referenced functions inside the code block.
		 <5>	 6/10/94	KON		Updated to support new screen dispatcher calls for preflight and
									reset.
		 <4>	 6/10/94	KON		Added a VDPIdle so the shit draws.
		 <3>	 6/10/94	KON		Updated to use new screen dispatch model. Needs the three button
									intermediate screen added back in.
		 <2>	  6/5/94	KON		Always use main2bit font and added the Camplejohn string.
									Blasting out for Digital World.
		 <1>	  6/5/94	CMY		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "SegaOS.h"
#include "SegaBM.h"
#include "SegaVDP.h"
#include "vdp.h"
#include "sega.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "SegaIn.h"
#include "DitlMgr.h"
#include "UsrConfg.h"
#include "TextButt.h"
#include "DBTypes.h"
#include "Events.h"
#include "GraphicsDB.h"
#include "SegaTraps.h"
#include "StringDB.h"
#include "DITLItemSetup.h"
#include "Options.h"
#include "Time.h"
#include "Screens.h"
#include "Errors.h"
#include "Heaps.h"
#include "MungeCode.h"
#include "Backdrops.h"
#include "StatText.h"
#include "SmartCard.h"
#include "TextUtls.h"
#include "OptionsPriv.h"

void DrawSmartCardCredits( OptionsRefCon *optionGlobals, short numCreds );


//
// Core options screen routines
//
static long OptionsDispatch( short action, long command, long *refCon, ScreenParams *params );
static long OptionsPreflight( long initValue );
static void OptionsReset( long refCon );
static long OptionsSetup( ScreenParams *params );
static long OptionsCommand( long command, long refCon, ScreenParams *params );
static void OptionsKill( long refCon );

static long OptionsDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = OptionsPreflight( params->initValue );
			break;

		case kDoScreenReset:
			OptionsReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = OptionsSetup( params );
			break;

		case kDoScreenCommand:
			result = OptionsCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			OptionsKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Options Dispatch" );
			break;
			
		}
	return result;
}

static void OptionsReset( long refCon )
{
OptionsRefCon	*optionGlobals;

	optionGlobals = (OptionsRefCon *) refCon;
	ClearDITLDone( optionGlobals->theDitlList );
}

static long OptionsPreflight( long initValue )
{
	return 0;
}

static long OptionsSetup( ScreenParams *params )
{
OptionsRefCon	*optionGlobals;
CreditRecord *cred = (CreditRecord *)DBGetItem( kCreditsDBType, 0 );
RestrictionsRecord *rest = (RestrictionsRecord *)DBGetItem( kRestrictionsDBType, 0 );
char 			acctStr[8];
char 			smartCardStr[8];
char			usedCredStr[8];
StatTextDesc	*STDPtr;
TextButtonDesc	*TBDPtr;
short			screds;
xyString		*s;
segaCharRect 	myRect;

	optionGlobals = (OptionsRefCon *) NewMemory( kTemp, sizeof(OptionsRefCon) );
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kType2PlainBackground, true, 0 );

	optionGlobals->theDitlList = SetupDITLItemList( kAccountInfoScreen );	
	optionGlobals->myControlTable = SetupControlTable( kAccountInfoScreen );

	optionGlobals->graphRef = DrawDBGraphicAt( 0, kAccountInfoIcon, 30, 3, kScrollB );
// pound Credits Used string
	SegaNumToString( (long)cred->usedCredits, usedCredStr );
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kTotalCreditsUsed].objectData);
	STDPtr->myCString = usedCredStr;

// pound XBand Account string
	SegaNumToString( (long)cred->accountCredits, acctStr );
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kAccountCredits].objectData);
	STDPtr->myCString = acctStr;


// setup for SmartCard Credits AutoPound
	
	optionGlobals->textRect.top = ((optionGlobals->theDitlList)->items[kSmartCardCredits].yPos >> 3);
	optionGlobals->textRect.bottom = optionGlobals->textRect.top + 2;
	optionGlobals->textRect.left = ((optionGlobals->theDitlList)->items[kSmartCardCredits].xPos >> 3);
	optionGlobals->textRect.right = optionGlobals->textRect.left + 6;		// 6 cells wide
	

// pound Playing Field string
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kPlayingField].objectData);
	STDPtr->myCString = rest->playField;

// pound Hours of Play strings
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kHoursOfPlay1].objectData);
	STDPtr->myCString = rest->hours1;
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kHoursOfPlay2].objectData);
	STDPtr->myCString = rest->hours2;

// pound off the Return text
	s = (xyString *)DBGetItem( kStringType, 0 );		// ID 0 is the system null string
	TBDPtr = (TextButtonDesc *)((optionGlobals->theDitlList)->items[kReturnButton].objectData);
	TBDPtr->myCString = s->cString;
	

	NewDITL( optionGlobals->myControlTable, optionGlobals->theDitlList );
			

// finish setup for SmartCard Credits AutoPound

	SetCurrentDevice( kScrollA );
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kSmartCardCredits].objectData);
	optionGlobals->textPatterns = LinearizeScreenArea( &(optionGlobals->textRect), STDPtr->palette );
	
	screds = GetRemainingCredits(0);
	DrawSmartCardCredits( optionGlobals, screds );
	optionGlobals->cardDetected = 0;

// Jizzle

	myRect.left = 3;
	myRect.top = 2;
	myRect.right = 36;
	myRect.bottom = 25;
		
	optionGlobals->myTextBoxMems = DrawTextBox( &myRect, 14, kScrollA );			
	StartTextBoxAnimation( optionGlobals->myTextBoxMems, 20 );

	VDPIdle();							// let the shit update
	
	return (long)optionGlobals;

}

static long OptionsCommand( long command, long refCon, ScreenParams *params )
{
OptionsRefCon	*optionGlobals;
long			result;
Boolean			done;
short			screds;

	optionGlobals = (OptionsRefCon *) refCon;
	result = 0;
	
	if ( CardInstalled() == noErr) // installed
	{
		if (!optionGlobals->cardDetected) // and wasn't installed before
		{
			optionGlobals->cardDetected = 1;
			screds = GetRemainingCredits(0);
			DrawSmartCardCredits( optionGlobals, screds );
		}
	} else {	// not installed
		if (optionGlobals->cardDetected)	// but we were installed, draw "None"
			DrawSmartCardCredits( optionGlobals, -1 );
		optionGlobals->cardDetected = 0;
	}

	done = GiveDITLTime( optionGlobals->myControlTable, optionGlobals->theDitlList, command );
	if( done )
	{
		result = GetDITLItem( optionGlobals->myControlTable );
		result += 1;
	}

	return result;
}

static void OptionsKill( long refCon )
{
OptionsRefCon	*optionGlobals;

	optionGlobals = (OptionsRefCon *) refCon;

	StopTextBoxAnimation( optionGlobals->myTextBoxMems );
	DisposeTextBoxReference( optionGlobals->myTextBoxMems );

	DisposeDITL( optionGlobals->theDitlList );
	FreePatternBlock( optionGlobals->textPatterns );
	DisposeControlTable( optionGlobals->myControlTable );
	DisposeDITLItemList( optionGlobals->theDitlList );
	DisposeGraphicReference( optionGlobals->graphRef );
	DisposeMemory( (Ptr) refCon );
}


void DrawSmartCardCredits( OptionsRefCon *optionGlobals, short numCreds )
{
short			oldFont;
char			buffer[8];
StatTextDesc	*STDPtr;
xyString		*s;

	SetCurrentDevice( kScrollA );
	STDPtr = (StatTextDesc *)((optionGlobals->theDitlList)->items[kSmartCardCredits].objectData);
	
	oldFont = GetCurFont ();
	
	SetCurFont( STDPtr->font );
	SetupTextGDevice( &(optionGlobals->textRect), optionGlobals->textPatterns );
	SetFontColors( STDPtr->color0, STDPtr->color1, STDPtr->color2, STDPtr->color3 );

	if (numCreds >= 0)
	{
		SegaNumToString( numCreds, buffer );
		DrawPaddedClippedSegaText( (STDPtr->animationData).xPos, (STDPtr->animationData).yPos, 
											6*8, kJustifyLeft, buffer );
	}
	else
	{
		s = (xyString *)DBGetItem( kStringType, kNoSmartCardPresentString );
		DrawPaddedClippedSegaText( (STDPtr->animationData).xPos, (STDPtr->animationData).yPos, 
											6*8, kJustifyLeft, s->cString );
	}
	
	SetCurFont ( oldFont );
}



Ptr GetOptionsDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) OptionsDispatch + 2));
	size = (*(long *) ((Ptr) GetOptionsDispatchPtr + 2)) - srcFunction;
	data = NewMemory( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Options proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) OptionsDispatch;
#else
		CheckCodeForJTRefs((void *)OptionsDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}

