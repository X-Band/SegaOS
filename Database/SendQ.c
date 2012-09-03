/*
	File:		SendQ.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/14/94	ADS		Removed obsolete #include
		 <8>	 8/12/94	HEC		Renamed errors.
		 <7>	  8/4/94	SAH		Simplified. type is now the DBID.
		 <6>	 7/19/94	ADS		Vector Mania
		 <5>	 7/15/94	SAH		Added some more stuff before we kill this one.
		 <4>	  7/8/94	DJ		fixed compare of DBID from -1 to kDBIDErrorValue
		 <3>	 6/30/94	SAH		Removed database purge values. Made AddItemToSendQ take the data
									directly rather than a dbase entry.
		 <2>	 6/14/94	SAH		Managerized.
		 <3>	 5/25/94	HEC		New memory manager
		 <2>	 5/12/94	KON		Add support for kSNDNoAction proc as a done action. Removed
									flush action proc since I don't know what that means...
		 <1>	 5/12/94	KON		first checked in

	To Do:
*/

#include "DataBase.h"
#include "DBTypes.h"
#include "Errors.h"
#include "SendQ.h"
#include "PatchDBHeaps.h"
#include "heaps.h"

DBErr 	_AddItemToSendQ( DBID id, void * data );
DBErr	_AddItemSizeToSendQ( DBID id, void * data, long size );
void	_DeleteSendQ( void );
void	_KillSendQItem( DBID theID );
DBID	_GetFirstSendQElementID( void );
DBID	_GetNextSendQElementID( DBID prevID );
short	_CountSendQElements( void );
void *	_GetSendQElement( DBID theID, long * size );

//
// local, vectorized
//
static void _RemoveItemFromSendQ( DBID theID );

void RemoveItemFromSendQ( DBID theID ) =
		CallDispatchedFunction( kRemoveItemFromSendQ );




long
_SendQControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* install our selectors */
			SetDispatchedFunction ( kAddItemToSendQ,		kSendQManager,	_AddItemToSendQ );
			SetDispatchedFunction ( kAddItemSizeToSendQ,	kSendQManager,	_AddItemSizeToSendQ );
			SetDispatchedFunction ( kDeleteSendQ,			kSendQManager,	_DeleteSendQ );
			SetDispatchedFunction ( kKillSendQItem,			kSendQManager,	_KillSendQItem );
			SetDispatchedFunction ( kGetFirstSendQElementID,kSendQManager,	_GetFirstSendQElementID );
			SetDispatchedFunction ( kGetNextSendQElementID,	kSendQManager,	_GetNextSendQElementID );
			SetDispatchedFunction ( kCountSendQElements,	kSendQManager,	_CountSendQElements );
			SetDispatchedFunction ( kGetSendQElement,		kSendQManager,	_GetSendQElement );

			SetDispatchedFunction ( kRemoveItemFromSendQ,	kSendQManager,	_RemoveItemFromSendQ );
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


DBErr _AddItemToSendQ( DBID id, void * data )
{
	return AddItemSizeToSendQ ( id, data, GetMemorySize ( data ) );
}


DBErr _AddItemSizeToSendQ( DBID id, void * data, long size )
{
DBErr	err;

	err = kOutOfMemoryErr;

	/* preflight the add to the database */
	if ( DBPreflight ( kSendQType, id, size, kPreflightLight ) == noErr )
		{
		err = DBAddItemPtrSize( kSendQType, id, (Ptr) data, size );
		}
	
	ASSERT( !err );
	
	return err;
}


void _KillSendQItem( DBID theID )
{
//
// Kill the item without doing the done action
//
	DBDeleteItem( kSendQType, theID );
}

static void _RemoveItemFromSendQ( DBID theID )
{
	DBDeleteItem( kSendQType, theID );
}

void _DeleteSendQ( void )
{
short count;
short iii;
DBID theID;

	count = CountSendQElements();
	if( count == 0 )
		return;

	theID = GetFirstSendQElementID();
	while( theID != kDBIDErrorValue )
	{
		RemoveItemFromSendQ( theID );
		theID = GetFirstSendQElementID();
	}		
}

DBID _GetFirstSendQElementID( void )
{
DBID	theID;
short	theCount;

	theCount = DBCountItems( kSendQType );
	if( theCount == 0 )
		return kDBIDErrorValue;
		
	return( DBGetFirstItemID( kSendQType ) );

}

DBID _GetNextSendQElementID( DBID prevID )
{
DBID	theID;

	return( DBGetNextItemID( kSendQType, prevID ) );
}

void * _GetSendQElement( DBID theID, long * size )
{
Ptr	elm;

	elm = DBGetItem( kSendQType, theID );
	
	if ( size )
		{
		*size = DBGetItemSize ( kSendQType, theID );
		}
	
	return elm;
}

short _CountSendQElements( void )
{
	return( DBCountItems( kSendQType ) );
}








