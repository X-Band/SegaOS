/*
	File:		UsrConfgPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 6/28/94	SAH		Killed userID. Added full localUser identification.
		 <3>	 6/20/94	KON		Make all the UserConfig shit DBID based rather than a short.
		 <2>	 5/26/94	SAH		Managerised.
		 <1>	 5/26/94	KON		Checked in for KON the hoser.

	To Do:
*/


#ifndef __UsrConfgPriv__
#define __UsrConfgPriv__


typedef
struct UsrConfgGlobals
{
	userIdentification	localUser;
} UsrConfgGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE UsrConfgGlobals
#else
extern UsrConfgGlobals user;
#endif

#endif __UsrConfgPriv__