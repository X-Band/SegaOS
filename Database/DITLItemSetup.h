/*
	File:		DITLItemSetup.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 7/20/94	CMY		Each item may use the x & y from the Laid, intead of from its
									own data.
		<14>	  7/6/94	ADS		Moved per-screen info to ScreenLists.h
		<13>	  7/6/94	ADS		Added player info options screen
		<12>	  7/5/94	CMY		Layouts for empty mail and address book.
		<11>	 6/28/94	JOE		added kRankingScreen.
		<10>	 6/13/94	SAH		Made functions dispatched through the ditl manager.
		 <9>	  6/5/94	CMY		DITL to ask for replay.
		 <8>	  6/4/94	KON		Added generic ProcessDITLScreen.
		 <7>	  6/3/94	CMY		Added Options screen.
		 <6>	  6/3/94	KON		Added top level interface screen.
		 <5>	  6/3/94	KON		Add cursor X and Y offset.
		 <4>	  6/2/94	KON		Remove stale enum, add kChoose4PlayerScreen.
		 <3>	  6/1/94	KON		Add support for address book.
		 <2>	 5/31/94	KON		Add support for control tables.
		 <1>	 5/30/94	KON		first checked in

	To Do:
*/


#ifndef __DITLItemSetup__
#define __DITLItemSetup__

#ifndef __DataBase__
#include "DataBase.h"
#endif __DataBase__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__


typedef struct
{
	short	xPos;
	short	yPos;
	DBID	typeID;
	DBID	cursorID;
	DBID	objectDataRef;
}ItemDesc;

typedef struct
{
	short		count;
	short		cursorXOffset;
	short		cursorYOffset;
	ItemDesc	itemDesc[1];
} ScreenLayout;

unsigned char ProcessDITLScreen( DBID ditlRefNum, unsigned char curSelection ) = 
	CallDispatchedFunction( kProcessDITLScreen );

DITLItemList *SetupDITLItemList( DBID ditlLayout ) = 
	CallDispatchedFunction( kSetupDITLItemList );

Ptr SetupDITLObjectData( short objectType, ItemDesc *item ) = 
	CallDispatchedFunction( kSetupDITLObjectData );

void DisposeDITLItemList( DITLItemList * theDitlList ) = 
	CallDispatchedFunction( kDisposeDITLItemList );

ControlTable *SetupControlTable( DBID controlRef ) = 
	CallDispatchedFunction( kSetupControlTable );

void DisposeControlTable( ControlTable *myControlTable ) = 
	CallDispatchedFunction( kDisposeControlTable );


#endif __DITLItemSetup__