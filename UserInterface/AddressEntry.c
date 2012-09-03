/*
	File:		AddressEntry.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<44>	 8/22/94	CMY		Changed button ID's so they draw left to right.
		<43>	 8/17/94	CMY		Changed "Stats:" to "Win - Loss" and changed the format of the
									data.
		<42>	 8/12/94	JOE		Draw player icon the new way
		<41>	  8/9/94	CMY		Moved graphics around.
		<40>	  8/8/94	KON		Remove ClearBackdrop.
		<39>	  8/3/94	CMY		Revectored the DITL buttons
		<38>	  8/3/94	CMY		Changed backgrounds, button layout, jizzle, etc.
		<37>	 7/30/94	KON		Preflight address entry to make sure it exists. The server might
									delete an entry if it's invalid, and bad things happen if we are
									currently looking at that entry.
		<36>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<35>	 7/27/94	KON		Change jizzle color to blue.
		<34>	 7/24/94	CMY		Fixed the little men to draw in the background. You see them
									too, right?
		<33>	 7/22/94	KON		UI poop.
		<32>	 7/22/94	CMY		Added small envelope graphic
		<31>	 7/22/94	KON		UI pooh.
		<30>	 7/22/94	CMY		Fixed the jizzle rect
		<29>	 7/21/94	JOE		Added flags param to SegaBoxText()
		<28>	 7/21/94	KON		Fix up for only four buttons.
		<27>	 7/21/94	KON		Update for new date formats.
		<26>	 7/20/94	KON		Joey turn.
		<25>	 7/19/94	KON		Baby tweaks.
		<24>	 7/19/94	KON		Update for jizzlin borders.
		<23>	 7/19/94	KON		Update to support new screens.
		<22>	 7/12/94	KON		Add confirmation on delete dialog.
		<21>	 7/11/94	KON		Add connect to specific opponent.
		<20>	  7/9/94	CMY		Made "New" work from the "about this address" screen.
		<19>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<18>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<17>	  7/6/94	KON		Remove special case handling of CATAPULT.
		<16>	  7/5/94	KON		Only net register after successful server connect. Check
									Register before connect.
		<15>	  7/4/94	ADS		New interface for screen control
		<14>	  7/1/94	KON		Make connect to CATAPULT the same as FindOpponent.
		<13>	  7/1/94	KON		Remove CES specific stuff, don't show date or W/L for CATAPULT.
		<12>	 6/20/94	KON		Update dialogs and look for CES.
		<11>	 6/17/94	KON		Return a reasonable result when NetRegister wants to do the
									connection.
		<10>	 6/17/94	KON		Added screen reset support.
		 <9>	 6/17/94	KON		Add support for NetRegister Manager.
		 <8>	 6/16/94	KON		Fix the delete key.
		 <7>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <6>	 6/13/94	KON		Make selection parameter to screens a long.
		 <5>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <4>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		 <3>	 6/10/94	KON		Add screen reset and preflight functions.
		 <2>	 6/10/94	KON		Update to use screen dispatcher.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "DitlMgr.h"
#include "AddressCntl.h"
#include "PlayerDB.h"
#include "SegaScrn.h"
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
#include "Screens.h"
#include "MungeCode.h"
#include "AddressEntry.h"
#include "AddressEntryPriv.h"
#include "Backdrops.h"
#include "BoxSer.h"
#include "DialogMgr.h"
#include "NetRegister.h"
#include "Challnge.h"

static long AddressEntryPreflight( long initValue );
static void AddressEntryReset( long refCon );
static long AddressEntrySetup( ScreenParams *params );
static long AddressEntryCommand( long command, long refCon, ScreenParams *params );
static void AddressEntryKill( long refCon );


static long AddressEntryDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = AddressEntryPreflight( params->initValue );
			break;

		case kDoScreenReset:
			AddressEntryReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = AddressEntrySetup( params );
			break;

		case kDoScreenCommand:
			result = AddressEntryCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			AddressEntryKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Address Book Dispatch" );
			break;
			
		}
	return result;
}

static void AddressEntryReset( long refCon )
{
AddressEntryRefCon	*entryGlobals;

	entryGlobals = (AddressEntryRefCon *)refCon;
	ClearDITLDone( entryGlobals->theDitlList );
}

static long AddressEntryPreflight( long initValue )
{
PlayerInfo 		*theInfo;
short			count;
short			iii;

	theInfo = (PlayerInfo *)initValue;

	count = CountAddressBookEntries();
	for( iii = 0; iii < count; iii++ )
	{
		if( theInfo == GetIndexAddressBookEntry( iii ) )
		{
			return 0;	//no problem - the entry we are asked to display exists
		}			
	}
	return -1;	//it doesn't exist, bail
}

static long AddressEntrySetup( ScreenParams *params )
{
PlayerInfo 		*theInfo;
AddressEntryRefCon	*entryGlobals;
char			scratch[30];
char			scratch2[10];
long			shortString;
long			patternStart;
segaCharRect 	myRect;
short			xpos;
Rect			textRect;
short			ypos;
long			littleString;
char			*theCString;


	if (params->selection)
		theInfo = (PlayerInfo *) params->selection;
	else
		theInfo = (PlayerInfo *) params->initValue;

	entryGlobals = (AddressEntryRefCon *)NewMemory( kTemp, sizeof(AddressEntryRefCon) );

	entryGlobals->theDitlList = SetupDITLItemList( kAddressRead );
	entryGlobals->myControlTable = SetupControlTable( kAddressRead );
	entryGlobals->theAddress = theInfo;
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );
	SetCurrentDevice( kScrollA );

	entryGlobals->iconRef = DrawPlayerIcon( &theInfo->userId, 41, 34, 0 );
	entryGlobals->playersRef = DrawDBGraphicAt( 0, kPlayerListIcon, 30, 4, kScrollB );
	entryGlobals->line2 = DrawDBGraphicAt( 0, kGrayHorizontalLine, 5, 19, kScrollB );

	myRect.left = 3;
	myRect.top = 2;
	myRect.right = 36;
	myRect.bottom = 25;
	
	entryGlobals->myTextBoxMems = DrawTextBox( &myRect, 10, kScrollA );
	StartTextBoxAnimation( entryGlobals->myTextBoxMems, 10 );

	myRect.left = 11;
	myRect.top = 3;
	myRect.right = 36;
	myRect.bottom = 12;

	SetCurrentDevice( kScrollA );
	patternStart = LinearizeScreenArea( &myRect, 0 );
	SetupTextGDevice( &myRect, patternStart );
	entryGlobals->textPatterns1 = patternStart;


	SetCurFont( kXBandHeavy );
	SetFontColors( 0, 15, 14, 0 );

#define kNamePixelLengthLong  200
#define kAddressEntryNameXpos  86
	
	ypos = 4;
	xpos = 4;
	DrawClippedSegaText( xpos, ypos, 110, theInfo->userId.userName );
		
	SetFontColors( 0, 9, 8, 0 );

//
// Draw the name, town, last played, and stats
//

	SetCurFont( kXBandBold10Font );
	theCString = GetSegaString( kTownColon );
	xpos = 4;
	ypos = 26;
	DrawSegaString( &xpos, &ypos, theCString );

	SetCurFont( kXBandLight9Font );
	xpos = 40;
	DrawSegaString( &xpos, &ypos, theInfo->userId.userTown );

	SetCurFont( kXBandBold10Font );
	theCString = GetSegaString( kDateLastPlayedColon );
	xpos = 4;
	ypos = 41;
	DrawSegaString( &xpos, &ypos, theCString );
	
	SetCurFont( kXBandLight9Font );
	SegaNumToDate( theInfo->dateLastPlayed, scratch, kLongDateFormat );
	xpos = 67;
	DrawSegaString( &xpos, &ypos, scratch );

	SetCurFont( kXBandBold10Font );
	theCString = GetSegaString( kStatsColon );
	ypos = 56;
	xpos = 4;
	DrawSegaString( &xpos, &ypos, theCString );

	xpos = 60;
	SetCurFont( kXBandLight9Font );
	SegaNumToString( theInfo->wins, scratch );
	littleString = 0x202D2000;	//' - '
	SegaAppendText( scratch, (char *) &littleString, scratch );
	SegaNumToString( theInfo->losses, scratch2 );
	SegaAppendText( scratch, (char *) &scratch2, scratch );

	DrawSegaString( &xpos, &ypos, scratch );

//
// Player about text fits in the standard mail rect
//

	myRect.left = 5;
	myRect.top = 12;
	myRect.right = 35;
	myRect.bottom = 19;

	patternStart = LinearizeScreenArea( &myRect, 0 );
	SetupTextGDevice( &myRect, patternStart );
	entryGlobals->textPatterns2 = patternStart;

	textRect.left = 0;
	textRect.right = 240;
	textRect.top = 1;
	textRect.bottom = 64;
	
	SetFontColors( 0, 9, 8, 0 );
	SetCurFont( kXBandLight9Font );
	SegaBoxText( &textRect, theInfo->info, kJustLeft, 0 );

	NewDITL( entryGlobals->myControlTable, entryGlobals->theDitlList );
	VDPIdle();

	return (long) entryGlobals;
}

static long AddressEntryCommand( long command, long refCon, ScreenParams *params )
{
Boolean	done;
long	result;
AddressEntryRefCon	*entryGlobals;
short	iii;
short	count;
short	dialogResult;
userIdentification	*myUserID;

	result = 0;	//default to keep looping
	entryGlobals = (AddressEntryRefCon *)refCon;
	params->initValue = 0;
	params->selection = (long)entryGlobals->theAddress;
	
	done = GiveDITLTime( entryGlobals->myControlTable, entryGlobals->theDitlList, command );
	
	if( done )
	{
		DialogParameterText( &entryGlobals->theAddress->userId.userName[0], 0, 0, 0 ); 

		result = GetDITLItem( entryGlobals->myControlTable );

		if( result == 1 )
		{

			dialogResult = DoDialog( kAreYouSureDeleteAddressEntry );
			if( dialogResult == kDialogYes )
			{	
			//delete item hit: find this address entry and kill it
				count = CountAddressBookEntries();
				for( iii = 0; iii < count; iii++ )
				{
					if( entryGlobals->theAddress == GetIndexAddressBookEntry( iii ) )
					{
						RemoveAddressBookEntry( iii );
						break;
					}			
				}
			}
			else
				result = kResetScreen-1;

		}
		else
		if( result == 0 )
		{
		// setup the connection for the selected user
		// we will automagically be routed to the Find Opponent screen.
		
			myUserID = &entryGlobals->theAddress->userId;
			SetChallengeIdentification( myUserID );
		}
		else
		if( result == 4 )
		{
			result = 2;
		}
		
		result += 1;	//real world results go from 1 to N;
	}
	return result;
}

static void AddressEntryKill( long refCon )
{
AddressEntryRefCon	*entryGlobals;

	entryGlobals = (AddressEntryRefCon *)refCon;

//
// Cleanup since we aren't coming back here until we setup again!
//
	FreePatternBlock ( entryGlobals->textPatterns1 );
	FreePatternBlock ( entryGlobals->textPatterns2 );

	StopTextBoxAnimation( entryGlobals->myTextBoxMems );
	DisposeTextBoxReference( entryGlobals->myTextBoxMems );

	DisposeIconReference( entryGlobals->iconRef );
	DisposeGraphicReference( entryGlobals->playersRef );
	DisposeGraphicReference( entryGlobals->line2 );
	DisposeDITL( entryGlobals->theDitlList );
	DisposeDITLItemList( entryGlobals->theDitlList );
	DisposeControlTable( entryGlobals->myControlTable );
	DisposeMemory( (Ptr)refCon );
}

Ptr GetAddressEntryDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) AddressEntryDispatch + 2));
	size = (*(long *) ((Ptr) GetAddressEntryDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Address Entry proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) AddressEntryDispatch;
#else
		CheckCodeForJTRefs((void *)AddressEntryDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
