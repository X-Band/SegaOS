/*
	File:		DeferredDialogMgrPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/15/94	JBH		Added globals to maintain enabled / disabled state
		 <1>	 6/20/94	KON		first checked in

	To Do:
*/


typedef struct DefDialog
{
	short	when;
	DBID	userID;
	CPDialog theDialog;
} DefDialog;

typedef
struct DeferredDialogGlobals
{
	Boolean		disabled;
} DeferredDialogGlobals;


#ifdef SIMULATOR
#define MANAGERGLOBALTYPE DeferredDialogGlobals
#else

extern DeferredDialogGlobals deferredDialogs;

#endif
