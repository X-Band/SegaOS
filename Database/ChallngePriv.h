/*
	File:		ChallngePriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  6/4/94	KON		first checked in

	To Do:
*/


typedef
struct ChallengeGlobals
{
	phoneNumber 		gChallengePhoneNumber;
	userIdentification	gChallengeIdentification;
} ChallengeGlobals;

#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	ChallengeGlobals
#else
extern ChallengeGlobals challenge;
#endif
