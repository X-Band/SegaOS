/*
	File:		DitlMgr.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 7/15/94	dwh		unix-ise.
		<14>	  7/9/94	CMY		Added support for PersonButton DITL items.
		<13>	  7/3/94	JOE		added kUserIcon DITL item type
		<12>	 6/30/94	JOE		added kStatIcon DITL item type
		<11>	 6/29/94	JOE		added kStatIconGraphics DITL item type
		<10>	 6/28/94	JOE		added kStatText DITL item type, changed kSelectProcSetsCursor
									from -1 to 255 so it would work.
		 <9>	 6/20/94	KON		Added kTextButtonGraphics type.
		 <8>	 6/10/94	KON		Add ClearDITLDone.
		 <7>	  6/3/94	KON		Add cursorX and Y offset.
		 <6>	  6/1/94	KON		Added kAddressItem
		 <5>	 5/31/94	KON		Add pinning to control tables.
		 <4>	 5/28/94	SAH		Updated to new cursor interface so we canhave one cursor per
									ditl.
		 <3>	 5/28/94	KON		Pass controller reads into DITL proc. Make it use the
									dispatcher.
		 <2>	 5/27/94	KON		Added kMailItem.
		 <6>	 5/20/94	SAH		Added support for cursors.
		 <5>	 5/18/94	KON		Moved textbutton to it's own file.
		 <4>	 5/18/94	HEC		changed DITLItemmemory type to long
		 <3>	 5/17/94	KON		Added support for DisposeDITL and text buttons.
		 <2>	 5/17/94	KON		Previous check in hosed my file!!
		 <1>	 5/17/94	KON		first checked in

	To Do:
*/

#ifndef __DitlMgr__
#define __DitlMgr__

#ifndef __DataBase__
#include "DataBase.h"
#endif __DataBase__


#ifndef __Cursor__
#include "Cursor.h"
#endif __Cursor__


//
// types of DITL elements
//

enum
{
	kTextButton,
	kMailItem,
	kAddressItem,
	kTextButtonGraphics,
	kStatText,
	kStatIconGraphics,
	kStatIcon,
	kUserIcon,
	kPersonButton
};

//
// DITL element commands
//

enum
{
	kInitDITLElement,
	kRenderDITLElement,
	kSelectDITLElement,
	kDeselectDITLElement,
	kClickDITLElement,
	kCloseDITLElement
};


//
// DITL cursor codes
//

enum
{
	// magic code to tell us that the select proc will set the new cursor image
	kSelectProcSetsCursor = 255
};


typedef struct
{
	char			pinX;
	char			pinY;
	short			width;
	short			height;
	short			selection;
	unsigned char	theTable[1];
} ControlTable;

typedef Boolean (*doDITLItemProc)(short message, struct DITLItem *theItem, short keyPress);

typedef struct DITLItem
{
	Ptr						objectData;
	short					xPos;
	short					yPos;
	DBID					itemTypeID;
	DBID					itemCursorID;
	struct DITLItemList *	ditlList;
//
// privates follow
//
	doDITLItemProc	ditlProc;
	long			refCon;
} DITLItem, *DITLItemPtr;

typedef struct DITLItemList
{
	Boolean 	isDITLDone;
	CursorPtr	ditlCursor;
	short		count;
	short		cursorXOffset;
	short		cursorYOffset;
	DITLItem	items[1];
} DITLItemList;

#ifndef	unix
void NewDITL( ControlTable *controlTable, DITLItemList *theList ) =
	CallDispatchedFunction( kNewDITL );

Boolean GiveDITLTime( ControlTable *controlTable, DITLItemList *theList, short keyPress ) =
	CallDispatchedFunction( kGiveDITLTime );
	
void DisposeDITL( DITLItemList *theList ) =
	CallDispatchedFunction( kDisposeDITL );
	
unsigned char GetDITLItem( ControlTable *controlTable ) =
	CallDispatchedFunction( kGetDITLItem );
	
void InitDITLMgr( void ) =
	CallDispatchedFunction( kInitDITLMgr );

void ClearDITLDone( DITLItemList *theList ) =
	CallDispatchedFunction( kClearDITLDone );
#endif	/* ! unix */

#endif __DitlMgr__
