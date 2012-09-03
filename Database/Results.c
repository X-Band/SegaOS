/*
	File:		Results.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 6/14/94	SAH		Managerized.
		 <5>	 5/25/94	HEC		New memory manager
		 <4>	 5/23/94	KON		Added support for four user result FIFO's.
		 <3>	 5/19/94	KON		Made count items call DBCountItems.
		 <2>	 5/12/94	KON		Update to use the send Q rather than special messages and
									control for just sending results. Ripped out all the flush code.

	To Do:
*/

//
// Update to only use the history database for the current user
//

#include "SegaOS.h"
#include "Results.h"
#include "GameID.h"
#include "PatchDBHeaps.h"
#include "Errors.h"
#include "DBTypes.h"
#include "Heaps.h"
#include "Reliability.h"
#include "DataBase.h"
#include "SendQ.h"
#include "UsrConfg.h"

static DBType GetFIFOTypeForCurrentUser( void );


void		_AddToResultFIFO( Result *myResult );
void		_ReplaceTopEntryOfResultFIFO( Result *newResult );
Result *	_GetTopEntryOfResultFIFO( void );
Result *	_GetIndexEntryInResultFIFO( short index );
short		_CountEntriesInResultFIFO( void );


long
_ResultsControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* install our selectors */
			SetDispatchedFunction ( kAddToResultFIFO,			kResultsManager,	_AddToResultFIFO );
			SetDispatchedFunction ( kReplaceTopEntryOfResultFIFO,	kResultsManager,	_ReplaceTopEntryOfResultFIFO );
			SetDispatchedFunction ( kGetTopEntryOfResultFIFO,		kResultsManager,	_GetTopEntryOfResultFIFO );
			SetDispatchedFunction ( kGetIndexEntryInResultFIFO,	kResultsManager,	_GetIndexEntryInResultFIFO );
			SetDispatchedFunction ( kCountEntriesInResultFIFO,	kResultsManager,	_CountEntriesInResultFIFO );
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

static DBType GetFIFOTypeForCurrentUser( void )
{
short	userID;
DBType	userType;

	userID = GetCurUserID();
	
	switch( userID )
	{
		case 0:
			userType = kResultFIFOType0;
			break;
			
		case 1:
			userType = kResultFIFOType1;
			break;
	
		case 2:
			userType = kResultFIFOType2;
			break;
	
		case 3:
			userType = kResultFIFOType3;
			break;	
			
		default:
			ERROR_MESG( "Unknown user name" );
			userType = kResultFIFOType0;
			break;
	}
	
	return userType;
}

void _AddToResultFIFO( Result *myResult )
{
FIFOEntry 	*firstEntry;
FIFOEntry 	*tempEntry;
FIFOEntry 	*thisEntry;
FIFOEntry 	*fEntry;
FIFOEntry 	*dbEntry;
short		entryCount;
DBID		lastButOneID;
DBID		lastID;
DBID		lastButTwoID;
DBID		thisID;
DBType		thisUserDBType;

	thisUserDBType = GetFIFOTypeForCurrentUser();
//
// find the FIFO size and get pointers to the last and last but one entries
// NOTE: the ID's are the next pointer, so the last entry is always zero
// when coming out of this loop, and lastButOneID has the ID of the last
// entry and lastButTwoID has the ID of the next to last entry. Confusing, huh?
//  ID 0 in FIFO database is special: it always points to the FIFO head
//
//
// Automatically add the new entry to the send Q so the server can get updated.
//
	thisEntry = (FIFOEntry *)NewMemory( false, sizeof( FIFOEntry ) );
	tempEntry = (FIFOEntry *)NewMemory( false, sizeof( FIFOEntry ) );
	firstEntry = (FIFOEntry *)NewMemory( false, sizeof( FIFOEntry ) );

	thisEntry->thisResult = *myResult;
	
	fEntry = (FIFOEntry *)DBGetItem( thisUserDBType, 0 );
	
	entryCount = 0;
	lastButOneID = 0;
	lastID = 0;
	if( fEntry )
	{
		*firstEntry = *fEntry;
		lastID = fEntry->nextID;
		lastButOneID = 0;
		while( lastID )
		{
			lastButTwoID = lastButOneID;
			lastButOneID = lastID;
			dbEntry = (FIFOEntry *)DBGetItem( thisUserDBType, lastID );
			ASSERT_MESG( dbEntry, "things are hosed" );
			if( !dbEntry )
			{
				return;
			}
			lastID = dbEntry->nextID;
			entryCount++;
		}
	}
	else
	{
//
// The FIFO doesn't exist yet, create the 0th and 1st entry
//
		firstEntry->nextID = 1;
		thisEntry->nextID = 0;
		SetDBTypeUnstable( thisUserDBType, true );
		DBAddItem( thisUserDBType, 0, kDBNotPurgeable, (Ptr) firstEntry );
		thisID = 1;
		DBAddItem( thisUserDBType, thisID, kDBNotPurgeable, (Ptr) thisEntry );
		SetDBTypeUnstable( thisUserDBType, false );
		goto Cleanup;
	}
	
//
// if the FIFO is full, reuse the last entry and move it to the head
//
	if( entryCount == kFIFOSize )
	{
		dbEntry = (FIFOEntry *)DBGetItem( thisUserDBType, lastButTwoID );
		*tempEntry = *dbEntry;
		tempEntry->nextID = 0;	//move last but one to the end

		SetDBTypeUnstable( thisUserDBType, true );

//
// move the old last but one to the end
//
			DBAddItem( thisUserDBType, lastButTwoID, kDBNotPurgeable, (Ptr) tempEntry );
//
// make the old last entry, now thisEntry, the top FIFO entry
//	
			thisEntry->nextID = firstEntry->nextID;
			thisID = lastButOneID;
			DBAddItem( thisUserDBType, thisID, kDBNotPurgeable, (Ptr) thisEntry );
//
// update the first entry to point to thisEntry, formerly the lastButOneID
//	
			firstEntry->nextID = lastButOneID;
			DBAddItem( thisUserDBType, 0, kDBNotPurgeable, (Ptr) firstEntry );
	
		SetDBTypeUnstable( thisUserDBType, false );
	
	}
	else
	{	
		thisID = DBGetUniqueID( thisUserDBType );
//
// else add a new entry to the head
//
		SetDBTypeUnstable( thisUserDBType, true );

//
// create a new entry at lastButOneID + 1 and make it the top FIFO entry
//	
			thisEntry->nextID = firstEntry->nextID;
			DBAddItem( thisUserDBType, thisID, kDBNotPurgeable, (Ptr) thisEntry );
//
// update the first entry to point to thisEntry, formerly the lastButOneID
//	
			firstEntry->nextID = thisID;
			DBAddItem( thisUserDBType, 0, kDBNotPurgeable, (Ptr) firstEntry );

		SetDBTypeUnstable( thisUserDBType, false );
	}

Cleanup:
//
// Add this entry to the sendQ
//
	AddDBItemToSendQ( thisUserDBType, thisID, kSNDNoAction );
	DisposeMemory( thisEntry );
	DisposeMemory( firstEntry );
	DisposeMemory( tempEntry );

}

void _ReplaceTopEntryOfResultFIFO( Result *newResult )
{
FIFOEntry 	thisEntry;
FIFOEntry 	*fEntry;
FIFOEntry 	*replaceEntry;
DBType		thisUserDBType;

	thisUserDBType = GetFIFOTypeForCurrentUser();

//
// prepare the new top entry
//
	thisEntry.thisResult = *newResult;
	
//
// get the top entry
//
	fEntry = (FIFOEntry *)DBGetItem( thisUserDBType, 0 );
	ASSERT( fEntry );
	replaceEntry = (FIFOEntry *)DBGetItem( thisUserDBType, fEntry->nextID );
	thisEntry.nextID = replaceEntry->nextID;
	DBAddItem( thisUserDBType, fEntry->nextID, kDBNotPurgeable, (Ptr) &thisEntry );
}

Result *_GetTopEntryOfResultFIFO( void )
{
FIFOEntry 	*fEntry;
DBType		thisUserDBType;

	thisUserDBType = GetFIFOTypeForCurrentUser();

	fEntry = (FIFOEntry *)DBGetItem( thisUserDBType, 0 );
	ASSERT( fEntry );
	fEntry = (FIFOEntry *)DBGetItem( thisUserDBType, fEntry->nextID );
	ASSERT( fEntry );
	return &fEntry->thisResult;
	
}

Result *_GetIndexEntryInResultFIFO( short index )
{
FIFOEntry 	*dbEntry;
short		entryCount;
short		nextID;
DBType		thisUserDBType;

	thisUserDBType = GetFIFOTypeForCurrentUser();

//
// walk to this index or the end, whichever comes first
//
// Don't return the first entry because it's a dummy entry!!
//
	
	dbEntry = (FIFOEntry *)DBGetItem( thisUserDBType, 0 );
	
	ASSERT( dbEntry );
	if( dbEntry )
	{
		nextID = dbEntry->nextID;
		for( entryCount = 0; entryCount < index+1; entryCount++ )
		{
			if( nextID == 0 )
				return 0;
				
			dbEntry = (FIFOEntry *)DBGetItem( thisUserDBType, nextID );
			ASSERT( dbEntry );
			nextID = dbEntry->nextID;
		}
	}
	else
		return 0;
		
	return &dbEntry->thisResult;

}


short _CountEntriesInResultFIFO( void )
{
DBType		thisUserDBType;

	thisUserDBType = GetFIFOTypeForCurrentUser();
	return DBCountItems( thisUserDBType ) - 1;
}

