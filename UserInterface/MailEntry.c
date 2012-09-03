/*
	File:		MailEntry.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<45>	 8/28/94	CMY		Use a custom line height to show the mail contents so that it's
									the same as the mail edit screen.
		<44>	 8/15/94	JBH		Disable deferred dialogs to avoid graphics problems. Show them
									at exit time, as we might be going to another screen with
									disabled deferred dialogs.
		<43>	 8/12/94	JOE		new icon stuff
		<42>	  8/9/94	CMY		Moved the town fields over 1 pixel.
		<41>	  8/8/94	KON		Remove ClearBackdrop.
		<40>	  8/4/94	CMY		Fixed a bug where "Write" would sometimes reply to a random
									user.
		<39>	  8/3/94	CMY		Added a line and revectored the DITL buttons.
		<38>	  8/3/94	CMY		Fixed backgrounds. Changed buttons. Changed jizzle rect.
		<37>	 7/30/94	JOE		Don't include MailViewPriv.h
		<36>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<35>	 7/22/94	KON		UI poop.
		<34>	 7/22/94	CMY		Added small envelope graphic.
		<33>	 7/22/94	KON		UI pooh.
		<32>	 7/22/94	CMY		Fixed jizzle rect
		<31>	 7/21/94	JOE		Added flags param to SegaBoxText()
		<30>	 7/21/94	KON		Add long date format.
		<29>	 7/20/94	KON		Joey turn.
		<28>	 7/19/94	KON		Update for jizzlin borders.
		<27>	 7/19/94	KON		Twiddle the graphics some.
		<26>	 7/18/94	KON		Adjust format for new screens.
		<25>	 7/18/94	KON		Add icon of from player.
		<24>	 7/14/94	KON		If box is net registered, don't allow user to update mail.
		<23>	 7/12/94	CMY		Save the piece of mail being read in screen state, so we can go
									to deeper screens and return with no probs.
		<22>	 7/12/94	KON		Add delete verification.
		<21>	 7/12/94	CMY		Set up the userID properly for the "Check Mail" button/
		<20>	 7/12/94	CMY		Added check mail button
		<19>	 7/11/94	CMY		Made the Reply button work.
		<18>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<17>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<16>	  7/4/94	ADS		New interface for screen control
		<15>	  7/3/94	KON		Fix delete, and add new mail entry bugs.
		<14>	  7/2/94	KON		Mark entries as read.
		<13>	  7/2/94	CMY		Just kidding...
		<12>	 6/30/94	CMY		Hooked 'er up to the keyboard manager.
		<11>	 6/20/94	KON		Update dialogs and look for CES.
		<10>	 6/16/94	KON		Remove unused stack variable.
		 <9>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <8>	 6/13/94	KON		Make selection parameter to screens a long.
		 <7>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <6>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		 <5>	 6/11/94	KON		Make delete key work in both view and entry screen.
		 <4>	 6/10/94	KON		Updated to support new screen dispatcher calls for preflight and
									reset.
		 <3>	 6/10/94	KON		Fix bug where NewDitl was being called twice in MailEntrySetup.
		 <2>	 6/10/94	KON		Update to use screen dispatcher.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "vdp.h"
#include "Mail.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "Errors.h"
#include "DITLMgr.h"
#include "MailCntl.h"
#include "Heaps.h"
#include "SegaIn.h"
#include "Events.h"
#include "TextUtls.h"
#include "StringDB.h"
#include "TextButt.h"
#include "GraphicsDB.h"
#include "Time.h"
#include "DITLItemSetup.h"
#include "MailEdit.h"
#include "MungeCode.h"
#include "Screens.h"
#include "MailEntryPriv.h"
#include "MailView.h"
#include "Backdrops.h"
#include "DialogMgr.h"
#include "Challnge.h"
#include "NetRegister.h"
#include "DeferredDialogMgr.h"

static long MailEntryDispatch( short action, long command, long *refCon, ScreenParams *params );
static long MailEntryPreflight( long initValue );
static void MailEntryReset( long refCon );
static long MailEntrySetup( ScreenParams *params );
static long MailEntryCommand( long command, long refCon, ScreenParams *params );
static void MailEntryKill( long refCon );

static long MailEntryDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = MailEntryPreflight( params->initValue );
			break;

		case kDoScreenReset:
			MailEntryReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = MailEntrySetup( params );
			break;

		case kDoScreenCommand:
			result = MailEntryCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			MailEntryKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Mail Entry Dispatch" );
			break;
			
		}
	return result;
}

static void MailEntryReset( long refCon )
{
MailEntryRefCon	*entryGlobals;

	entryGlobals = (MailEntryRefCon *)refCon;
	ClearDITLDone( entryGlobals->theDitlList );
}

static long MailEntryPreflight( long initValue )
{
	return 0;
}

static long MailEntrySetup( ScreenParams *params )
{
Mail *theMail;
Rect	textRect;
short	keyPress, player2;
short	ypos;
short	xpos;
MailEntryRefCon	*entryGlobals;
char			theDate[30];
xyString		*theXYString;
segaCharRect 	myRect;
long	patternStart;
char	*theCString;

	if (params->selection == 0)
	{
		params->selection = params->initValue;
		theMail = (Mail *) params->initValue;
	}
	else
	{
		theMail = (Mail *) params->selection;
	}
	entryGlobals = (MailEntryRefCon *)NewMemory( kTemp, sizeof(MailEntryRefCon) );

	entryGlobals->theDitlList = SetupDITLItemList( kMailRead );
	entryGlobals->myControlTable = SetupControlTable( kMailRead );
	entryGlobals->theMail = theMail;
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );
	SetCurrentDevice( kScrollA );

	entryGlobals->envelopeRef = DrawDBGraphicAt( 0, kXMailIcon, 29, 4, kScrollB );
	entryGlobals->line2 = DrawDBGraphicAt( 0, kGrayHorizontalLine, 5, 19, kScrollB );

	ShowDefDialogs( kDDMainEventLoop );

	DisableDefDialogs();

//
// Install a DITL to handle the buttons across the bottom:
// Reply, Delete, Done
//
	NewDITL( entryGlobals->myControlTable, entryGlobals->theDitlList );

	{
		entryGlobals->iconRef = DrawPlayerIcon( &theMail->from, 41, 34, 0 );

		myRect.left = 3;
		myRect.top = 2;
		myRect.right = 36;
		myRect.bottom = 25;
		
		entryGlobals->myTextBoxMems = DrawTextBox( &myRect, 8, kScrollA );
		StartTextBoxAnimation( entryGlobals->myTextBoxMems, 20 );

		myRect.left = 11;
		myRect.top = 3;
		myRect.right = 36;
		myRect.bottom = 12;
	
		patternStart = LinearizeScreenArea( &myRect, 0 );
		SetupTextGDevice( &myRect, patternStart );
		entryGlobals->textPattern1 = patternStart;
		
		SetCurFont( kXBandHeavy );
		SetFontColors( 0, 15, 14, 0 );	//red

#define kNamePixelLengthLong  200
#define kMailEntryNameXpos  86

		ypos = 4;
		xpos = 4;
		DrawClippedSegaText( xpos, ypos, 110, theMail->from.userName );
		
		SetFontColors( 0, 11, 10, 0 );
		
	//
	// Draw the town and date sent fields
	//
		SetCurFont( kXBandBold10Font );
		theCString = GetSegaString( kTownColon );
		xpos = 4;
		ypos = 26;
		DrawSegaString( &xpos, &ypos, theCString );

		SetCurFont( kXBandLight9Font );
		xpos = 41;
		DrawSegaString( &xpos, &ypos, theMail->from.userTown );

		SetCurFont( kXBandBold10Font );
		theCString = GetSegaString( kDateInMailView );
		xpos = 4;
		ypos = 41;
		DrawSegaString( &xpos, &ypos, theCString );
		
		SetCurFont( kXBandLight9Font );
		SegaNumToDate( theMail->date, theDate, kLongDateFormat );
		xpos = 61;
		DrawSegaString( &xpos, &ypos, theDate );


		SetCurFont( kXBandBold10Font );
		theCString = GetSegaString( kTitleColon );
		xpos = 4;
		ypos = 56;
		DrawSegaString( &xpos, &ypos, theCString );

		SetCurFont( kXBandLight9Font );
		xpos = 35;
		DrawSegaString( &xpos, &ypos, theMail->title );
		
		VDPIdle();
	}

	myRect.left = 5;
	myRect.top = 12;
	myRect.right = 35;
	myRect.bottom = 19;

	patternStart = LinearizeScreenArea( &myRect, 0 );
	SetupTextGDevice( &myRect, patternStart );
	entryGlobals->textPattern2 = patternStart;

	textRect.left = 0;
	textRect.right = 240;
	textRect.top = 1;
	textRect.bottom = 64;
	
	SetFontColors( 0, 11, 10, 0 );
	SetCurFont( kXBandLight9Font );
	SegaBoxText( &textRect, theMail->message, kJustLeft, kCustomLineHeight + (11<<8) );
	return (long) entryGlobals;
}

static long MailEntryCommand( long command, long refCon, ScreenParams *params )
{
Boolean			done;
MailEntryRefCon	*entryGlobals;
short			result;
short			iii;
short			count;
Mail			*theMail;
short			dialogResult;

	entryGlobals = (MailEntryRefCon *)refCon;

	result = 0;
	done = GiveDITLTime( entryGlobals->myControlTable, entryGlobals->theDitlList, command );

	if( done )
	{
		DialogParameterText( entryGlobals->theMail->title, 0, 0, 0 ); 

		result = GetDITLItem( entryGlobals->myControlTable );

		if( result == 1 )
		{
			dialogResult = DoDialog( kAreYouSureDeleteMailEntry );
			if( dialogResult == kDialogYes )
			{
			//delete item hit: find this mail entry and kill it
				count = CountInBoxEntries();
				for( iii = 0; iii < count; iii++ )
				{
					if( entryGlobals->theMail == GetIndexInBoxMail( iii ) )
					{
						RemoveMailFromInBox( iii );
						break;
					}			
				}
			}
			else
				result = kResetScreen-1;

		}
		else
		if( result == 3 )
		{
			//
			//	This selection goes to the mail edit screen. Say no more!
			//
			params->initValue = 0;
		}
		else
		if( result == 2 )
		{
			userIdentification checkMailUser;
			
			if( IsBoxNetRegistered() )
			{
				DoDialog( kAlreadyRegisteredCantUpdateMailDialog );
				result = kResetScreen-1;
			}
			else
			{
			// Check mail
				checkMailUser.box.region = -1;
				checkMailUser.box.box = kDownloadOnlyMailSerialNumber;
				checkMailUser.userID = -1;
				checkMailUser.userName[0] = 0;
				SetChallengeIdentification( &checkMailUser );
			}
		}
		else	
		if( result == 0 )
		{
			// Reply
			//
			// We pass the Mail edit screen the address of the user to reply to
			params->initValue = (long)&entryGlobals->theMail->from;
		}

		if( result != kResetScreen-1 )
		{
		//
		// mark it read
		// find it first...
		//
			count = CountInBoxEntries();
			for( iii = 0; iii < count; iii++ )
			{
				if( entryGlobals->theMail == GetIndexInBoxMail( iii ) )
				{
					MarkMailItemRead( iii );
					break;
				}			
			}
		}
		result = result + 1;
	}

	return result;
}

static void MailEntryKill( long refCon )
{
MailEntryRefCon	*entryGlobals;

	entryGlobals = (MailEntryRefCon *)refCon;

	StopTextBoxAnimation( entryGlobals->myTextBoxMems );
	DisposeTextBoxReference( entryGlobals->myTextBoxMems );
	DisposeGraphicReference( entryGlobals->envelopeRef );
	DisposeGraphicReference( entryGlobals->line2 );
	FreePatternBlock ( entryGlobals->textPattern1 );
	FreePatternBlock ( entryGlobals->textPattern2 );
	DisposeIconReference( entryGlobals->iconRef );
	DisposeDITL( entryGlobals->theDitlList );
	DisposeDITLItemList( entryGlobals->theDitlList );
	DisposeControlTable( entryGlobals->myControlTable );
	DisposeMemory( (Ptr) refCon );

	EnableDefDialogs();
}

Ptr GetMailEntryDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) MailEntryDispatch + 2));
	size = (*(long *) ((Ptr) GetMailEntryDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Mail Entry proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) MailEntryDispatch;
#else
		CheckCodeForJTRefs((void *)MailEntryDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
