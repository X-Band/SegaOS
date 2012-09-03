/*
	File:		GameDB.c

	Contains:	NGP 0 contains the game patch list.
				NGP 1 contains the game names, in the order that they appear in NGP 0.

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/19/94	JBH		When a new NGP list comes down, we will clear out any
									out-of-date game patches (except for current game, which will
									happen when server downloads the new patch)
		 <8>	  8/9/94	JOE		Nuke ClearGameNames()
		 <7>	  8/1/94	SAH		Fixed other zero count fuckers.
		 <6>	  8/1/94	SAH		Learned what zero based counts are.
		 <5>	 7/19/94	JOE		Game names are now stored in NGP 1
		 <4>	 6/30/94	SAH		Removed database purge values.
		 <3>	 6/18/94	KON		Added game patch version and game patch info calls.
		 <2>	 6/14/94	SAH		Managerized.
		 <5>	 5/25/94	HEC		new memory manager
		 <4>	 5/23/94	KON		Add warning message if game name doesn't exist.
		 <3>	 5/19/94	KON		If NGPList doesn't exist, return a warning instead of a warning.
		 <2>	 5/17/94	KON		Fix bug in clear game names.

	To Do:
*/

#include "GameDB.h"
#include "DataBase.h"
#include "Errors.h"
#include "DBTypes.h"
#include "PatchDBHeaps.h"
#include "heaps.h"
#include "PatchDB.h"
#include "GameID.h"


long		_GetNGPListGamePatchInfo( long gameID );
long		_GetNGPListGamePatchVersion( long gameID );
long		_GetNGPVersion( void );
void		_UpdateNGPList( NGPData *myData );
void 		_UpdateNameList( Ptr names );
char 	*	_GetGameName( long gameID );


long
_GameDBControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* install our selectors */
			SetDispatchedFunction ( kGetNGPListGamePatchInfo,	kGameDBManager,	_GetNGPListGamePatchInfo );
			SetDispatchedFunction ( kGetNGPListGamePatchVersion,	kGameDBManager,	_GetNGPListGamePatchVersion );
			SetDispatchedFunction ( kGetNGPVersion,		kGameDBManager,	_GetNGPVersion );
			SetDispatchedFunction ( kUpdateNGPList,		kGameDBManager,	_UpdateNGPList );
			SetDispatchedFunction ( kUpdateNameList,	kGameDBManager,	_UpdateNameList );
			SetDispatchedFunction ( kGetGameName,		kGameDBManager,	_GetGameName );
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


//
// Network Game Patch (NGP) Database (entry 0)
//

long _GetNGPListGamePatchInfo( long gameID )
{
NGPData *myData;
short	iii;

	myData = (NGPData *)DBGetItem( kNGPType, 0 );
	if( myData )
	{
		for( iii = 0; iii <= myData->count; iii++ )
		{
			if( myData->gameInfo[iii].gameID == gameID )
				return myData->gameInfo[iii].gameFlags;
		}
	}
	else
	{
		WARNING_MESG( "NGPList is not allocated" );
	}
//
// Game not found
//
	return kGameNotRecognizedByNGPList;
}

long _GetNGPListGamePatchVersion( long gameID )
{
NGPData *myData;
short	iii;

	myData = (NGPData *)DBGetItem( kNGPType, 0 );
	if( myData )
	{
		for( iii = 0; iii <= myData->count; iii++ )
		{
			if( myData->gameInfo[iii].gameID == gameID )
				return myData->gameInfo[iii].patchVersion;
		}
	}
	else
	{
		WARNING_MESG( "NGPList is not allocated" );
	}
//
// Game not found
//
	return kGameNotRecognizedByNGPList;
}

long _GetNGPVersion( void )
{
NGPData *myData;

	myData = (NGPData *)DBGetItem( kNGPType, 0 );
	if( myData )
		return myData->version;
	else
	{
		WARNING_MESG( "NGPList is not allocated" );
		return -1;
	}
}

void _UpdateNGPList( NGPData *myData )
{
short iii;
DBID	patchID;
PatchDescriptor *patch;
long	thisGameID;

	ASSERT( myData );
	DBAddItem( kNGPType, 0, (Ptr) myData );
	
	thisGameID = GetGameID();
	
	/* look thru all patches in the list */
	for ( iii = 0; iii <= myData->count; iii++ )
	{
		/* don't trash the current game's game patch (if there is a new version, the server */
		/* will overwrite our current one anyway...) */
		if ( myData->gameInfo[iii].gameID != thisGameID )
		{
			patch = FindGamePatch( myData->gameInfo[iii].gameID, &patchID );
			
			/* if we found a patch on the box with this gameID, and it is older, nuke it! */
			if ( patch && ( patch->patchVersion < myData->gameInfo[iii].patchVersion ) )
			{
				DBDeleteItem( kGamePatchType, patchID );
			}
		}
	}	
}

void _UpdateNameList( Ptr names )
{
	ASSERT( names );
	DBAddItem( kNGPType, 1, names );
}

char *_GetGameName( long gameID )
{
NGPData *myData;
Ptr		nameData;
short	iii;

	myData = (NGPData *)DBGetItem( kNGPType, 0 );
	nameData = DBGetItem( kNGPType, 1 );
	
	if( myData && nameData )
	{
		for( iii = 0; iii <= myData->count; iii++ )
		{
			if( myData->gameInfo[iii].gameID == gameID )
			{
				while ( iii )
				{
					if ( *nameData == 0 )
						iii--;
					nameData++;
				}
				return( nameData );
			}
		}
	}

	return(0);
}








