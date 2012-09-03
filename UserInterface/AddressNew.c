/*
	File:		AddressNew.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<29>	 8/27/94	CMY		Preflight the RAM DB before letting the user enter a handle.
		<28>	 8/25/94	CMY		Added default info text for new players in player list.
		<27>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<26>	 8/13/94	CMY		Fix an odd-address error on 68K that was resetting the box.
		<25>	 8/12/94	JOE		Custom icon changes
		<24>	  8/9/94	CMY		Removed the "Select Done when finished" mystery string.
		<23>	  8/4/94	CMY		Changed text layout. Use ScreenLayout rather than constants.
		<22>	  8/4/94	ADS		Correctly preset new serverUniqueID field
		<21>	  8/3/94	CMY		Added a title.
		<20>	  8/3/94	CMY		Changed backgrounds, button layout, jizzle, etc.
		<19>	 7/31/94	KON		Add descriptive text and input field.
		<18>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<17>	 7/26/94	CMY		Dispose the jizzlers.
		<16>	 7/24/94	CMY		Fixed the New Address to use jizzlers.
		<15>	 7/21/94	CMY		Use the fancy new background.
		<14>	 7/19/94	KON		Set hometown field for new users to "Unknown."
		<13>	 7/19/94	KON		Remove the old icon ref stuff and set the userIdentification
									icon refs.
		<12>	 7/13/94	KON		If box is netRegistered, warn user that he may be interrupted.
		<11>	  7/9/94	CMY		Made "
		<10>	  7/8/94	CMY		Start button scrolls through address book entries in New Mail
									screen.
		 <9>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		 <8>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <7>	  7/7/94	CMY		Added checks for name collisions and too-short handles.
		 <6>	  7/7/94	KON		Make date last played map to kNever.
		 <5>	  7/5/94	CMY		Error checking: empty addresses not allowed.
		 <4>	  7/5/94	JOE		init iconId to kDBReservedItemType (instead of 0)
		 <3>	  7/4/94	SAH		Added #ifdef	ROMDB conditional.
		 <2>	  7/3/94	CMY		Preflight to see if there's room for address.
		 <1>	  7/2/94	CMY		first checked in

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
#include "AddressNewPriv.h"
#include "PlayerDB.h"
#include "Utils.h"
#include "DialogMgr.h"
#include "AddressBook.h"
#include "NewAddressMgr.h"
#include "NetRegister.h"


#define kLayoutJizzleRect 0
#define kLayoutBoxRect					1

#define kAddressBookTitleString			0
#define kAddressBookDescriptionString	1
#define kAddressBookDoneString			2

static long NewAddressPreflight( long initValue );
static void NewAddressReset( long refCon );
static long NewAddressSetup( ScreenParams *params );
static long NewAddressCommand( long command, long refCon, ScreenParams *params );
static void NewAddressKill( long refCon );

static long NewAddressDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = NewAddressPreflight( params->initValue );
			break;

		case kDoScreenReset:
			NewAddressReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = NewAddressSetup( params );
			break;

		case kDoScreenCommand:
			result = NewAddressCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			NewAddressKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in NewAddress Dispatch" );
			break;
			
		}
	return result;
}

static void NewAddressReset( long refCon )
{
NewAddressGlobals	*new;

	new = (NewAddressGlobals *)refCon;

}

static long NewAddressPreflight( long initValue )
{
	
	if ( CountAddressBookEntries() == kMaxAddressBookEntries )
	{
		DoDialog(kAddressBookFullDialog);
		return -1;
	}
	
	/* See if we have room in the RAM DB for another player */
	if ( PreflightNewAddressEntry() )
	{
		DoDialog(kNotEnoughRoomForNewPlayerList);
		return -1;
	}
	
	if( IsBoxNetRegistered() )
	{
		DoDialog( kYouMayBeInterruptedAddress );
	}
	return 0;
}

static long NewAddressSetup( ScreenParams *params )
{
NewAddressGlobals	*new;
KeyboardEntryLayout *keyLayout;
segaCharRect		myRect;
segaCharRect		textGDRect;
char				*description;
Rect				textRect;

	new = (NewAddressGlobals *)NewMemory( kTemp, sizeof(NewAddressGlobals) );
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );

	GetScreenLayoutCharRect( kAddressBookNewScreen, kLayoutJizzleRect, &myRect );
	new->jizzle = DrawTextBox( &myRect, 8, kScrollA );
	StartTextBoxAnimation(new->jizzle, 7);
	
	new->playersRef = DrawDBGraphicAt( 0, kPlayerListIcon, 30, 4, kScrollB );
	SetCurrentDevice( kScrollA );

	SetCurFont( kXBandHeavy );
	SetFontColors( 0, 15, 14, 0 );
	new->titlePatterns = DrawScreenLayoutString( kAddressBookNewScreen, kAddressBookTitleString );

	SetCurFont( kXBandLight9Font );
	SetFontColors( 0, 9, 8, 0 );
	new->descriptionPatterns = BoxScreenLayoutString( kAddressBookNewScreen,
		kAddressBookDescriptionString,kLayoutBoxRect, kJustLeft );

	new->handleField = DrawDBGraphicAt( 0, kHandleEntryField, 13, 15, kScrollB );
	SetCurrentDevice( kScrollA );

	keyLayout = SetupKeyboardEntryLayout( kAddressEditKeyboardEntry );
	new->keyRef = InitKeyboardEntry( keyLayout, 0 );
	new->keyLayout = keyLayout;
	
	return (long) new;
}

static long NewAddressCommand( long command, long refCon, ScreenParams *params )
{
Boolean			done;
NewAddressGlobals	*new;
short			result;
short			iii;
short			count;
short			collisionIndex;

	new = (NewAddressGlobals *)refCon;

	result = SendCommandToKeyboard(new->keyRef, command);
	
	if (result)
	{
		// User decided to Add the address?
		if (result == kKeyboardDone)
		{	PlayerInfo *newEntry;
			short stringLength;
			userIdentification newUser;
			
			CopyCString(new->keyLayout->fields[kNewAddressAddressField].buffer, newUser.userName);
			MinimizeUserHandle(newUser.userName);
			stringLength = GetStringLength(newUser.userName);
			newUser.box.box = -1;
			
			if (stringLength == 0)
			{
				DoDialog(kAddressEmptyDialog);
				RefreshKeyboard(new->keyRef);
				result = 0;
			}
			else if (stringLength < 2)
			{
				DoDialog(kHandleTooShortDialog);
				RefreshKeyboard(new->keyRef);
				result = 0;
			}
			else if ( (collisionIndex = GetUserAddressBookIndex(&newUser)) != kDBIDErrorValue)
			{
				PlayerInfo *info = GetIndexAddressBookEntry(collisionIndex);
				
				DialogParameterText(new->keyLayout->fields[kNewAddressAddressField].buffer, info->userId.userName, nil, nil);
				if (EqualCStrings(new->keyLayout->fields[kNewAddressAddressField].buffer, info->userId.userName))
				{
					DoDialog(kDuplicateAddressBookEntryDialog);
				}
				else
				{
					DoDialog(kEquivalentAddressBookEntryDialog);
				}
				result = 1;
			}
			else
			{
				long unknown[2];
				char *defaultInfoText = GetSegaString( kDefaultInfoText );
				short infoLength = GetStringLength( defaultInfoText );
				
				newEntry = (PlayerInfo *)NewMemory( kTemp, sizeof(PlayerInfo) + infoLength );
				newEntry->userId.box.region = -1;
				newEntry->userId.box.box = -1;
				newEntry->userId.ROMIconID = 0;
				newEntry->userId.colorTableID = kDBIDErrorValue;
				unknown[0] = 0x556E6B6EL;
				unknown[1] = 0x6F776E00L;
				CopyCString( (char *)unknown, newEntry->userId.userTown );
				newEntry->userId.userID = -1;
				newEntry->serverUniqueID = kUncorrelatedEntry;
				CopyCString(new->keyLayout->fields[kNewAddressAddressField].buffer, newEntry->userId.userName);
				
				newEntry->wins = 0;
				newEntry->losses = 0;
				newEntry->dateLastPlayed = 1;	//Never!
				CopyCString(defaultInfoText, newEntry->info);
	
				AddAddressBookEntry( newEntry );
				
				DisposeMemory( (Ptr) newEntry );
				result = 1;	
			}
		}
		else
		{
			// An error ocurred.
			result = 1;
		}
	}
	return result;
}

static void NewAddressKill( long refCon )
{
NewAddressGlobals	*new = (NewAddressGlobals *)refCon;

	FinishKeyboardEntry( new->keyRef);	
	DisposeKeyboardEntryLayout(new->keyLayout);
	DisposeTextBoxReference( new->jizzle );
	FreePatternBlock( new->titlePatterns );
	FreePatternBlock( new->descriptionPatterns );
	DisposeGraphicReference( new->playersRef );
	DisposeGraphicReference( new->handleField );
	DisposeMemory((Ptr)new);
}

Ptr GetNewAddressDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) NewAddressDispatch + 2));
	size = (*(long *) ((Ptr) GetNewAddressDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for New Address proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) NewAddressDispatch;
#else
		CheckCodeForJTRefs((void *)NewAddressDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
