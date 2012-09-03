/*
	File:		DBPurge.c (aka memMgr client unload.c)

	Contains:	Code to allow us to purge a DB item and continue box operation

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/19/94	JBH		De-managerized - now it's part of kDatabaseManager.
		 <1>	 8/19/94	JBH		first checked in

	To Do:
*/

#include "OSManagers.h"
#include "Dispatcher.h"
#include "SegaOs.h"
#include "DBPurge.h"
#include "Errors.h"
#include "DBTypes.h"

#include "PatchDB.h"
#include "GameID.h"

#include "SegaGraphics.h"
#include "UsrConfg.h"
#include "BoxSer.h"

Boolean _AllowDBItemPurge( void *item, DBID itemID, DBType itemType );

/* This function knows about the specific data types. If this gets much more complicated than */
/* 2 or 3 types, perhaps we should put them in their respective managers instead */

/* This is separate from DB.c so that we can try to make the database manager not know what */
/* data is stored in it. */

Boolean	_AllowDBItemPurge( void *item, DBID itemID, DBType itemType )
{
	switch ( itemType )
		{
		case kGamePatchType :
			{
			PatchDescriptor	*thePatch = (PatchDescriptor *) item;
			
			/* don't nuke the current game patch! */
			if ( thePatch->gameID == GetGameID() )
				return false;
			}
			break;
		case kRAMIconType :
			{
			RAMIconBitMapPtr	RAICData = (RAMIconBitMapPtr) item;
			BoxSerialNumber		*thisBox;
			
			thisBox = GetBoxSerialNumber();
			
			/* don't nuke custom icons owned by the users of this box! */
			if ( RAICData->box.region == thisBox->region && RAICData->box.box == thisBox->box )
				return false; 
			}
			
			/* nuke the associated frame rate info, if it's there */
			DBDeleteItem( kIconDescType, itemID );
			
			break;
		default :
			break;
		}

	return true;		/* assume it's OK to nuke. */
}
