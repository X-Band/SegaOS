/*
	File:		UsrConfg.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<30>	 8/24/94	KON		Remove customer service phone number reference.
		<29>	 8/22/94	KON		Make FillInUserIdentification get the local user clut ID (really
									hue shift) rather than initing it to 255.
		<28>	 8/20/94	JOE		SetCurUserID() now inits colorTableID
		<27>	 8/20/94	KON		Moved serverUniqueID to PlayerInfo and put ColorTableID in its
									place in userIdentification.
		<26>	 8/18/94	ADS		Assertions to protect array lookups
		<25>	 8/18/94	ADS		Hide gBoxID globals.
		<24>	 8/18/94	HEC		Don't index based on invalid user id in SetCurUserID. In
									GetCurUserID, assert getting an invalid user id.
		<23>	 8/18/94	JOE		I suck!  recalc the gBoxID globals checksum after diddling the
									validation flags
		<22>	 8/12/94	JOE		Custom icon updates
		<21>	 8/11/94	JOE		local user validation/invalidation stuff update
		<20>	  8/9/94	JOE		install _IsUserValidated
		<19>	  8/4/94	ADS		Preset new field in the userInformation
		<18>	  8/3/94	JOE		get taunt/info strings the right way
		<17>	  8/3/94	JOE		Added more user info manipulating routines
		<16>	 7/31/94	KON		The customer support number for failed passwords now comes from
									a writeable string.
		<15>	 7/19/94	DJ		set and get of user icons, plus update hometown and usericons in
									SetCurUserID
		<14>	  7/8/94	ADS		Add special mode for connect-disable-password
		<13>	  7/6/94	ADS		Changes to PW functs
		<12>	  7/6/94	SAH		Don't call SetCurUserID during soft init!
		<11>	  7/6/94	ADS		Added password functions
		<10>	 8/28/56	SAH		Moved the local access number stuff to PhoneNumbers.c
		 <9>	  7/2/94	SAH		Made use real globals.
		 <8>	 6/28/94	SAH		Killed Results.h
		 <7>	 6/28/94	SAH		Updated to new local user id scheme.
		 <6>	 6/20/94	KON		Made the user config stuff take a DBID rather than a short.
		 <5>	  6/4/94	KON		Use kDBIDErrorValue instead of -1 or 255 which is pretty fucking
									ambiguous.
		 <4>	  6/1/94	KON		Add check to make sure current user ID never exceeds the maximum
									number of local users.
		 <3>	 5/26/94	SAH		Managerised.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.

	To Do:
*/

#include "UsrConfg.h"
#include "UsrConfgPriv.h"
#include "DataBase.h"
#include "Errors.h"
#include "DBTypes.h"
#include "SegaText.h"
#include "BoxSer.h"
#include "DialogMgr.h"
#include "Personification.h"
#include "DBConstants.h"
#include "StringDB.h"
#include "Utils.h"

#ifndef SIMULATOR
UsrConfgGlobals user;
#endif


userIdentification 	*	_GetCurrentLocalUser( void );

void					_FillInUserIdentification( unsigned char userID, userIdentification *user );

char				*	_GetLocalUserTaunt ( unsigned char userID );
void					_SetLocalUserTaunt ( unsigned char userID, char *taunt );
char				*	_GetLocalUserInfo ( unsigned char userID );
void					_SetLocalUserInfo ( unsigned char userID, char *info );

void					_SetCurUserID( unsigned char theUser );
unsigned char			_GetCurUserID(  void );

Boolean					_VerifyPlayerPassword( short playerNum );
Boolean					_EmptyPW( PasswordString* playerPW );
Boolean					_ComparePW( PasswordString* pw1, PasswordString* pw2 );
void					_GetPlayerPW( short playerNum, PasswordString* pw);



long
_UsrConfgControl ( short command, long data )
{
long	offset;
short	error;
UsrConfgGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
		
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(user,offset);
			
			error = AllocateGlobalSpace ( kUsrConfgManager, offset, sizeof(UsrConfgGlobals),
				(Ptr *) &globals );
			if ( error != noErr )
				{
				return error;
				}
			
			/* set the current local user name to nil. copy the box serial number */
			/* whether it's right or not. it will be nil if not inited yet */
			globals->localUser.userName[ 0 ] = 0;
			globals->localUser.userID = 0;
			globals->localUser.box.region = -1;		// gBoxID->boxID.region;	  ADS 8-18-94
			globals->localUser.box.box = -1;		// gBoxID->boxID.box;		  ADS 8-18-94
			
			/* install our selectors */
			SetDispatchedFunction ( kGetCurrentLocalUser,	kUsrConfgManager,	_GetCurrentLocalUser );
			SetDispatchedFunction ( kFillInUserIdentification,	kUsrConfgManager,	_FillInUserIdentification );
			
			SetDispatchedFunction ( kGetLocalUserTaunt,		kUsrConfgManager,	_GetLocalUserTaunt );
			SetDispatchedFunction ( kSetLocalUserTaunt,		kUsrConfgManager,	_SetLocalUserTaunt );
			SetDispatchedFunction ( kGetLocalUserInfo,		kUsrConfgManager,	_GetLocalUserInfo );
			SetDispatchedFunction ( kSetLocalUserInfo,		kUsrConfgManager,	_SetLocalUserInfo );

			SetDispatchedFunction ( kSetCurUserID,			kUsrConfgManager,	_SetCurUserID );
			SetDispatchedFunction ( kGetCurUserID,			kUsrConfgManager,	_GetCurUserID );

			SetDispatchedFunction ( kVerifyPlayerPassword,	kUsrConfgManager,	_VerifyPlayerPassword );
			SetDispatchedFunction ( kIsEmptyPassword,		kUsrConfgManager,	_EmptyPW );
			SetDispatchedFunction ( kComparePassword,		kUsrConfgManager,	_ComparePW );
			SetDispatchedFunction ( kGetPlayerPassword,		kUsrConfgManager,	_GetPlayerPW );
			
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


// This might return a "blank" user, during that time in which we
// haven't selected a user yet.

userIdentification *_GetCurrentLocalUser( void )
{
userIdentification *theName;

	return &REFGLOBAL(user,localUser);
}


void _FillInUserIdentification( unsigned char userID, userIdentification *user )
{
	ASSERT(userID < kMaxLocalUsers);
	
	user->box.region		= GetBoxSerialNumber()->region;
	user->box.box			= GetBoxSerialNumber()->box;
	user->userID 			= userID;
	user->colorTableID	 	= GetLocalUserCustomROMClutID( userID );
	user->ROMIconID 		= GetLocalUserROMIconID( userID );
	CopyCString(*GetBoxHometown( ), user->userTown );
	CopyCString(*GetLocalUserName ( userID ), user->userName );
}


char *_GetLocalUserTaunt ( unsigned char userID )
{
	ASSERT(userID < kMaxLocalUsers);
	
	return ( GetWriteableString ( userID ) );
}

void _SetLocalUserTaunt ( unsigned char userID, char *taunt )
{
	ASSERT(userID < kMaxLocalUsers);
	
	DeleteWriteableString( userID );
	SetWriteableString( userID, taunt );
}

char *_GetLocalUserInfo ( unsigned char userID )
{
	ASSERT(userID < kMaxLocalUsers);
	
	return ( GetWriteableString ( userID + kMaxLocalUsers ) );
}

void _SetLocalUserInfo ( unsigned char userID, char *info )
{
	ASSERT(userID < kMaxLocalUsers);
	
	DeleteWriteableString( userID + kMaxLocalUsers );
	SetWriteableString( userID + kMaxLocalUsers, info );
}


void _SetCurUserID( unsigned char theUser )
{
	REFGLOBAL(user,localUser).box.region 		= GetBoxSerialNumber()->region;
	REFGLOBAL(user,localUser).box.box 			= GetBoxSerialNumber()->box;
	REFGLOBAL(user,localUser).userID 			= theUser;
	REFGLOBAL(user,localUser).colorTableID	 	= 0;

	if (theUser == kDBIDErrorValue)
	{
		REFGLOBAL(user,localUser).ROMIconID = kDBIDErrorValue;
		REFGLOBAL(user,localUser).userTown[0] = 0;
		REFGLOBAL(user,localUser).userName[0] = 0;
	}
	else
	{
		REFGLOBAL(user,localUser).ROMIconID		= GetLocalUserROMIconID( theUser );
		REFGLOBAL(user,localUser).colorTableID	= GetLocalUserCustomROMClutID( theUser );
		CopyCString(*GetBoxHometown( ), REFGLOBAL(user,localUser).userTown );
		CopyCString(*GetLocalUserName ( theUser ), REFGLOBAL(user,localUser).userName );
	}
}

unsigned char _GetCurUserID( void )
{
	ASSERT_MESG( REFGLOBAL(user,localUser).userID != kDBIDErrorValue,
		"CurUserID is 255!" );
	return (REFGLOBAL(user,localUser).userID );
}


///  Functions to support passwords
///		(here for now, unless they find a better home)



Boolean _VerifyPlayerPassword( short playerNum )
{
PasswordString	playerPW;
PasswordString	enteredPW;
short			cmd1, cmd2;
DBID			try, fail;
char *			theString;

	if (playerNum == -1)	
	{
		long* dest = (long*) &playerPW;
		DBGetConstant( kConnectPasswordHiConst, dest );
		++dest;
		DBGetConstant( kConnectPasswordLoConst, dest );
		
		try = kConnectPasswordDialog;
		fail = kConnectPWFailDialog;
	}
	else
	{
		GetPlayerPW( playerNum, &playerPW );
		
		try = kEnterPasswordDialog;
		fail = kWrongPasswordDialog;
	}
		
	if (EmptyPW( &playerPW ))
		return true;			// no password for this user
	
	DoDialogParam( try, &enteredPW );
	
	if (ComparePW( &playerPW, &enteredPW))
		return true;
	
	DoDialog( fail );
	
	return false;
}


Boolean _EmptyPW( PasswordString* playerPW )
{
	return ((*playerPW)[0] == kEndOfPW);
}


Boolean _ComparePW( PasswordString* pw1, PasswordString* pw2 )
{
	short	iii;
	
	for (iii = 0; iii < sizeof(PasswordString); ++iii)	{
		if ((*pw1)[iii] != (*pw2)[iii])
			return false;
		if ((*pw1)[iii] == kEndOfPW)
			return true;
	}
	
	return true;
}


// hardcoded!
void _GetPlayerPW( short playerNum, PasswordString* pw)
{
	PasswordString* result;
	result = GetPersonificationPart(playerNum, kPersonificationPassword);
	CopyPassword(result, pw);
}




