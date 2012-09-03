/*
	File:		NewAddressMgr.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	  7/7/94	CMY		MinimizeUserHandle - strips spaces and case-sensitivity from a
									handle.
		 <1>	 6/18/94	KON		first checked in

	To Do:
*/


#ifndef __NewAddressMgr__
#define __NewAddressMgr__

#define kUpdateOnly		0
#define kUpdateOrAdd	1

void UpdateAddressBookStuff( short action ) =
	CallDispatchedFunction( kUpdateAddressBookStuff );

static Boolean AddPlayerToAddressBook( short wins, short losses, userIdentification *theUserID ) =
	CallDispatchedFunction( kAddPlayerToAddressBook );

void AddOnDeckAddressBookEntry( void ) =
	CallDispatchedFunction( kAddOnDeckAddressBookEntry );
	
void MinimizeUserHandle( char *handle ) =
	CallDispatchedFunction( kMinimizeUserHandle );


#endif __NewAddressMgr__