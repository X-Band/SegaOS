/*
	File:		DataBase.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <5>	 5/19/94	KON		Added include for PatchDBHeaps.
		 <4>	 5/19/94	KON		Added DBGetItemSize.
		 <3>	 5/17/94	KON		Added switch between resource manager based DB and jevans DB.
		 <2>	 5/12/94	KON		Added ability to set purge state. This is stubbed out for the
									SIMULATOR.

	To Do:
*/

#ifdef RESOURCE_DATABASE	// use this one instead of the real one.  -dj

#include "SegaOS.h"			/* Shannon's stuff */
#include "DataBase.h"
#include "heaps.h"
#include "Errors.h"
#include "PatchDBHeaps.h"

void DBInitialize(void)
{
}


Ptr DBGetItem( DBType type, DBID theID )
{
//
// Check SRAM database first, if failure, check ROM database
//
Handle	myResource;

	myResource = GetResource( type, theID );
	HLock( myResource );

	if( myResource )
		return( *myResource );
	else
		return 0;	//for now
}

DBErr DBAddItem( DBType type, DBID theID, DBPurge thePurgeValue, Ptr theData )
{
Handle	theDataHandle;
Handle	existingResource;
long	ptrSize;
//
// Remove the existing one if it's there
//
	existingResource = GetResource( type, theID );
	if( existingResource )
	{
		RmveResource( existingResource );
		DisposeHandle( existingResource );
	}
		
	ASSERT( theData );
	if( theData )
	{
		ptrSize = GetPtrSize( theData );
		ASSERT_MESG( ptrSize, "You must pass real pointers into DBAddItem!" );
		PtrToHand( theData, &theDataHandle, ptrSize );
		AddResource( theDataHandle, type, theID, "\p" );
		if( ResError() )
		{
			ERROR_MESG( "ResError in DBAddItem" );
		}
		WriteResource( theDataHandle );
	}
	return 0;
}

DBErr DBDeleteItem( DBType type, DBID theID )
{
Handle	existingResource;

	existingResource = GetResource( type, theID );
	if( existingResource )
	{
		RmveResource( existingResource );
		DisposeHandle( existingResource );
	}
	return( ResError() );
}

long DBGetItemSize( DBType type, DBID theID )
{
	return GetSegaPtrSize( DBGetItem( type, theID ) );
}

//
// Iterators
//
DBID DBGetFirstItemID( DBType type )
{
Handle	myResource;
Str255	myName;
short	myID;
ResType	myType;

//
// Make sure ROM database is handled properly!!
//

	myResource = GetIndResource( type, 1 );
	GetResInfo( myResource, &myID, &myType, myName );
	if( ResError() )
	{
		ERROR_MESG( "ResError in DBGetFirstItemID" );
		myID = -1;
	}
	return myID;
}

DBID DBGetNextItemID( DBType type, DBID thePrevID )
{
Handle	myResource;
Str255	myName;
short	myID;
ResType	myType;
Handle	previousResource;
short	resCount;
short	iii;
//
// Make sure ROM database is handled properly!!
//

//
// Mapping this onto the Resource manager is a bit of a pain!
// First find the current resources index number
//
	previousResource = GetResource( type, thePrevID );
	resCount = CountResources( type );
	for( iii = 1; iii <= resCount; iii++ )
	{
		if( previousResource == GetIndResource( type, iii ) )
			break;
	}

	if( previousResource != GetIndResource( type, iii ) )
	{
		ERROR_MESG( "Majorly hosed in DBGetNextItemID: previous resource not found" );
	}
	else
	{
		if( iii == resCount )
			return -1;			//That's it
			
		myResource = GetIndResource( type, iii+1 );
		GetResInfo( myResource, &myID, &myType, myName );
		if( ResError() )
		{
			ERROR_MESG( "ResError in DBGetNextItemID" );
		}
	}
	return myID;	
}

short DBCountItems( DBType type )
{
	return CountResources( type );
}

DBID DBGetUniqueID( DBType type )
{
	return( UniqueID( type ) );
}

//
// Purge stuff
//
DBPurge DBGetItemPurgeValue( DBType type, DBID theID )
{
//
// ROM database items always return unpurgeable!
//

}

//
// Set an items purge state. This is used by the SendQ to mark times
// after they've been sent.
//
DBErr DBSetItemPurgeValue( DBType type, DBID theID, DBPurge newPurgeValue )
{
DBErr	result;

	result = 0;
	
	return result;
}

//
// Creating/deleting types
//
DBErr DBNewItemType( DBType type )
{

}

DBErr DBDeleteItemType( DBType type )
{

}

#endif RESOURCE_DATABASE

