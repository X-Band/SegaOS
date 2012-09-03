/*
	File:		DITLItemSetup.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<28>	 8/17/94	CMY		Removed palette from textButt descriptor, added flags.
		<27>	 8/12/94	JOE		Changes to Setup for PersonButtons for new custom icon stuff
		<26>	  8/3/94	JOE		update PersonButton setup for new personification mgr
		<25>	 7/21/94	ADS		Remove temp use of OSManagersTEMP
		<24>	 7/20/94	CMY		The setup proc for each item type now has access to the 'Laid' x
									& y position, so they may use it if they want.
		<23>	 7/20/94	JOE		StatText's now set up the xPos & yPos in the animationData
									struct for subcell positioning
		<22>	 7/19/94	CMY		TextButts now draw animated buttonGraphics, not 'SEGA's
		<21>	 7/19/94	ADS		Vector Mania
		<21>	 7/19/94	ADS		Vector Mania
		<20>	  7/9/94	CMY		Added support for PersonButton DITL items.
		<19>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<18>	  7/6/94	ADS		ASSERT missing control tables
		<17>	  7/3/94	JOE		added user icon setup
		<16>	 6/30/94	JOE		added static icon item type stuff.
		<15>	 6/29/94	JOE		added static icon graphics item type stuff.
		<14>	 6/28/94	JOE		added static text item type stuff.
		<13>	 6/20/94	KON		Add TextButtGraphics type.
		<12>	 6/19/94	KON		If the bitMapID is reserved, set the bitmap ptr to zero.
		<11>	 6/16/94	CMY		Remove call to AllowUserEvent.
		<10>	 6/16/94	BET		Revert Ted's changes.
		 <9>	 6/15/94	HEC		SegaOSIdle now takes a param
		 <8>	 6/13/94	SAH		Made functions dispatched through the ditl manager.
		 <7>	 6/10/94	KON		Use GetNextCommand instead of ReadControllers.
		 <6>	  6/4/94	KON		Added generic ProcessDITLScreen.
		 <5>	  6/3/94	KON		Add offset X and Y to cursor.
		 <4>	  6/3/94	SAH		Made kDBReservedItemType be the reserved item id insead of zero
									for SetuptextDITL item.
		 <3>	  6/2/94	KON		If DB ID's are zero, don't setup the bitmap or string fields.
		 <2>	 5/31/94	KON		Add support for control tables.
		 <1>	 5/30/94	KON		first checked in

	To Do:
*/

#include "DitlItemSetup.h"
#include "DitlItemSetupPriv.h"
#include "DBTypes.h"
#include "TextButt.h"
#include "StatText.h"
#include "StatIconGraphics.h"
#include "StatIcon.h"
#include "UserIcons.h"
#include "PersonificationButton.h"
#include "Personification.h"
#include "Animators.h"
#include "Errors.h"
#include "Heaps.h"
#include "GraphicsDB.h"
#include "StringDB.h"
#include "MailCntl.h"
#include "Events.h"
#include "SegaIn.h"
#include "Time.h"

//
// non-vectorized because these are all dispatched
// from one place - _SetupDITLObjectData

static Ptr SetupTextButtonData( DBID objectDataRef, short x, short y );
static Ptr SetupMailItemData( DBID objectDataRef, short x, short y );
static Ptr SetupStatTextData( DBID objectDataRef, short x, short y );
static Ptr SetupStatIconGraphicsData( DBID objectDataRef, short x, short y );
static Ptr SetupStatIconData( DBID objectDataRef, short x, short y );
static Ptr SetupUserIconData( DBID objectDataRef, short x, short y );
static Ptr SetupPersonButtonData( DBID objectDataRef, short x, short y );


//
//  local vectorized routines
//
Ptr _GetDITLObjectData( DBID objectDataRef );

Ptr GetDITLObjectData( DBID objectDataRef ) =
		CallDispatchedFunction( kGetDITLObjectData);


//  NOTE - these routines are "owned" by DitlMgr.c


unsigned char _ProcessDITLScreen( DBID ditlRefNum, unsigned char curSelection )
{
ControlTable 	*myControlTable;
DITLItemList 	*theDitlList;
Boolean			done;
short			keyPress, player2;
unsigned char	myResult;

	theDitlList = SetupDITLItemList( ditlRefNum );	
	myControlTable = SetupControlTable( ditlRefNum );

	ASSERT( theDitlList );
	ASSERT( myControlTable );
	
	myControlTable->selection = curSelection;
	
	NewDITL( myControlTable, theDitlList );

	done = false;
	while( !done )
	{
		GetNextCommand( &keyPress, &player2 );
		done = GiveDITLTime( myControlTable, theDitlList, keyPress );
		SegaOSIdle( kNonSpecificCode );
	}

	myResult = GetDITLItem( myControlTable );
		
	DisposeDITL( theDitlList );
	DisposeControlTable( myControlTable );
	DisposeDITLItemList( theDitlList );
	
	return myResult;
}


DITLItemList *_SetupDITLItemList( DBID ditlLayout )
{
ScreenLayout	*myLayout;
DITLItemList 	*theDitlList;
short			iii;

	myLayout = (ScreenLayout *)DBGetItem( kScreenLayout, ditlLayout );

	theDitlList = (DITLItemList *) NewMemory( kTemp, sizeof(DITLItemList) + (myLayout->count-1)*sizeof(DITLItem) );
	theDitlList->count = myLayout->count;
	theDitlList->cursorXOffset = myLayout->cursorXOffset;
	theDitlList->cursorYOffset = myLayout->cursorYOffset;
	
	for( iii = 0; iii < myLayout->count; iii++ )
	{
		theDitlList->items[iii].objectData = SetupDITLObjectData( myLayout->itemDesc[iii].typeID, &myLayout->itemDesc[iii] );
		theDitlList->items[iii].xPos = myLayout->itemDesc[iii].xPos;
		theDitlList->items[iii].yPos = myLayout->itemDesc[iii].yPos;
		theDitlList->items[iii].itemTypeID = myLayout->itemDesc[iii].typeID;
		theDitlList->items[iii].itemCursorID = myLayout->itemDesc[iii].cursorID;
		theDitlList->items[iii].ditlList = theDitlList;	
	}
	return theDitlList;
}

void _DisposeDITLItemList( DITLItemList * theDitlList )
{
short	iii;

	for( iii = 0; iii < theDitlList->count; iii++ )
	{
		DisposeMemory( theDitlList->items[iii].objectData );
	}
	DisposeMemory( theDitlList );
}

ControlTable *_SetupControlTable( DBID controlRef )
{
ControlTable *myControlLayout;
ControlTable *myControlResult;
long		theSize;

	myControlLayout = (ControlTable *)DBGetItem( kControlLayout, controlRef );
	ASSERT(myControlLayout);
	theSize = DBGetItemSize( kControlLayout, controlRef );
	myControlResult = (ControlTable *)NewMemory( kTemp, theSize );
	SegaBlockMove( (Ptr)myControlLayout, (Ptr)myControlResult, theSize );

	return myControlResult;
}

void _DisposeControlTable( ControlTable *myControlTable )
{
	DisposeMemory( myControlTable );
}

Ptr _SetupDITLObjectData( short objectType, ItemDesc *item )
{
Ptr		objectData;

	switch( objectType )
	{
		case kTextButton:
		case kTextButtonGraphics:
			objectData = SetupTextButtonData( item->objectDataRef, item->xPos, item->yPos );
			break;
			
		case kMailItem:
			objectData = SetupMailItemData( item->objectDataRef, item->xPos, item->yPos );
			break;
			
		case kStatText:
			objectData = SetupStatTextData( item->objectDataRef, item->xPos, item->yPos );
			break;
			
		case kStatIconGraphics:
			objectData = SetupStatIconGraphicsData( item->objectDataRef, item->xPos, item->yPos );
			break;
			
		case kUserIcon:
			objectData = SetupUserIconData( item->objectDataRef, item->xPos, item->yPos );
			break;
			
		case kPersonButton:
			objectData = SetupPersonButtonData( item->objectDataRef, item->xPos, item->yPos );
			break;
			
		default:
			ERROR_MESG( "Setup for this item type not implemented yet!" );
			objectData = 0;
			break;
			
	}
	
	return objectData;

}

static Ptr SetupTextButtonData( DBID objectDataRef, short x, short y )
{
TextButtonDesc 	*textDesc;
TextButtonSetup	*textSetup;
	
	textDesc = (TextButtonDesc *)NewMemory( kTemp, sizeof(TextButtonDesc) );
	textSetup = (TextButtonSetup *)GetDITLObjectData( objectDataRef );
	textDesc->animationData.frameDelay = textSetup->animationData.frameDelay;
//
// if the ID is kDBReservedItemType ( 255), someone else is setting this thing up
// I use this as it is neither a valid database entry or a pointer
//
	if( textSetup->animationData.bitMapID != kDBReservedItemType )
		textDesc->animationData.bitmap = GetDBButtonFrame( textSetup->animationData.bitMapID, kAnimationFrames );
	else
		textDesc->animationData.bitmap = 0;
		
	if ( textSetup->animationData.xPos )
		textDesc->animationData.xPos = textSetup->animationData.xPos;
	else
		textDesc->animationData.xPos = x;
		
	if ( textSetup->animationData.yPos )
		textDesc->animationData.yPos = textSetup->animationData.yPos;
	else
		textDesc->animationData.yPos = y;
		
	if( textSetup->myCStringID != kDBReservedItemType )
		textDesc->myCString = GetSegaString( textSetup->myCStringID );
		
	textDesc->textXCenter = textSetup->textXCenter;
	textDesc->textYTop = textSetup->textYTop;
	textDesc->color0 = textSetup