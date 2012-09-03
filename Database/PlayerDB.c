/*
	File:		PlayerDB.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 8/27/94	CMY		Added PreflightNewAddressEntry.
		<25>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<24>	 8/20/94	DJ		made markaddressbookunchanged only marks the bit for the current
									user
		<23>	 8/18/94	JBH		Tightened up time the database is inconsistent to decrease
									chance of mind-loss.
		<22>	 8/15/94	SAH		Added CorrelateAddressBookEntry.
		<21>	 8/12/94	JOE		update for new SegaIcons calls
		<20>	 7/18/94	KON		Dispose icon refs when deleting an address book player.
		<19>	 7/17/94	KON		Add routines to mark address book as unchanged and check whether
									it has changed.
		<18>	  7/7/94	CMY		Made GetUserAddressBookIndex insensitive to differences in case
									and spaces in handles.
		<17>	  7/6/94	DJ		fixed _GetUserAddressBookIndex
		<16>	  7/6/94	KON		Remove special case handling of CATAPULT in the address book.
		<15>	  7/5/94	DJ		move addr book validation into a single message
		<14>	  7/5/94	CMY		SendNewAddressesToServer -- handles validation of new address
									book entries.
		<13>	  7/1/94	KON		Don't allow user to delete DBID 0 in the address book which is
									always the CATAPULT entry.
		<12>	 6/30/94	SAH		Removed database purge values.
		<11>	 6/28/94	SAH		User ID's no longer live in the database.
		<10>	 6/20/94	KON		Include AddressBook.h to get kMaxAddressBookEntries.
		 <9>	 6/18/94	KON		Moved the add to address book stuff to NewAddressMgr.c.
		 <8>	 6/17/94	KON		More address book dialog stuff.
		 <7>	 6/15/94	KON		Added parameter text dialogs to show names of users that are
									being added to the address book.
		 <6>	 6/15/94	KON		Update existing address book entries. None of this shit is
									tested.
		 <5>	 6/15/94	KON		Add Ask to address book stuff. The name doesn't get added
									properly yet...
		 <4>	 6/14/94	KON		Add AddAddressBookEntry.
		 <3>	 6/13/94	SAH		Managerized.
		 <2>	  6/4/94	KON		Use kDBIDErrorValue instead of -1.
		 <1>	  6/1/94	KON		first checked in

	To Do:
*/

#include "PlayerDB.h"
#include "SegaOS.h"
#include "DataBase.h"
#include "Errors.h"
#include "DBTypes.h"
#include "Heaps.h"
#include "Personification.h"
#include "SegaText.h"
#include "Opponent.h"
#include "Time.h"
#include "DialogMgr.h"
#include "BoxSer.h"
#include "NewAddressMgr.h"
#include "AddressBook.h"
#include "Messages.h"
#include "PlayerDBPriv.h"
#include "SegaBM.h"
#include "Keyboard.h"

DBType	GetAddressBookTypeForCurrentUser( void ) = 
	CallDispatchedFunction( kGetAddressBookTypeForCurrentUser );

DBID	GetAddressBookIDFromIndex( short index ) = 
	CallDispatchedFunction( kGetAddressBookIDFromIndex );

DBErr DeleteAddressBookEntry( short index ) =
	CallDispatchedFunction( kDeleteAddressBookEntry );

// private dispatched calls
DBType	_GetAddressBookTypeForCurrentUser( void );
DBID	_GetAddressBookIDFromIndex( short index );
void 	_AddAddressBookEntry( PlayerInfo * theEntry ); 
DBErr 	_DeleteAddressBookEntry( short index );

// public dispatched calls
short	_CountAddressBookEntries( void );
DBErr	_RemoveAddressBookEntry( short index );
PlayerInfo *_GetIndexAddressBookEntry( short index );
short _GetUserAddressBookIndex( userIdentification	*theUserID );
void _SendNewAddressesToServer( void );
void _MarkAddressBookUnchanged( void );
Boolean _AddressBookHasChanged( void );
void _CorrelateAddressBookEntry ( short index, short serverUniqueID );
Boolean _PreflightNewAddressEntry ( void );


#ifndef SIMULATOR
AddressBookGlobals addressGlobals;
#endif	

long
_PlayerDBControl ( short command, long data )
{
long offset;
short error;
AddressBookGlobals *globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:

			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET( addressGlobals, offset );
			
			error = AllocateGlobalSpace ( kPlayerDBManager, offset,
				sizeof(AddressBookGlobals), (Ptr *) &globals );
				
			ASSERT_MESG ( !error, "Can't create address globals" );
			
			globals->addressBookChanged = 0;			//mark address book unread

			/* install our selectors */
			SetDispatchedFunction ( kGetAddressBookTypeForCurrentUser,	kPlayerDBManager,	_GetAddressBookTypeForCurrentUser );
			SetDispatchedFunction ( kGetAddressBookIDFromIndex,	kPlayerDBManager,	_GetAddressBookIDFromIndex );
			SetDispatchedFunction ( kCountAddressBookEntries,	kPlayerDBManager,	_CountAddressBookEntries );
			SetDispatchedFunction ( kRemoveAddressBookEntry,	kPlayerDBManager,	_RemoveAddressBookEntry );
			SetDispatchedFunction ( kGetIndexAddressBookEntry,	kPlayerDBManager,	_GetIndexAddressBookEntry );
			SetDispatchedFunction ( kAddAddressBookEntry,		kPlayerDBManager,	_AddAddressBookEntry );
			SetDispatchedFunction ( kGetUserAddressBookIndex, 	kPlayerDBManager,	_GetUserAddressBookIndex );
			SetDispatchedFunction ( kDeleteAddressBookEntry, 	kPlayerDBManager,	_DeleteAddressBookEntry );
			SetDispatchedFunction ( kSendNewAddressesToServer, 	kPlayerDBManager,	_SendNewAddressesToServer );
			SetDispatchedFunction ( kMarkAddressBookUnchanged, 	kPlayerDBManager,	_MarkAddressBookUnchanged );
			SetDispatchedFunction ( kAddressBookHasChanged, 	kPlayerDBManager,	_AddressBookHasChanged );
			SetDispatchedFunction ( kCorrelateAddressBookEntry, kPlayerDBManager,	_CorrelateAddressBookEntry );
			SetDispatchedFunction ( kPreflightNewAddressEntry,	kPlayerDBManager,	_PreflightNewAddressEntry );
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


static DBType _GetAddressBookTypeForCurrentUser( void )
{
short	userID;
DBType	userType;

	userID = GetCurrentLocalUser()->userID;
	
	switch( userID )
	{
		case 0:
			userType = kAddressBookType0;
			break;
			
		case 1:
			userType = kAddressBookType1;
			break;
	
		case 2:
			userType = kAddressBookType2;
			break;
	
		case 3:
			userType = kAddressBookType3;
			break;	
			
		default:
			ERROR_MESG( "Unknown user ID" );
			userType = kAddressBookType0;
			break;
	}
	
	return userType;
}

static DBID _GetAddressBookIDFromIndex( short index )
{
DBID	curID;
DBType	thisUserDBType;
short	iii;
short	theCount;

	thisUserDBType = GetAddressBookTypeForCurrentUser();

	theCount = CountAddressBookEntries();
	
	if( theCount == 0 )
	{
		ERROR_MESG( "Address book empty - can't get index entry" );
		return kDBIDErrorValue;
	}
	
	if( theCount < index )
	{
		ERROR_MESG( "Trying to get a higher address book index than exists!" );
		return kDBIDErrorValue;
	}
//
// walk to this index 
//
	curID = DBGetFirstItemID( thisUserDBType );

	for( iii = 0; iii < index; iii++ )
	{
		curID = DBGetNextItemID( thisUserDBType, curID );
	}
	return curID;
}

short _CountAddressBookEntries( void )
{
DBType		thisUserDBType;

	thisUserDBType = GetAddressBookTypeForCurrentUser();
	return DBCountItems( thisUserDBType );
}

//
// The DelteAddressBookEntry function is a low-level routine to wipe out
// a specific address book entry.
//
// RemoveAddressBookEntry is called by the higher level UI and automagically
// adds the "on-deck" current opponent if there is one.
//
DBErr _DeleteAddressBookEntry( short index )
{
DBType			thisUserDBType;
DBID			thisID;
unsigned char	thisUserID;
short			addressChangedMask;
PlayerInfo		*myUserID;

	thisUserDBType = GetAddressBookTypeForCurrentUser();
	
	if( CountAddressBookEntries() == 0 )
	{
		ERROR_MESG( "Nothing in the Address Book to delete!" );
	}

	thisID = GetAddressBookIDFromIndex( index );
	myUserID = GetIndexAddressBookEntry( index );
	NukePlayerRAMIcon( &myUserID->userId );
	DBDeleteItem( thisUserDBType, thisID );

	thisUserID = GetCurrentLocalUser()->userID;
	addressChangedMask = 1 << thisUserID;
	
	REFGLOBAL( addressGlobals, addressBookChanged ) |= addressChangedMask;
}

DBErr _RemoveAddressBookEntry( short index )
{
	DeleteAddressBookEntry( index );
	AddOnDeckAddressBookEntry();	//adds on deck guy if there is one
}

PlayerInfo *_GetIndexAddressBookEntry( short index )
{
DBID	curID;
DBType	thisUserDBType;

	thisUserDBType = GetAddressBookTypeForCurrentUser();
	curID = GetAddressBookIDFromIndex( index );	
	
	return ( (PlayerInfo *)DBGetItem( thisUserDBType, curID ) );
}

short _GetUserAddressBookIndex( userIdentification	*theUserID )
{
PlayerInfo *thisInfo;
short	theCount;
short	iii;

	theCount = CountAddressBookEntries();
	for( iii = 0; iii < theCount; iii++ )
	{
	//
	// Look for this box and user ID in the address book
	//
		thisInfo = GetIndexAddressBookEntry( iii );
		
		/* If the address book entry has not been validated, match on the name */
		if (thisInfo->userId.box.box == -1 || theUserID->box.box == -1)
		{
			UserName user1, user2;
			
			/* If the names match, return it */
			if (EqualCStrings(thisInfo->userId.userName, theUserID->userName))
				return iii;
				
			/* Otherwise, remove spaces and convert to lower-case and try again */
			CopyCString(thisInfo->userId.userName, user1);
			CopyCString(theUserID->userName, user2);
			MinimizeUserHandle(user1);
			MinimizeUserHandle(user2);
			if (EqualCStrings(user1, user2))
				return iii;
		}
					
		if( (thisInfo->userId.box.region == theUserID->box.region) 
		&& (thisInfo->userId.box.box == theUserID->box.box)
		&& (thisInfo->userId.userID == theUserID->userID) )
			return iii;
	}
	
	return kDBIDErrorValue;
}

Boolean _PreflightNewAddressEntry ( void )
{
	long maxSizedAddress;
	short err;
	DBID addressBookType;

	/* The maximum address size is the size of an address and as much info text as a user can
	 * type in. Even though we won't get the user's info here, we prevent them from adding it
	 * so that if the player has a huge info string, we don't blow it away after a server
	 * connect.
	 */
	maxSizedAddress = sizeof(PlayerInfo) + GetKeyLayoutFieldSize(kChooseInfoTextKeyboardEntry, 0);
 	addressBookType = GetAddressBookTypeForCurrentUser();
 
 	/* Is there enough room for a max sized address book entry and still leave some slop? */
 	err = DBPreflight( addressBookType, DBGetUniqueID(addressBookType), maxSizedAddress + kRAMDBSlop,
		kPreflightLight );
	
	return (err != 0);
}

void _AddAddressBookEntry( PlayerInfo * theEntry )
{
DBType		thisUserDBType;
DBID		newID;
unsigned char	thisUserID;
short			addressChangedMask;

	thisUserDBType = GetAddressBookTypeForCurrentUser();
	
	if( CountAddressBookEntries() == kMaxAddressBookEntries )
	{
		ERROR_MESG( "Address Book is full! Quit adding shit!" );
		return;
	}
	
	newID = DBGetUniqueID( thisUserDBType );
	if( DBAddItem( thisUserDBType, newID, theEntry ) )
	{
		ERROR_MESG( "Error adding Address Book entry in PlayerDB.c" );
	}

	thisUserID = GetCurrentLocalUser()->userID;
	addressChangedMask = 1 << thisUserID;
	
	REFGLOBAL( addressGlobals, addressBookChanged ) |= addressChangedMask;
}

void _SendNewAddressesToServer( void )
{
	SendMessage( msSendAddressesToVerify, NULL );
}

Boolean _AddressBookHasChanged( void )
{
unsigned char	thisUserID;
short			addressChangedMask;

	thisUserID = GetCurrentLocalUser()->userID;
	addressChangedMask = 1 << thisUserID;
	
	if( REFGLOBAL( addressGlobals, addressBookChanged ) & addressChangedMask )
		return true;
	else
		return false;
}

void _MarkAddressBookUnchanged( void )
{	
unsigned char	thisUserID;
short			addressChangedMask;

	thisUserID = GetCurrentLocalUser()->userID;
	addressChangedMask = 1 << thisUserID;

	REFGLOBAL( addressGlobals, addressBookChanged ) &= ~addressChangedMask;
}


void _CorrelateAddressBookEntry ( short index, short serverUniqueID )
{
PlayerInfo * entry;
DBType		thisUserDBType;

	entry = GetIndexAddressBookEntry ( index );
	thisUserDBType = GetAddressBookTypeForCurrentUser();
	if ( entry )
		{
		entry->serverUniqueID = serverUniqueID;
		DBTypeChanged ( thisUserDBType );
		}
}