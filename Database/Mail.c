/*
	File:		MailCntl.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<17>	 8/19/94	JBH		GetOutBoxTypeForCurrentUser made public
		<16>	 7/24/94	ADS		got rid of temp leftovers
		<15>	 7/19/94	ADS		Vector Mania
		<14>	 7/15/94	CMY		Added DeleteAllOutBoxMail.
		<13>	 7/14/94	CMY		Add size parameter to GetIndexOutBoxMail
		<12>	 7/14/94	CMY		Add GetIndexOutBoxMail
		<11>	 7/13/94	CMY		Added CountOutBoxEntries for putting up dialogs.
		<10>	  7/3/94	KON		Add MarkMailItemRead function.
		 <9>	 6/30/94	SAH		Removed database purge values. Updated to new SendQ. Killed all
									th eold out box db related calls.
		 <8>	 6/28/94	SAH		Updated to new user id stuff.
		 <7>	 6/20/94	KON		GetInBoxMailGraphic now takes a screen parameter.
		 <6>	 6/14/94	SAH		Managerized.
		 <5>	 6/13/94	KON		Add call to get mail icon depending on amount of mail.
		 <4>	  6/4/94	KON		Check for mail overflow.
		 <3>	  6/4/94	KON		Use kDBIDErrorValue instead of -1.
		 <2>	 5/29/94	DJ		changed resultfifo dbtype to mailfifo...
		 <1>	 5/27/94	KON		first checked in
	To Do:
*/

#include "SegaOS.h"
#include "UsrConfg.h"
#include "DataBase.h"
#include "Mail.h"
#include "Errors.h"
#include "DBTypes.h"
#include "Heaps.h"
#include "Reliability.h"
#include "SendQ.h"
#include "GraphicsDB.h"
#include "Screens.h"


static short _CountInBoxEntries( void );
static short _CountOutBoxEntries( void );

static void _AddMailToOutBox( Mail *theMail );

static void _AddMailToInBox( Mail *theMail );
static void _RemoveMailFromInBox( short index );
static Mail *_GetIndexInBoxMail( short index );
static Mail *_GetIndexOutBoxMail( short index, long *size );

static DBID _GetInBoxGraphicID( short whichScreen );
static void _MarkMailItemRead( short index );
static void _DeleteAllOutBoxMail( void );

//
// local, vectorized
//
static DBType	_GetInBoxTypeForCurrentUser( void );
static DBType	_GetOutBoxTypeForCurrentUser( void );
static DBID		_GetOutBoxIDFromIndex( short index );
static DBID 	_GetInBoxIDFromIndex( short index );
static DBID 	_GetBoxIDFromIndex( Boolean inOrOut, short index );

DBType GetInBoxTypeForCurrentUser( void ) =
		CallDispatchedFunction( kGetInBoxTypeForCurrentUser );

DBID GetOutBoxIDFromIndex( short index ) =
		CallDispatchedFunction( kGetOutBoxIDFromIndex );

DBID GetInBoxIDFromIndex( short index ) =
		CallDispatchedFunction( kGetInBoxIDFromIndex );

DBID GetBoxIDFromIndex( Boolean inOrOut, short index ) =
		CallDispatchedFunction( kGetBoxIDFromIndex );




long
_MailControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* install our selectors */
			SetDispatchedFunction ( kCountInBoxEntries,		kMailManager,	_CountInBoxEntries );
			SetDispatchedFunction ( kCountOutBoxEntries,	kMailManager,	_CountOutBoxEntries );
			SetDispatchedFunction ( kAddMailToOutBox,		kMailManager,	_AddMailToOutBox );
			SetDispatchedFunction ( kAddMailToInBox,		kMailManager,	_AddMailToInBox );
			SetDispatchedFunction ( kRemoveMailFromInBox,	kMailManager,	_RemoveMailFromInBox );
			SetDispatchedFunction ( kGetIndexInBoxMail,		kMailManager,	_GetIndexInBoxMail );
			SetDispatchedFunction ( kGetIndexOutBoxMail,	kMailManager,	_GetIndexOutBoxMail );
			SetDispatchedFunction ( kGetInBoxGraphicID,		kMailManager,	_GetInBoxGraphicID );
			SetDispatchedFunction ( kMarkMailItemRead,		kMailManager,	_MarkMailItemRead );
			SetDispatchedFunction ( kDeleteAllOutBoxMail,	kMailManager,	_DeleteAllOutBoxMail );

			SetDispatchedFunction ( kGetInBoxTypeForCurrentUser,	kMailManager,	_GetInBoxTypeForCurrentUser );
			SetDispatchedFunction ( kGetOutBoxTypeForCurrentUser,	kMailManager,	_GetOutBoxTypeForCurrentUser );
			SetDispatchedFunction ( kGetOutBoxIDFromIndex,			kMailManager,	_GetOutBoxIDFromIndex );
			SetDispatchedFunction ( kGetInBoxIDFromIndex,			kMailManager,	_GetInBoxIDFromIndex );
			SetDispatchedFunction ( kGetBoxIDFromIndex,				kMailManager,	_GetBoxIDFromIndex );
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

static DBType _GetInBoxTypeForCurrentUser( void )
{
DBType	userType;

	userType = GetCurrentLocalUser()->userID + kMailInBoxType0;
	
	return userType;
}

static DBType _GetOutBoxTypeForCurrentUser( void )
{
DBType	userType;

	userType = GetCurrentLocalUser()->userID + kMailOutBoxType0;
	
	return userType;
}

static void _DeleteAllOutBoxMail( void )
{
	short userIndex;
	DBType outbox;
	DBID toKill;
	
	
	for (userIndex=0; userIndex<4; ++userIndex)
	{
		outbox = kMailOutBoxType0 + userIndex;
		toKill = DBGetFirstItemID( outbox );
		
		while (toKill != kDBIDErrorValue)
		{
			DBDeleteItem(outbox, toKill);
			toKill = DBGetFirstItemID( outbox );
		}
	}
}

static DBID _GetBoxIDFromIndex( Boolean inOrOut, short index )
{
DBID	curID;
DBType	thisUserDBType;
short	iii;
short	theCount;

	if (inOrOut)
	{
		thisUserDBType = GetInBoxTypeForCurrentUser();
		theCount = CountInBoxEntries();
	}
	else
	{
		thisUserDBType = GetOutBoxTypeForCurrentUser();
		theCount = CountOutBoxEntries();
	}
	
	if( theCount == 0 )
	{
		ERROR_MESG( "In box is empty - can't get index entry" );
		return kDBIDErrorValue;
	}
	
	if( theCount < index )
	{
		ERROR_MESG( "Trying to get a higher in box index than exists!" );
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


static DBID _GetInBoxIDFromIndex( short index )
{
	return GetBoxIDFromIndex( true, index );
}


static DBID _GetOutBoxIDFromIndex( short index )
{
	return GetBoxIDFromIndex( false, index );
}


short _CountInBoxEntries( void )
{
DBType		thisUserDBType;
short		count;

	thisUserDBType = GetInBoxTypeForCurrentUser();

	count = DBCountItems( thisUserDBType );
	if( count > kMaxInBoxEntries )
	{
		count = kMaxInBoxEntries;
		ERROR_MESG( "Server should never let in-box entries go greater than kMaxInBoxEntries!!" );
	}
			
	return count;
}

short _CountOutBoxEntries( void )
{
DBType		thisUserDBType;
short		count;

	thisUserDBType = GetOutBoxTypeForCurrentUser();

	count = DBCountItems( thisUserDBType );
	return count;
}

DBID _GetInBoxGraphicID( short whichScreen )
{
DBID	myMailGraphicID;
short	count;
//
// Get an icon depending on how much mail we've got:
// 3 levels: none, some, full.
//	
	count = CountInBoxEntries();

	if( whichScreen == kMainScreen )
	{
		myMailGraphicID = kMailIcon;
		if( count == 0 )
			myMailGraphicID = kNoMail;
		else
		if( count == kMaxInBoxEntries )
			myMailGraphicID = kOverflowingMail;
	}
	else
	if( whichScreen == k4WayMailScreen )
	{
		myMailGraphicID = kPersonalMailIcon;
		if( count == 0 )
			myMailGraphicID = kPersonalNoMailIcon;
		else
		if( count == kMaxInBoxEntries )
			myMailGraphicID = kPersonalOverflowingMailIcon;
	}

	return myMailGraphicID;
}

void _AddMailToOutBox( Mail *theMail )
{
DBType		thisUserDBType;
DBID		thisID;

	thisUserDBType = GetOutBoxTypeForCurrentUser();
	DBAddItem(thisUserDBType, DBGetUniqueID(thisUserDBType), theMail);
}


void _AddMailToInBox( Mail *theMail )
{
DBType		thisUserDBType;
DBID		thisID;

//
// Only the server should be making this call!!
//
	thisUserDBType = GetInBoxTypeForCurrentUser();

	if( CountInBoxEntries() >= kMaxInBoxEntries )
	{
		ERROR_MESG( "In box full, you should not be adding entries" );
	}

	thisID = DBGetUniqueID( thisUserDBType );
	DBAddItem( thisUserDBType, thisID, (Ptr) theMail );
}

void _RemoveMailFromInBox( short index )
{
DBType		thisUserDBType;
DBID		thisID;

	thisUserDBType = GetInBoxTypeForCurrentUser();
	if( CountInBoxEntries() == 0 )
	{
		ERROR_MESG( "Nothing in the in box to delete!" );
	}
	
	thisID = GetInBoxIDFromIndex( index );
	DBDeleteItem( thisUserDBType, thisID );
}

Mail *_GetIndexInBoxMail( short index )
{
DBID	curID;
DBType	thisUserDBType;

	thisUserDBType = GetInBoxTypeForCurrentUser();

	curID = GetInBoxIDFromIndex( index );	

	return ( (Mail *)DBGetItem( thisUserDBType, curID ) );
}

Mail *_GetIndexOutBoxMail( short index, long *size )
{
DBID	curID;
DBType	thisUserDBType;

	thisUserDBType = GetOutBoxTypeForCurrentUser();

	curID = GetOutBoxIDFromIndex( index );	
	*size = DBGetItemSize( thisUserDBType, curID );
	
	return ( (Mail *)DBGetItem( thisUserDBType, curID ) );
}

void _MarkMailItemRead( short index )
{
DBType	thisUserDBType;
Mail 	*theMail;
DBID	curID;

	thisUserDBType = GetInBoxTypeForCurrentUser();
	curID = GetInBoxIDFromIndex( index );	
	theMail = DBGetItem( thisUserDBType, curID );
	theMail->hasThisMailBeenRead = 1;
	DBTypeChanged( thisUserDBType );
}















