/*
	File:		PatchDB.h

	Contains:	Goo for handling game patches

	Written by:	Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/21/94	JOE		Add GetGamePatchFlags()
		<14>	 8/19/94	ADS		GetPatchVersion -> GetGamePatchVersion for clarity
		<13>	 8/18/94	JBH		Added intelligence to game-patch disposing when low on memory.
									Add game patches high in the database heap to help avoid
									fragmentation.
		<12>	 8/15/94	JOE		Add DisposeGamePatch
		<11>	  8/6/94	ATM		GT.h ended up in GamePatch.h, breaking UNIX server build.
									#ifdefed out GamePatch.h.
		<10>	  8/6/94	HEC		Added GameDoDialog
		 <9>	  8/4/94	SAH		Added UpdateGameResultsAfterError.
		 <8>	  8/3/94	ATM		Moved #include "GT.h" into non-server area; the GameTalk
									includes are too unUNIXable, and the include file is only needed
									for prototypes.
		 <7>	  8/1/94	HEC		In with the new, out with the old.
		 <6>	 7/26/94	HEC		Added HandleCallWaiting proto.
		 <5>	 7/21/94	BET		add #ifdef __SERVER__
		 <4>	 7/21/94	BET		Fix caps on DataBase.c
		 <3>	 5/31/94	HEC		Removed definition of _PatchDBControl which I accidentally added
									here.
		 <2>	 5/31/94	HEC		Massively revised for new database.

	To Do:
*/


#ifndef __PatchDB__
#define __PatchDB__

#ifndef __Errors__
#include "Errors.h"
#endif

#ifndef __Database__
#include "DataBase.h"
#endif

typedef struct
{
	long 	gameID;
	long	patchVersion;		/* must be greater than -1 */
	long	patchFlags;
	long	codeSize;
	long	data[1];			/* patch code starts here */
} PatchDescriptor;


#ifdef __SERVER__
// do something so the server can use these files

#else
#ifndef __GamePatch__
#include "GamePatch.h"
#endif

#ifndef __GT__
#include "GT.h"
#endif

/************************************************************************/
/*	 Lame routines
/************************************************************************/

OSErr	GameDoDialog ( GPDialogRec *gpd ) =
	CallDispatchedFunction( kGameDoDialog );

OSErr	CreateGameDispatcher ( void ) =
	CallDispatchedFunction( kCreateGameDispatcher );

OSErr	InitGamePatch ( Boolean master, GTSession * commsession, long gameID ) =
	CallDispatchedFunction( kInitGamePatch );

OSErr	PlayCurrentGame ( Boolean master, GTSession * commsession, long gameFlags ) =
	CallDispatchedFunction( kPlayCurrentGame );

Err AddGamePatch( const PatchDescriptor *newPatch)
	= CallDispatchedFunction( kAddGamePatch );

PatchDescriptor * FindGamePatch( long gameID, DBID *patchID )
	= CallDispatchedFunction( kFindGamePatch );

long GetGamePatchVersion( long gameID )
	= CallDispatchedFunction( kGetGamePatchVersion );

long GetGamePatchFlags( long gameID )
	= CallDispatchedFunction( kGetGamePatchFlags );

Err LoadGamePatch( long gameID )
	= CallDispatchedFunction( kLoadGamePatch );

void DisposeGamePatch( void )
	= CallDispatchedFunction( kDisposeGamePatch );

OSErr StartGame( long gameFlags )
	= CallDispatchedFunction( kStartGame );

OSErr	UpdateGameResultsAfterError ( OSErr defaultErr ) =
	CallDispatchedFunction( kUpdateGameResultsAfterError );


/* NOTE:
	To kill a game patch, get the database id of the game patch
	by calling FindGamePatch(), then either set its purge value
	or delete the item.
*/

long	DisposeOldestGamePatch( void ) =
	CallDispatchedFunction( kDisposeOldestGamePatch );
	
void	MarkGamePatchUsed( long gameID ) =
	CallDispatchedFunction( kMarkGamePatchUsed );

#endif //__SERVER__
#endif __PatchDB__
