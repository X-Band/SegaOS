/*
	File:		OpponentPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 8/12/94	JOE		Set/GetOpponentPersonificationDBID no longer used
		 <4>	  8/9/94	SAH		Added the opponentPersonificationDBID global.
		 <3>	 7/17/94	DJ		added opponentVerificationTag
		 <2>	 6/28/94	SAH		Only keep track of the current opponent.
		 <1>	 5/26/94	SAH		first checked in

	To Do:
*/




typedef
struct OpponentGlobals
{
	phoneNumber			gOpponentPhoneNumber;
	userIdentification	opponent;
	long				opponentVerificationTag;
} OpponentGlobals;



#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE		OpponentGlobals
#else
extern OpponentGlobals opponent;
#endif

