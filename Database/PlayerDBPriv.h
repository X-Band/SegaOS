/*
	File:		PlayerDBPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 7/17/94	KON		first checked in

	To Do:
*/



#ifndef __PlayerDBPriv__
#define __PlayerDBPriv__

typedef struct AddressBookGlobals
{
	short	addressBookChanged;		//bitfield which indicates whether each user has read the news
} AddressBookGlobals;

#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	AddressBookGlobals
#else
extern AddressBookGlobals addressGlobals;
#endif

#endif

