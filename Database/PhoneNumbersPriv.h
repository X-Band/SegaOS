/*
	File:		PhoneNumbersPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/28/56	SAH		Killed the globals.
		 <1>	 6/11/94	KON		first checked in

	To Do:
*/


typedef
struct PhoneNumberGlobals
{
	short	dummy;
} PhoneNumberGlobals;



#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE		PhoneNumberGlobals
#else
extern PhoneNumberGlobals numbers;
#endif

