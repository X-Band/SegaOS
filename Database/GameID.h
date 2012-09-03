/*
	File:		GameID.h

	Contains:	xxx put contents here xxx

	Written by:	Steve Perlman

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 7/15/94	dwh		unix-ise.
		 <2>	  6/4/94	SAH		Managerized.
		 <2>	 5/24/94	SAH		Made the non-simulator version of IDBlock a long.

	To Do:
*/

#ifndef __GameID__
#define __GameID__

#ifndef	unix
long GetGameID( void ) =
	CallDispatchedFunction( kGetGameID );
#endif

#endif __GameID__
