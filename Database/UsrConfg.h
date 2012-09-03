/*
	File:		UsrConfg.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<17>	 8/18/94	ADS		Hide gBoxID globals
		<16>	 8/12/94	JOE		custom icon updates
		<15>	 8/11/94	JOE		local user pers validation stuff update
		<14>	  8/3/94	JOE		added more user info manipulating routines
		<13>	 7/19/94	DJ		added get and set of user icons
		<12>	 7/15/94	dwh		unix-ise.
		<11>	  7/6/94	ADS		Added CopyPassword
		<10>	  7/6/94	ADS		Added password functions
		 <9>	 8/28/56	SAH		Moved the LocalAccess Number stuff to PhoneNumbers.c
		 <8>	 6/28/94	SAH		Rearranged calls for new user id.
		 <7>	 6/20/94	KON		Make all the user config stuff take a DBID rather than a short.
		 <6>	 6/18/94	KON		Add kOpponentRangeEnd.
		 <5>	  6/1/94	KON		Set max local users to 4.
		 <4>	 5/28/94	BET		Fix a Stink C + minor misunderstanding
		 <3>	 5/26/94	SAH		Managerised.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.
		 <2>	 5/24/94	DJ		added #ifndef __SERVER__ because server doesn't want to see the
									prototypes, just the typedefs.  dj

	To Do:
*/


#ifndef __UsrConfg__
#define __UsrConfg__

#ifndef __SegaOS__
#include "SegaOs.h"
#endif

#ifndef __Database__
#include "DataBase.h"
#endif

//
// User ID 0-3 is reserverved for local users
// ID's 32 or greater are reserved for opponents
//
#define kOpponentRangeStart 32
#define kOpponentRangeEnd	250


///
// password stuff
//
#define	kMaxPWLength	8
#define	kEndOfPW		0x00

typedef char	PasswordString[kMaxPWLength];

#define CopyPassword(src, dst)							\
	{	long *s, *d; s = (long*) src; d = (long*) dst; 	\
		*d++ = *s++; *d = *s;	}




#ifndef __SERVER__	// server doesn't want the func protos

void FillInUserIdentification( unsigned char userID, userIdentification *user ) = 
	CallDispatchedFunction( kFillInUserIdentification );

userIdentification * GetCurrentLocalUser( void ) = 
	CallDispatchedFunction( kGetCurrentLocalUser );

char *GetLocalUserTaunt ( unsigned char userID ) =
	CallDispatchedFunction( kGetLocalUserTaunt );

void SetLocalUserTaunt ( unsigned char userID, char *taunt ) =
	CallDispatchedFunction( kSetLocalUserTaunt );

char *GetLocalUserInfo ( unsigned char userID ) =
	CallDispatchedFunction( kGetLocalUserInfo );

void SetLocalUserInfo ( unsigned char userID, char *info ) =
	CallDispatchedFunction( kSetLocalUserInfo );

void SetCurUserID( unsigned char theUser ) = 
	CallDispatchedFunction( kSetCurUserID );

unsigned char GetCurUserID( void ) = 
	CallDispatchedFunction( kGetCurUserID );


Boolean	VerifyPlayerPassword( short playerNum ) =
	CallDispatchedFunction( kVerifyPlayerPassword );

Boolean	EmptyPW( PasswordString* playerPW ) =
	CallDispatchedFunction( kIsEmptyPassword );

Boolean	ComparePW( PasswordString* pw1, PasswordString* pw2 ) =
	CallDispatchedFunction( kComparePassword );

void	GetPlayerPW( short playerNum, PasswordString* pw) =
	CallDispatchedFunction( kGetPlayerPassword );


#endif __SERVER__

#endif __UsrConfg__
