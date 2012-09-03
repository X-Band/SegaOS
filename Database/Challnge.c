/*
	File:		Challnge.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 7/13/94	KON		Init some globals on hard initialize.
		 <4>	  6/4/94	SAH		Fixed the GetMgrOffset macro for sega.
		 <3>	  6/4/94	KON		Managerize.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.

	To Do:
*/

#include "UsrConfg.h"
#include "Challnge.h"
#include "ChallngePriv.h"
#include "OSManagers.h"
#include "Errors.h"

//
// Prototypes - dispatched functions
//

phoneNumber * _GetChallengePhoneNumber( void );
void _SetChallengePhoneNumber( phoneNumber *thePhoneNumber );
userIdentification * _GetChallengeIdentification( void );
void _SetChallengeIdentification( userIdentification *theIdentification );

#ifndef SIMULATOR
ChallengeGlobals challenge;
#endif	


long
_ChallengeControl ( short command, long data )
{
long offset;
short error;
ChallengeGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(challenge,offset);
			
			error = AllocateGlobalSpace ( kChallengeManager, offset,
				sizeof(ChallengeGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create challenge globals" );
			if ( error != noErr )
				{
				return error;
				}
		
			globals->gChallengeIdentification.box.box = kFindNetworkOpponentSerialNumber;
			globals->gChallengeIdentification.box.region = kFindNetworkOpponentSerialNumber;
			/* install our selectors */
			SetDispatchedFunction ( kGetChallengePhoneNumber,		kChallengeManager,	_GetChallengePhoneNumber );
			SetDispatchedFunction ( kSetChallengePhoneNumber,		kChallengeManager,	_SetChallengePhoneNumber );
			SetDispatchedFunction ( kGetChallengeIdentification,	kChallengeManager,	_GetChallengeIdentification );
			SetDispatchedFunction ( kSetChallengeIdentification,	kChallengeManager,	_SetChallengeIdentification );
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

phoneNumber * _GetChallengePhoneNumber( void )
{
	return &REFGLOBAL( challenge, gChallengePhoneNumber );
}

void _SetChallengePhoneNumber( phoneNumber *thePhoneNumber )
{
	REFGLOBAL( challenge, gChallengePhoneNumber ) = *thePhoneNumber;
}

userIdentification * _GetChallengeIdentification( void )
{
	return &REFGLOBAL( challenge, gChallengeIdentification );
}

void _SetChallengeIdentification( userIdentification *theIdentification )
{
	REFGLOBAL( challenge, gChallengeIdentification ) = *theIdentification;
}

