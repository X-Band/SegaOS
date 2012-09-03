/*
	File:		NewAddressMgr.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<19>	 8/27/94	CMY		Preflight the RAM DB before adding the opponent to the player's
									address book.
		<18>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<17>	 8/19/94	JOE		Include SegaBM.h
		<16>	 8/19/94	JOE		Nuke opponent's RAM icon after game if (1) he's not in the
									address book and (2) he wasn't just added to the address book
		<15>	 8/19/94	JOE		nuke some obsolete "opponent exists" stuff
		<14>	 8/19/94	JOE		sigh.  also get the opponent's info from GetCurOpponentInfo().
		<13>	 8/18/94	JOE		AddPlayerToAddressBook() now adds the opponent's stuff, not the
									current local user's
		<12>	 8/14/94	KON		Add parameter text in update address book to put opponent's name
									in dialogs.
		<11>	  8/4/94	ADS		Correctly set the new serverUniqueID field
		<10>	 7/18/94	KON		Remove icon field since its in the userIdentification now.
		 <9>	 7/15/94	SAH		New interface for getting game results.
		 <8>	  7/7/94	CMY		MinimizeUserHandle - removes spaces and case-sensitivity.
		 <7>	  7/5/94	JOE		updated for new personification model
		 <6>	  7/2/94	DJ		fuck mpw
		 <5>	  7/2/94	DJ		added gameresultflag to setgamescore
		 <4>	 6/28/94	SAH		Updated to new box serial id and local user stuff.
		 <3>	 6/20/94	KON		Add AddressBook.h to get kMaxAddressBookEntries define.
		 <2>	 6/18/94	KON		Check for nil userIDs.
		 <1>	 6/18/94	KON		first checked in

	To Do:
*/


#include "UsrConfg.h"
#include "DBTypes.h"
#include "DataBase.h"
#include "PlayerDB.h"

#include "NewAddressMgr.h"
#include "NewAddressMgrPriv.h"
#include "DialogMgr.h"


#include "BoxSer.h"
#include "SegaText.h"
#include "SegaText.h"
#include "Opponent.h"
#include "Errors.h"
#include "UsrConfg.h"
#include "Personification.h"
#include "time.h"
#include "heaps.h"
#include "AddressBook.h"
#include "SegaBM.h"

#ifndef SIMULATOR
NewAddressMgrGlobals newAddressGlobals;
#endif


//
// Dispatched stuff
//
static Boolean _AddPlayerToAddressBook( short wins, short losses, userIdentification *theUserID );
static void _UpdateAddressBookStuff( short action );
static void _AddOnDeckAddressBookEntry( void );
void _MinimizeUserHandle(char *handle);

long
_NewAddressControl( short command, long data )
{
long offset;
short error;
NewAddressMgrGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
		
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(newAddressGlobals,offset);
			
			error = AllocateGlobalSpace ( kNewAddressManager, offset, sizeof(NewAddressMgrGlobals),
				(Ptr *) &globals );
			if ( error != noErr )
				{
				return error;
				}
						
			/* install our selectors */
			SetDispatchedFunction ( kUpdateAddressBookStuff, kNewAddressManager, _UpdateAddressBookStuff );
			SetDispatchedFunction ( kAddPlayerToAddressBook, kNewAddressManager, _AddPlayerToAddressBook );
			SetDispatchedFunction ( kAddOnDeckAddressBookEntry, kNewAddressManager, _AddOnDeckAddressBookEntry );
			SetDispatchedFunction ( kMinimizeUserHandle, kNewAddressManager, _MinimizeUserHandle );

			globals->playerWaitingForDelete = 0;
			break;
		
		case kSoftInialize:
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


static void _UpdateAddressBookStuff( short action )
{
Boolean	response;
short dialogResult;
short thisOpponentIndex;
long wins;
long losses;
userIdentification	*theUserID;
PlayerInfo *theEntry;

//
// Get the opponent
//
	theUserID = GetCurOpponentIdentification();
	
	wins = 0;
	losses = 0;
	
//
// A tie counts as both a win and a loss!
//		
	GetGameWinsLosses ( &wins, &losses );
	
	thisOpponentIndex = GetUserAddressBookIndex( theUserID );

	if( thisOpponentIndex != kDBIDErrorValue )
	{
	//
	// This opponent is already in the address book, update the entry
	//
		theEntry = GetIndexAddressBookEntry( thisOpponentIndex );
		wins += theEntry->wins;
		losses += theEntry->losses;

		//
		// if the current opponent has the same box and user ID but a new name, inform user
		//
	
		if( !EqualCStrings( theUserID->userName, theEntry->userId.userName ) )
		{
			DialogParameterText( theEntry->userId.userName, theUserID->userName, 0, 0 ); 
			DoDialog( kUserHasNewNameDialog );
		}
			
		RemoveAddressBookEntry( thisOpponentIndex );
		AddPlayerToAddressBook( wins, losses, theUserID );
		REFGLOBAL(newAddressGlobals, playerWaitingForDelete) = 0;		
	}
	else			// ELSE put up dialog "Would you like to add this opponent to your address book"
		if( action == kUpdateOrAdd )
		{
			DialogParameterText( theUserID->userName, 0, 0, 0 ); 
			dialogResult = DoDialog( kAddToAddressBookDialog );
			if( dialogResult == kDialogYes )
			{
				if( PreflightNewAddressEntry() )
				{
					DoDialog( kNotEnoughRoomForNewPlayerList );
				}
				else if( AddPlayerToAddressBook( wins, losses, theUserID ) == true )
				{
				// 
				//put up address book is full, will add when someone is deleted message
				//
					DialogParameterText( theUserID->userName, 0, 0, 0 ); 
					DoDialog( kWillBeAddedWhenDeleteDialog );
				}
			}
			else
			{
				REFGLOBAL(newAddressGlobals, playerWaitingForDelete) = 0;		
				NukePlayerRAMIcon( theUserID );		/* not in addr book AND not added to addr book, nuke his icon */
			}
		}
}

static Boolean _AddPlayerToAddressBook( short wins, short losses, userIdentification *theUserID )
{
short	stringLength;
Ptr		personificationAboutText;
PlayerInfo *theEntry;
Boolean	result = false;
	
	personificationAboutText = GetCurOpponentInfo();

	if( personificationAboutText == 0 )
	{
		ERROR_MESG( "There's no about text in AddCurrentOpponentToAddressBook!" );
		return false;
	}

	REFGLOBAL(newAddressGlobals, wins) = wins;
	REFGLOBAL(newAddressGlobals, losses) = losses;
	REFGLOBAL(newAddressGlobals, userID) = *theUserID;
	REFGLOBAL(newAddressGlobals, date) = GetJesusDate();
		

	stringLength = GetStringLength( personificationAboutText );

	if( CountAddressBookEntries() == kMaxAddressBookEntries )
	{
		REFGLOBAL(newAddressGlobals, playerWaitingForDelete) = 1;			
		result = true;
	}
	else
	{
		theEntry = (PlayerInfo *)NewMemory( kTemp, sizeof(PlayerInfo) + sizeof(char)*stringLength );
		theEntry->userId = REFGLOBAL(newAddressGlobals, userID);
		theEntry->serverUniqueID = kUncorrelatedEntry;	// always reset new addr book entries
		theEntry->wins = REFGLOBAL(newAddressGlobals, wins);
		theEntry->losses = REFGLOBAL(newAddressGlobals, losses);
		theEntry->dateLastPlayed = REFGLOBAL(newAddressGlobals, date);
		CopyCString( personificationAboutText, &theEntry->info[0] );
		
	
		AddAddressBookEntry( theEntry );

		DisposeMemory( (Ptr) theEntry );
		REFGLOBAL(newAddressGlobals, playerWaitingForDelete) = 0;	
		result = false;	
	}
	return result;
}

void _AddOnDeckAddressBookEntry( void )
{
	if( REFGLOBAL(newAddressGlobals, playerWaitingForDelete) == 1 )
	{
		if( (CountAddressBookEntries() <= kMaxAddressBookEntries - 1) )
		{
			AddPlayerToAddressBook( REFGLOBAL(newAddressGlobals, wins), 
									REFGLOBAL(newAddressGlobals, losses),
									&REFGLOBAL(newAddressGlobals, userID) );
									
			REFGLOBAL(newAddressGlobals, playerWaitingForDelete) = 0;		
		}
	}
}


void _MinimizeUserHandle(char *handle)
{
	char *src = handle;
	char *dst = handle;
	
	while (*src)
	{
		if (*src == ' ')
			++src;
		else if (*src >= 'A' && *src <= 'Z')
		{
			*src += 'a'-'A';
			*dst++ = *src++;
		}
		else
			*dst++ = *src++;
	}
	*dst++ = 0;
}

