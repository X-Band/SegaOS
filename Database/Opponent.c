/*
	File:		Opponent.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 8/12/94	JOE		Set/GetOpponentPersonificationDBID no longer used
		<11>	  8/9/94	SAH		Added Get and Set OpponentPersonificationDBID.
		<10>	  8/9/94	HEC		Include errors.h
		 <9>	 7/17/94	DJ		added opponentVerificationTag
		 <8>	  7/5/94	SAH		Added ClearOldOpponent. Clear the opponent in there rather than
									at soft init time.
		 <7>	 6/28/94	SAH		Only keep track of the current opponent.
		 <6>	 6/18/94	KON		Remove ClearOpponent.
		 <5>	 6/15/94	KON		Clear opponent on hard initialize. Set ID to an error value
									rather than 0.
		 <4>	 6/15/94	KON		Add clear opponent.
		 <3>	 5/26/94	SAH		Managerised.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.

	To Do:
*/

#include "UsrConfg.h"
#include "Opponent.h"
#include "DBTypes.h"
#include "DataBase.h"
#include "OpponentPriv.h"
#include "Errors.h"
#include "StringDB.h"


#ifndef SIMULATOR
OpponentGlobals opponent;
#endif


short					_GetCurOpponentID( void );
phoneNumber *			_GetOpponentPhoneNumber( void );
void					_SetOpponentPhoneNumber( phoneNumber *thePhoneNumber );
userIdentification *	_GetCurOpponentIdentification( void );
void					_SetCurOpponentIdentification( userIdentification *theName );
char			 	*	_GetCurOpponentTaunt( void );
char				*	_GetCurOpponentInfo( void );
long					_GetOpponentVerificationTag( void );
void					_SetOpponentVerificationTag( long theTag );
void					_ClearOldOpponent ( void );

long
_OpponentControl ( short command, long data )
{
long offset;
short error;
OpponentGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
		
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(opponent,offset);
			
			error = AllocateGlobalSpace ( kOpponentManager, offset, sizeof(OpponentGlobals),
				(Ptr *) &globals );
			if ( error != noErr )
				{
				return error;
				}
			
			globals->gOpponentPhoneNumber.phoneNumber[0] = 0;
			globals->opponent.box.region = 0;
			globals->opponent.box.box = 0;
			globals->opponent.userID = kDBIDErrorValue;
			globals->opponent.userName[ 0 ] = 0;
			globals->opponentVerificationTag = 0;
			
			/* install our selectors */
			SetDispatchedFunction ( kGetOpponentPhoneNumber,		kOpponentManager,	_GetOpponentPhoneNumber );
			SetDispatchedFunction ( kSetOpponentPhoneNumber,		kOpponentManager,	_SetOpponentPhoneNumber );
			SetDispatchedFunction ( kGetCurOpponentIdentification,	kOpponentManager,	_GetCurOpponentIdentification );
			SetDispatchedFunction ( kSetCurOpponentIdentification,	kOpponentManager,	_SetCurOpponentIdentification );
			SetDispatchedFunction ( kGetCurOpponentTaunt,			kOpponentManager,	_GetCurOpponentTaunt );
			SetDispatchedFunction ( kGetCurOpponentInfo,			kOpponentManager,	_GetCurOpponentInfo );
			SetDispatchedFunction ( kGetOpponentVerificationTag,	kOpponentManager,	_GetOpponentVerificationTag );
			SetDispatchedFunction ( kSetOpponentVerificationTag,	kOpponentManager,	_SetOpponentVerificationTag );
			SetDispatchedFunction ( kClearOldOpponent,				kOpponentManager,	_ClearOldOpponent );
			
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


phoneNumber *_GetOpponentPhoneNumber( void )
{
	return &REFGLOBAL(opponent,gOpponentPhoneNumber);
}

void _SetOpponentPhoneNumber( phoneNumber *thePhoneNumber )
{
	REFGLOBAL(opponent,gOpponentPhoneNumber) = *thePhoneNumber;
}

userIdentification * _GetCurOpponentIdentification( void )
{
	return &REFGLOBAL(opponent,opponent);
}

void _SetCurOpponentIdentification( userIdentification *theIdentification )
{
	REFGLOBAL(opponent,opponent) = *theIdentification;
}


char *_GetCurOpponentTaunt( void )
{
	return( GetWriteableS