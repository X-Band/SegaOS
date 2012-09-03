/*
	File:		SendQ.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/27/94	KON		Add debugChatScriptConst800 to differentiate between X25 chat
									sendQ items and 800 number sendQ items.
		 <8>	 8/22/94	DJ		moved kRestartInfoType from DBTypes.h, and added kChatDebugType
		 <7>	  8/4/94	SAH		Simplified
		 <6>	 7/15/94	SAH		Added kGameResultsDBID.
		 <5>	 7/13/94	CMY		Removed mail from SendQ
		 <4>	 6/30/94	DJ		#ifndef __SERVER__
		 <3>	 6/30/94	SAH		Added SendQ types. Changed AddItemToSendQ to take data directly
									rather than a dbase entry.
		 <2>	 6/14/94	SAH		Managerized.
		 <2>	 5/12/94	KON		Add support for kSNDNoAction proc as a done action. Removed
									flush action proc since I don't know what that means...
		 <1>	 5/12/94	KON		first checked in

	To Do:
*/

#ifndef __SendQ__
#define __SendQ__


#ifndef __DataBase__
#include "DataBase.h"
#endif __DataBase__


/* reserved sendq id's for the new way */
enum
{
	/* game results ids */
	kGameResultDataDBID,
	kRestartInfoType,
	kChatDebugType,
	kDebugChatScriptConst800,

	/* don't delete this */
	kLastSendQID
};


#ifndef __SERVER__
//
// SNDDoneAction has the following values
//
#define	kSNDNoAction		1
#define kSNDDelete			2

DBErr AddItemToSendQ( DBID id, void * data ) =
	CallDispatchedFunction( kAddItemToSendQ );

DBErr AddItemSizeToSendQ( DBID id, void * data, long size ) =
	CallDispatchedFunction( kAddItemSizeToSendQ );

void DeleteSendQ( void ) =
	CallDispatchedFunction( kDeleteSendQ );

void KillSendQItem( DBID theID ) =
	CallDispatchedFunction( kKillSendQItem );

DBID GetFirstSendQElementID( void ) =
	CallDispatchedFunction( kGetFirstSendQElementID );

DBID GetNextSendQElementID( DBID prevID ) =
	CallDispatchedFunction( kGetNextSendQElementID );

short CountSendQElements( void ) =
	CallDispatchedFunction( kCountSendQElements );

void * GetSendQElement( DBID theID, long * size ) =
	CallDispatchedFunction( kGetSendQElement );


#endif __SERVER__

#endif __SendQ__

