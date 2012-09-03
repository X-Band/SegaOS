/*
	File:		UserIcons.h

	Contains:	User Icons (animated or static) screen layout item stuff header

	Written by:	Joe Britt 

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  7/3/94	JOE		first checked in

	To Do:
*/

#ifndef __UserIcon__
#define __UserIcon__

#ifndef __SegaIcons__
#include "SegaIcons.h"
#endif

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__


typedef struct UserIconPriv
{
	IconReference	iconRef;
} UserIconPriv;


doDITLItemProc GetUserIconItemPtr( void );


#endif __UserIcon__
