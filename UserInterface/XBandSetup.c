/*
	File:		XBandSetup.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<16>	 8/25/94	CMY		Draw the radio buttons top to bottom.
		<15>	 8/24/94	HEC		Keyboard and call waiting options are now user-based.
		<14>	 8/22/94	HEC		Fixed calling from option to only display New Number if local
									access phone is zero. Was using box phone number before.
		<13>	 8/21/94	CMY		If user has never called (we don't know phone number), default
									to the IMoved option set and don't allow user to un-set it.
		<12>	 8/21/94	CMY		Sissy boys like their ABC's not their QWERTY's.
		<11>	 8/20/94	CMY		Added Accept Challenge option. Put description text at bottom of
									screen.
		<10>	 8/18/94	ADS		Hide gBoxID globals
		 <9>	 8/15/94	HEC		Fixed checkin bug.
		 <8>	 8/15/94	HEC		Checkin, damn it!
		 <7>	 8/12/94	JOE		Added movement/selection sounds
		 <6>	  8/5/94	CMY		Changed background. Changed fonts. Added jizzler.
		 <5>	  8/3/94	HEC		New look and feel includes "Calling From" button. Use BoxSer to
									set/get options now.
		 <4>	 7/29/94	KON		Added decompressOptions parameter to SetBackgroundID.
		 <3>	 7/26/94	CMY		Respond to A, B, and C buttons instead of just the Start button.
		 <2>	 7/23/94	HEC		Selections are now saved into the constants database for all to
									use and love.
		 <1>	 7/14/94	CMY		first checked in

	To Do:
*/



#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "vdp.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "Errors.h"
#include "GraphicsDB.h"
#include "DITLMgr.h"
#include "DITLItemSetup.h"
#include "StringDB.h"
#include "TextUtls.h"
#include "Mail.h"
#include "SegaIn.h"
#include "SegaOS.h"
#include "Screens.h"
#include "Time.h"
#include "Events.h"
#include "Heaps.h"
#include "Backdrops.h"
#include "Keyboard.h"
#include "MungeCode.h"
#include "PlayerDB.h"
#include "Utils.h"
#include "DialogMgr.h"
#include "Personification.h"
#include "RadioButtons.h"
#include "BoxSer.h"
#include "SegaSound.h"
#include "SegaCluts.h"

/* Fields for the XBand Setup screen */

enum {
	kAcceptChallengesButton,
	kCallWaitingButton,
	kIMovedButton,
	kKeyboardButton,
	kLastButton
};

typedef struct {
	short				activeButton;
	long				titlePatterns;
	long				descriptionPatterns;
	segaCharRect		descriptionRect;
	TextBoxReference	jizzle;
	GraphicReference	bgGraphic;
	GraphicReference	lineGraphic;
	Boolean				descriptionInvalid;
	Boolean				newPhoneNumber;
	RadioButtonRef		radioRefs[kLastButton];
	DITLItemList 		*theDitlList;
	ControlTable 		*myControlTable;
} XSetupGlobals;

#define kSetupTitleString 0
#define kSetupDescriptionRect 1

long XSetupPreflight( long initValue );
void XSetupReset( long refCon );
long XSetupSetup( ScreenParams *params );
long XSetupCommand( long command, long refCon, ScreenParams *params );
void XSetupKill( long refCon );

static long XSetupDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = XSetupPreflight( params->initValue );
			break;

		case kDoScreenReset:
			XSetupReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = XSetupSetup( params );
			break;

		case kDoScreenCommand:
			result = XSetupCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			XSetupKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in XSetup Dispatch" );
			break;
			
		}
	return result;
}

void XSetupReset( long refCon )
{
XSetupGlobals	*setup;

	setup = (XSetupGlobals *)refCon;

}

long XSetupPreflight( long initValue )
{
	return 0;
}

long XSetupSetup( ScreenParams *params )
{
XSetupGlobals	*setup;
Point 			*xPosition;
segaCharRect	jizzleRect;
char			*phoneString;
phoneNumber		*local1, *local2;

	setup = (XSetupGlobals *)NewMemory( kTemp, sizeof(XSetupGlobals) );
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );
	SetCurrentDevice( kScrollA );

	SetCurFont( kXBandHeavy );
	SetFontColors( 0, kDarkYellowColor, kYellowColor, 0 );
	setup->titlePatterns = DrawScreenLayoutString( kXBandSetupScreen, kSetupTitleString );
	setup->bgGraphic = DrawDBGraphicAt( 0, kSettingsIcon, 29, 2, kScrollA );
	setup->lineGraphic = DrawDBGraphicAt( 0, kGrayHorizontalLine, 5, 18, kScrollA );
	
	GetScreenLayoutCharRect( kXBandSetupScreen, 0, &jizzleRect );
	setup->jizzle = DrawTextBox( &jizzleRect, 8, kScrollA );
	StartTextBoxAnimation( setup->jizzle, 10 );

	GetScreenLayoutCharRect( kXBandSetupScreen, kSetupDescriptionRect, &setup->descriptionRect );
	setup->descriptionPatterns = LinearizeScreenArea( &setup->descriptionRect, 0 );

// Accept Challenges:
	setup->radioRefs[kAcceptChallengesButton] = SetupRadioButton( kAcceptChallengesButton+1, phoneString );
	RadioButtonSetSelection(setup->radioRefs[kAcceptChallengesButton], GetAcceptChallengesOption( GetCurUserID() ) );
	DrawRadioButton( setup->radioRefs[kAcceptChallengesButton] );

// Call Waiting:
	setup->radioRefs[kCallWaitingButton] = SetupRadioButton( kCallWaitingButton+1, nil );
	RadioButtonSetSelection(setup->radioRefs[kCallWaitingButton], GetCallWaitingOption( GetCurUserID() ) );
	DrawRadioButton(setup->radioRefs[kCallWaitingButton]);

// Calling From:
	phoneString = (char *) GetBoxPhoneNumber()->phoneNumber;
	GetLocalAccessPhoneNumber(&local1, &local2);
	if ( local1->phoneNumber[0] == 0 )
	{
		phoneString = GetSegaString(kBoxPhoneNumber);
		setup->newPhoneNumber = true;
	}
	else
		setup->newPhoneNumber = false;
		
	setup->radioRefs[kIMovedButton] = SetupRadioButton( kIMovedButton+1, phoneString );
	RadioButtonSetSelection(setup->radioRefs[kIMovedButton],
								setup->newPhoneNumber || GetIMovedOption() );
	DrawRadioButton( setup->radioRefs[kIMovedButton] );

// Keyboard Type:
	setup->radioRefs[kKeyboardButton] = SetupRadioButton( kKeyboardButton+1, nil );
	RadioButtonSetSelection(setup->radioRefs[kKeyboardButton], GetQwertyKeyboardOption( GetCurUserID() ) );
	DrawRadioButton( setup->radioRefs[kKeyboardButton] );

// Set active button
	setup->activeButton = kAcceptChallengesButton;
	ActivateRadioButton(setup->radioRefs[setup->activeButton]);
	VDPIdle();

// Set up the DITL
	setup->theDitlList = SetupDITLItemList( kXBandSetupScreen );	
	setup->myControlTable = SetupControlTable( kXBandSetupScreen );
	NewDITL( setup->myControlTable, setup->theDitlList );

// Invalidate the description text
	setup->descriptionInvalid = true;
	
	return (long) setup;
}

#define kPadWidth 238
static long ShowDescriptionString( XSetupGlobals *setup, DBID stringID )
{
	char *string = GetSegaString( stringID );
	
	if (string)
	{
		SetupTextGDevice( &setup->descriptionRect, setup->descriptionPatterns );
		SetCurFont( kXBandBold10Font );
		SetFontColors( 0, kDarkGreenColor, kGreenColor, 0 );
		DrawPaddedClippedSegaText( 0, 0, kPadWidth, kJustCenter, string );
	}
}

long XSetupCommand( long command, long refCon, ScreenParams *params )
{
Boolean			done;
XSetupGlobals	*setup;
short			result;
long			sel;

	setup = (XSetupGlobals *)refCon;
	result = 0;
		
	GiveDITLTime( setup->myControlTable, setup->theDitlList, command );
	
	if (	setup->newPhoneNumber &&
			setup->activeButton == kIMovedButton &&
			(command & (kRIGHT | kLEFT)) )
		return 0;
		
	if ( command & (kRIGHT | kLEFT | kUP | kDOWN) )
	{		
		PlayDBFX ( kButtonMoveSnd, 0, 0 );
		setup->descriptionInvalid = true;
	}
	
	if (command & kRIGHT)
		RadioButtonSelectNext(setup->radioRefs[setup->activeButton]);
	else if (command & kLEFT)
		RadioButtonSelectPrevious(setup->radioRefs[setup->activeButton]);
	else if ( (command & kUP) && (setup->activeButton > 0) )
	{
		DeactivateRadioButton(setup->radioRefs[setup->activeButton]);
		--setup->activeButton;
		ActivateRadioButton(setup->radioRefs[setup->activeButton]);
	}
	else if ( (command & kDOWN) && (setup->activeButton < kLastButton-1) )
	{
		DeactivateRadioButton(setup->radioRefs[setup->activeButton]);
		++setup->activeButton;
		ActivateRadioButton(setup->radioRefs[setup->activeButton]);
	}
	else if (command & (kStart | kButtonA | kButtonB | kButtonC))
	{
		// save off state of options to database
		sel = RadioButtonGetSelection(setup->radioRefs[kCallWaitingButton]);
		SetCallWaitingOption( GetCurUserID(), sel);
	
		sel = RadioButtonGetSelection(setup->radioRefs[kKeyboardButton]);
		SetQwertyKeyboardOption( GetCurUserID(), sel);
	
		sel = RadioButtonGetSelection(setup->radioRefs[kIMovedButton]);
		SetIMovedOption(sel);
		
		sel = RadioButtonGetSelection(setup->radioRefs[kAcceptChallengesButton]);
		SetAcceptChallengesOption( GetCurUserID(), sel );
		
		PlayDBFX ( kButtonSelectSnd, 0, 0 );

		result = 1;
	} else
		RadioButtonIdle(setup->radioRefs[setup->activeButton]);

		
	if ( setup->descriptionInvalid )
	{
		DBID stringID;
		
		switch ( setup->activeButton )
		{
			case kAcceptChallengesButton:
				stringID = RadioButtonGetSelection(setup->radioRefs[kAcceptChallengesButton]) ? 
													kAcceptChallengesYES : kAcceptChallengesNO;
				ShowDescriptionString( setup, stringID );
				break;
				
			case kCallWaitingButton:
				stringID = RadioButtonGetSelection(setup->radioRefs[kCallWaitingButton]) ? 
													kCallWaitingON : kCallWaitingOFF;
				ShowDescriptionString( setup, stringID );
				break;

			case kIMovedButton:
				stringID = RadioButtonGetSelection(setup->radioRefs[kIMovedButton]) ? 
													kCallingFromNEW : kCallingFromSAME;
				ShowDescriptionString( setup, stringID );
				break;

			case kKeyboardButton:
				stringID = RadioButtonGetSelection(setup->radioRefs[kKeyboardButton]) ? 
													kKeyboardTypeQWERTY : kKeyboardTypeABCD;
				ShowDescriptionString( setup, stringID );
				break;
		}
		
		setup->descriptionInvalid = false;
	}
	
	VDPIdle();
	return result;
}

void XSetupKill( long refCon )
{
XSetupGlobals	*setup = (XSetupGlobals *)refCon;
short			ii;

	for (ii = 0; ii<kLastButton; ii++)
		DisposeRadioButtonRef(setup->radioRefs[ii]);

	FreePatternBlock( setup->titlePatterns );
	FreePatternBlock( setup->descriptionPatterns );
	DisposeGraphicReference( setup->bgGraphic );
	DisposeGraphicReference( setup->lineGraphic );
	DisposeTextBoxReference( setup->jizzle );

	DisposeDITL( setup->theDitlList );
	DisposeControlTable( setup->myControlTable );
	DisposeDITLItemList( setup->theDitlList );

	DisposeMemory((Ptr)setup);
}

Ptr GetXSetupDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) XSetupDispatch + 2));
	size = (*(long *) ((Ptr) GetXSetupDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for XBand Setup screen proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) XSetupDispatch;
#else
		CheckCodeForJTRefs((void *)XSetupDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
