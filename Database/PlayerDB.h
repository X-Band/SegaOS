/*
	File:		PlayerDB.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<19>	 8/27/94	CMY		Added PreflightNewAddressEntry.
		<18>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<17>	 8/14/94	SAH		Added CorrelateAddressBookEntry.
		<16>	  8/5/94	DJ		name it struct PlayerInfo in typedef
		<15>	 7/21/94	BET		Add padding to structs.
		<14>	 7/19/94	KON		Removed iconRef from PlayerInfo since it's part of the
									userIdentification now.
		<13>	 7/17/94	KON		Add routines to check if address book is changed, and to mark it
									unchanged.
		<12>	 7/16/94	BET		unix-ise include file capitalization.
		<11>	  7/6/94	DJ		added #ifndef __SERVER__
		<10>	  7/5/94	CMY		SendNewAddressesToServer.
		 <9>	 6/20/94	KON		Remove duplicate definition of kMaxAddressBookEntries.
		 <8>	 6/18/94	KON		Moved the new address stuff to NewAddressMgr.c.
		 <7>	 6/17/94	KON		More address book dialog stuff.
		 <6>	 6/15/94	KON		Rename AddADDadd to UpdateAddressBookStuff.
		 <5>	 6/15/94	KON		Add functions to update address book.
		 <4>	 6/14/94	KON		Add AddAddressBookEntry.
		 <3>	 6/13/94	SAH		Managerized.
		 <2>	  6/1/94	KON		Fixed function prototypes and added #ifndef. Fix UsrConfig to
									UsrConfg.
		 <1>	  6/1/94	HEC		first checked in

	To Do:
*/

#ifndef __PlayerDB__
#define __PlayerDB__

#ifndef __Database__
#include "DataBase.h"
#endif

#ifndef __UsrConfg__
#include "UsrConfg.h"
#endif

// current user vs. this opponent

typedef struct PlayerInfo {
	userIdentification	userId;
	long				dateLastPlayed;
	short				wins;
	short				losses;
	DBID				serverUniqueID;
	char				filler;
	char				info[4];
} PlayerInfo;

#ifndef __SERVER__

short CountAddressBookEntries( void ) = 
	CallDispatchedFunction( kCountAddressBookEntries );

DBErr RemoveAddressBookEntry( short index ) = 
	CallDispatchedFunction( kRemoveAddressBookEntry );

PlayerInfo *GetIndexAddressBookEntry( short index ) = 
	CallDispatchedFunction( kGetIndexAddressBookEntry );

void AddAddressBookEntry( PlayerInfo * theEntry ) = 
	CallDispatchedFunction( kAddAddressBookEntry );

short GetUserAddressBookIndex( userIdentification *theUserID ) =
	CallDispatchedFunction( kGetUserAddressBookIndex );

void SendNewAddressesToServer( void) =
	CallDispatchedFunction( kSendNewAddressesToServer );

void MarkAddressBookUnchanged( void) =
	CallDispatchedFunction( kMarkAddressBookUnchanged );

Boolean AddressBookHasChanged( void) =
	CallDispatchedFunction( kAddressBookHasChanged );

void CorrelateAddressBookEntry ( short index, short serverUniqueID ) =
	CallDispatchedFunction( kCorrelateAddressBookEntry );
	
Boolean PreflightNewAddressEntry ( void ) =
	CallDispatchedFunction( kPreflightNewAddressEntry );

#endif __SERVER__

#endif __PlayerDB__
