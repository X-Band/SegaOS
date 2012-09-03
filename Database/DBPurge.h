/*
	File:		DBPurge.h

	Contains:	xxx put contents here xxx

	Written by:	Josh Horwich

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 8/19/94	JBH		first checked in
	To Do:
*/

#ifndef __DBPurge__
#define __DBPurge__

#ifndef __SegaOS__
#include "SegaOs.h"
#endif

#ifndef __DataBase__
#include "DataBase.h"
#endif

Boolean AllowDBItemPurge( void *item, DBID itemID, DBType itemType ) =
	CallDispatchedFunction( kAllowDBItemPurge );

#endif __DBPurge__
