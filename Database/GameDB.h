/*
	File:		GameDB.h

	Contains:	xxx put contents here xxx

	Written by:	David Jevans

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<10>	  8/9/94	JOE		Nuke ClearGameNames()
		 <9>	 7/28/94	KON		Change gameInfo to match resource file and hopefully also be
									unix safe.
		 <8>	 7/21/94	BET		unix-ise.
		 <7>	 7/19/94	JOE		Game names are now in NGP 1
		 <6>	 7/17/94	DJ		made gameName a single string instead of 2
		 <5>	 6/18/94	KON		Updated NGP list to add gameFlags and patchVersions.
		 <4>	 6/14/94	SAH		Managerized.
		 <3>	 6/11/94	DJ		added kMortalKombatGameID and kNBAJamGameID
		 <2>	 5/31/94	DJ		added #ifdef __SERVER__

	To Do:
*/


#ifndef __GameDB__
#define __GameDB__


#ifndef __SegaOS__
#include "SegaOS.h"
#endif __SegaOS__


#define kGameNameSize	32
#define kGameNotRecognizedByNGPList	-1L
#define kPatchDoesNotExist			-1L

enum
{
	kOnePlayerCompetitivePlaySupported	= 1,
	kTwoPlayerNetworkPlaySupported		= 2,
	kFourPlayerNetworkPlaySupported		= 4
};

typedef struct
{
	long	gameID;
	long	gameFlags;
	long	patchVersion;
} gameInfo;

typedef struct
{
	short		count;		//0-based count
	short		version;
	gameInfo	gameInfo[1];
} NGPData;


#define kMortalKombatGameID	0xAB6348E9
#define kNBAJamGameID		0xE30C296E


#ifndef __SERVER__

long GetNGPListGamePatchInfo( long gameID ) =
	CallDispatchedFunction( kGetNGPListGamePatchInfo );

long GetNGPListGamePatchVersion( long gameID ) =
	CallDispatchedFunction( kGetNGPListGamePatchVersion );

long GetNGPVersion( void ) =
	CallDispatchedFunction( kGetNGPVersion );

void UpdateNGPList( NGPData *myData ) =
	CallDispatchedFunction( kUpdateNGPList );

void UpdateNameList( Ptr names ) =
	CallDispatchedFunction( kUpdateNameList );

char *GetGameName( long gameID ) =
	CallDispatchedFunction( kGetGameName );

#endif __SERVER__


#endif __GameDB__
