/*
	File:		PhoneNumbers.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/18/94	ADS		Obsolete
		 <8>	 8/15/94	HEC		Don't install string in db from box phone.
		 <7>	  8/9/94	HEC		Include errors.h
		 <6>	  8/3/94	HEC		Set the box's phone number string in the database for radio
									buttons to access.
		 <5>	 8/28/56	SAH		Changed base phone number to local access number.
		 <4>	 6/30/94	SAH		Made the 800 number point at the ROM.
		 <3>	 6/14/94	DJ		800 number is in the ROM db now
		 <2>	 6/11/94	DJ		returning NULL in 800GetNum
		 <1>	 6/11/94	KON		first checked in. These need to be made to use SRAM storage
									rather than globals. Blasting out this version so we have
									something to get started with!

	To Do:
*/

#include "UsrConfg.h"
#include "OSGlobals.h"
#include "PhoneNumbers.h"
#include "PhoneNumbersPriv.h"
#include "BoxSer.h"
#include "DBTypes.h"
#include "StringDB.h"
#include "Errors.h"



#ifndef SIMULATOR
PhoneNumberGlobals numbers;
#endif


	REMOVE THIS FILE - IT'S OBSOLETE
	


static phoneNumber *_GetBoxPhoneNumber( void );
static void	_SetBoxPhoneNumber( phoneNumber *thePhoneNumber );
static phoneNumber *_GetLocalAccessPhoneNumber( void );
static void	_SetLocalAccessPhoneNumber( phoneNumber *thePhoneNumber );
static phoneNumber *_Get800PhoneNumber( void );


long
_PhoneNumberControl ( short command, long data )
{
long offset;
short error;
PhoneNumberGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:			
			/* install our selectors */
			SetDispatchedFunction ( kGetBoxPhoneNumber,			kPhoneNumberManager,	_GetBoxPhoneNumber );
			SetDispatchedFunction ( kSetBoxPhoneNumber,			kPhoneNumberManager,	_SetBoxPhoneNumber );
			SetDispatchedFunction ( kGetLocalAccessPhoneNumber,	kPhoneNumberManager,	_GetLocalAccessPhoneNumber );
			SetDispatchedFunction ( kSetLocalAccessPhoneNumber,	kPhoneNumberManager,	_SetLocalAccessPhoneNumber );
			SetDispatchedFunction ( kGet800PhoneNumber,			kPhoneNumberManager,	_Get800PhoneNumber );
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

phoneNumber *_GetBoxPhoneNumber( void )
{
	return &gBoxID->boxPhoneNumber;
}

void _SetBoxPhoneNumber( phoneNumber *thePhoneNumber )
{
	gBoxID->boxPhoneNumber = *thePhoneNumber;
	ChangedBoxIDGlobals();
}

phoneNumber *_GetLocalAccessPhoneNumber( void )
{
	return &gBoxID->localAccessNumber;
}

void _SetLocalAccessPhoneNumber( phoneNumber *thePhoneNumber )
{
	gBoxID->localAccessNumber = *thePhoneNumber;
	ChangedBoxIDGlobals();
	SetIMovedOption(0);	// reset "i've moved" option
}

phoneNumber *_Get800PhoneNumber( void )
{
	return( &g800Number );
}
