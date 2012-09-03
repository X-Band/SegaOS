/*
	File:		StringDBPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 7/31/94	KON		Add writeable string calls.
		 <1>	 6/13/94	SAH		first checked in

	To Do:
*/



/* manager functions dispatched through the text manager */
void		_DrawDBXYString( DBID stringID );
xyString *	_GetDBXYString( DBID stringID );
char *		_GetSegaString( DBID stringID );
char *		_GetWriteableString( DBID stringID );
char *		_SetWriteableString( DBID stringID, char *theString );
void 		_DeleteWriteableString( DBID stringID );
DBID 		_GetUniqueWriteableStringID( void );
