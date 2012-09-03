/*
	File:		DITLItemSetupPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 7/20/94	CMY		Each item may use the x & y from the Laid, intead of from its
									own data.
		 <2>	 7/19/94	ADS		Vector Mania
		 <1>	 6/13/94	SAH		first checked in

	To Do:
*/


#ifndef __DITLItemSetup__
#include "DITLItemSetup.h"
#endif

/*
* These are functions that live in DitlItemSetup.c but are dispatched through the
* DITL manager.
*/

extern unsigned char	_ProcessDITLScreen( DBID ditlRefNum, unsigned char curSelection );
extern DITLItemList *	_SetupDITLItemList( DBID ditlLayout );
extern Ptr				_SetupDITLObjectData( short objectType, ItemDesc *item );
extern void				_DisposeDITLItemList( DITLItemList * theDitlList );
extern ControlTable *	_SetupControlTable( DBID controlRef );
extern void				_DisposeControlTable( ControlTable *myControlTable );
extern Ptr				_GetDITLObjectData( DBID objectDataRef );
