/*
	File:		Opponent.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<11>	 8/12/94	JOE		Set/GetOpponentPersonificationDBID no longer used
		<10>	  8/9/94	SAH		Added Get and Set OpponentPersonificationDBID.
		 <9>	 7/21/94	BET		add #ifdef __SERVER__
		 <8>	 7/17/94	DJ		added opponentVerificationTag
		 <7>	  7/5/94	SAH		Added ClearOldOpponent.
		 <6>	 6/28/94	SAH		Only keep track of the current opponent.
		 <5>	 6/18/94	KON		Remove ClearOpponent.
		 <4>	 6/15/94	KON		Added ClearOpponent.
		 <3>	 5/26/94	SAH		Managerised.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.

	To Do:
*/


#ifndef __Opponent__
#define __Opponent__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifdef __SERVER__
// do something so the server can use the file

#else

phoneNumber * GetOpponentPhoneNumber( void ) = 
	CallDispatchedFunction( kGetOpponentPhoneNumber );

void SetOpponentPhoneNumber( phoneNumber *thePhoneNumber ) = 
	CallDispatchedFunction( kSetOpponentPhoneNumber );

userIdentification * GetCurOpponentIdentification( void ) = 
	CallDispatchedFunction( kGetCurOpponentIdentification );

void SetCurOpponentIdentification( userIdentification *theName ) = 
	CallDispatchedFunction( kSetCurOpponentIdentification );

char * GetCurOpponentTaunt( void ) = 
	CallDispatchedFunction( kGetCurOpponentTaunt );

char * GetCurOpponentInfo( void ) = 
	CallDispatchedFunction( kGetCurOpponentInfo );

void ClearOldOpponent( void ) = 
	CallDispatchedFunction( kClearOldOpponent );

long GetOpponentVerificationTag( void ) =
	CallDispatchedFunction( kGetOpponentVerificationTag );

void SetOpponentVerificationTag( long theTag ) =
	CallDispatchedFunction( kSetOpponentVerificationTag );
	
#endif //__SERVER__

#endif
