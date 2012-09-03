/*
	File:		NewAddressMgrPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/18/94	KON		Remove icon field since its in the userIdentification now.
		 <1>	 6/18/94	KON		first checked in

	To Do:
*/


typedef struct NewAddressMgrGlobals
{
	short	 			playerWaitingForDelete;
	short				wins;
	short				losses;
	long				date;
	userIdentification	userID;	
} NewAddressMgrGlobals;


#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE		NewAddressMgrGlobals
#else
extern NewAddressMgrGlobals newAddressGlobals;
#endif

