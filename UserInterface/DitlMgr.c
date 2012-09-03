/*
	File:		DitlMgr.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<32>	 8/26/94	KON		Accept diagonal controller presses when navigating screen
									elements.
		<31>	  8/8/94	JOE		kButtonMoveSnd & kButtonSelectSnd were reversed
		<30>	 7/21/94	ADS		Remove temp use of OSManagersTEMP
		<29>	 7/20/94	JOE		use new sound FX constants
		<28>	 7/19/94	ADS		Vector Mania II
		<27>	 7/19/94	ADS		Vector Mania
		<26>	 7/10/94	JOE		play FX indirectly
		<25>	  7/9/94	CMY		Added support for PersonButton DITL items.
		<24>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<23>	  7/3/94	JOE		added usericon setup
		<22>	 6/30/94	SAH		Removed DB purge crap.
		<21>	 6/30/94	JOE		added StatIcon item type setup stuff.
		<20>	 6/29/94	JOE		added StatIconGraphics item type setup stuff.
		<19>	 6/28/94	JOE		added StatText item type setup stuff.
		<18>	 6/20/94	SAH		#ifdef SIMULATOR inside InitDITLMgr to cut out some unused code.
		<17>	 6/20/94	KON		Added TextButtonGraphics type.
		<16>	 6/16/94	BET		Revert Ted's changes.
		<15>	 6/15/94	HEC		SegaOSIdle takes a param now
		<14>	 6/13/94	SAH		Add the DITLItemList functions.
		<13>	 6/10/94	KON		Add ClearDITLDone
		<12>	  6/5/94	SAH		(Joe) Updated sounds.
		<11>	  6/4/94	SAH		(Joe) Integrated sound.
		<10>	  6/3/94	KON		Add cursor X and Y offset.
		 <9>	  6/1/94	KON		Add address item control proc.
		 <8>	 5/31/94	SAH		Made GiveDITLTime call SegaOSIdle until I figure out Psyq
									suckiness.
		 <7>	 5/31/94	KON		Add pinning to the control tables.
		 <6>	 5/28/94	SAH		Updated to new cursor interface so we canhave one cursor per
									ditl.
		 <5>	 5/28/94	KON		Fix temp memory leak bug. These DITL controls should be
									installed into the ROM db!
		 <4>	 5/28/94	KON		Pass controller reads into DITL proc. Make it use the
									dispatcher.
		 <3>	 5/27/94	KON		Made cursors appear on pixel, not character, boundaries.
		 <2>	 5/27/94	KON		Added a mail control item.
		 <7>	 5/23/94	SAH		Create the cursor before the DITL items.
		 <6>	 5/20/94	SAH		Remembered to dispose of the cursor when we kill the dialog.
		 <5>	 5/20/94	SAH		Added support for cursors.
		 <4>	 5/18/94	KON		Move text button stuff into it's own file. Make the control
									procs have one entry point and load them from the database.
		 <3>	 5/18/94	HEC		long pattern types
		 <2>	 5/17/94	KON		Added DisposeDITL and Text buttons.
		 <1>	 5/17/94	KON		first checked in. Need to add support for cursors.

	To Do:
*/


#include "DitlMgr.h"
#include "SegaIn.h"
#include "SegaBM.h"
#include "VDP.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "DataBase.h"
#include "TextButt.h"
#include "TextButtGraphics.h"
#include "MailCntl.h"
#include "StatText.h"
#include "StatIconGraphics.h"
#include "StatIcon.h"
#include "UserIcons.h"
#include "AddressCntl.h"
#include "DBTypes.h"
#include "Errors.h"
#include "OSManagers.h"
#include "Heaps.h"
#include "Time.h"
#include "SegaSound.h"
#include "DitlItemSetupPriv.h"
#include "PersonificationButton.h"

//
// Prototypes - dispatched functions
//

static void _NewDITL( ControlTable *controlTable, DITLItemList *theList );
static Boolean _GiveDITLTime( ControlTable *controlTable, DITLItemList *theList, short keyPress );
static void _DisposeDITL( DITLItemList *theList );
static unsigned char _GetDITLItem( ControlTable *controlTable );
static void _InitDITLMgr( void );
static void _ClearDITLDone( DITLItemList *theList );



long
_DITLControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
				
			/* install our selectors */
			SetDispatchedFunction ( kNewDITL,				kDITLManager,	_NewDITL );
			SetDispatchedFunction ( kGiveDITLTime,			kDITLManager,	_GiveDITLTime );
			SetDispatchedFunction ( kDisposeDITL,			kDITLManager,	_DisposeDITL );
			SetDispatchedFunction ( kGetDITLItem,			kDITLManager,	_GetDITLItem );
			SetDispatchedFunction ( kInitDITLMgr,			kDITLManager,	_InitDITLMgr );
			SetDispatchedFunction ( kClearDITLDone,			kDITLManager,	_ClearDITLDone );
			SetDispatchedFunction ( kProcessDITLScreen,		kDITLManager,	_ProcessDITLScreen );
			SetDispatchedFunction ( kSetupDITLItemList,		kDITLManager,	_SetupDITLItemList );
			SetDispatchedFunction ( kSetupDITLObjectData,	kDITLManager,	_SetupDITLObjectData );
			SetDispatchedFunction ( kDisposeDITLItemList,	kDITLManager,	_DisposeDITLItemList );
			SetDispatchedFunction ( kSetupControlTable,		kDITLManager,	_SetupControlTable );
			SetDispatchedFunction ( kDisposeControlTable,	kDITLManager,	_DisposeControlTable );

			SetDispatchedFunction ( kGetDITLObjectData,		kDITLManager,	_GetDITLObjectData );
			break;
		
		case kSoftInialize:
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}

static void _ClearDITLDone( DITLItemList *theList )
{
	theList->isDITLDone = false;
}

void _NewDITL( ControlTable *controlTable, DITLItemList *theList )
{
short	iii;
unsigned char	curSelectedItem;
short			numItems;

//
// ==== Get the itemTypeID proc out of the database and call the init proc on the DITLItem
//

	theList->isDITLDone = false;
	numItems = theList->count;
	curSelectedItem = GetDITLItem( controlTable );

//
// set the cursor up. set it's image if the select proc won't be doing it
//
	theList->ditlCursor = CreateSegaCursor();
	MoveSegaCursor( theList->ditlCursor, theList->items[curSelectedItem].xPos + theList->cursorXOffset,
		theList->items[curSelectedItem].yPos + theList->cursorYOffset );
		
		
	if ( theList->items[curSelectedItem].itemCursorID != kSelectProcSetsCursor )
		{
		SetSegaCursorImage ( theList->ditlCursor, theList->items[curSelectedItem].itemCursorID );
		}
	
	for( iii = 0; iii < numItems; iii++ )
	{
		theList->items[iii].ditlList = theList;
		
		theList->items[iii].ditlProc = (doDITLItemProc) DBGetItem( kDITLProcType, theList->items[iii].itemTypeID );
		ASSERT(	theList->items[iii].ditlProc );
//
// Call the init and render procs for this item
//	
		(theList->items[iii].ditlProc)( kInitDITLElement, &theList->items[iii], 0 );
		(theList->items[iii].ditlProc)( kRenderDITLElement, &theList->items[iii], 0 );
	}
	
//
// Call the select proc for the currently selected item
//
	(theList->items[curSelectedItem].ditlProc)( kSelectDITLElement, &theList->items[curSelectedItem], 0 );
	
	VDPIdle();
}

void _DisposeDITL( DITLItemList *theList )
{
short iii;

	DisposeSegaCursor ( theList->ditlCursor );
	
	for( iii = 0; iii < theList->count; iii++ )
	{
		(theList->items[iii].ditlProc)( kCloseDITLElement, &theList->items[iii], 0 );
	}
}

unsigned char _GetDITLItem( ControlTable *controlTable )
{
	return controlTable->theTable[controlTable->selection];
}

Boolean _GiveDITLTime( ControlTable *controlTable, DITLItemList *theList, short keyPress )
{
unsigned char	curSelectedItem;
unsigned char	newSelection;
short			heightCount;		
short			widthCount;		
short			tableSize;
short			selection;

	if( theList->isDITLDone )
		return true;
	
	curSelectedItem = GetDITLItem( controlTable );
	newSelection = curSelectedItem;

	if( keyPress & kButtonPressed )
	{
		theList->isDITLDone = (theList->items[curSelectedItem].ditlProc)( kClickDITLElement, &theList->items[curSelectedItem], keyPress );
		PlayDBFX ( kButtonSelectSnd, 0, 0 );
	}
	else
	{
//
// if a direction was pushed, move to next unique control table entry
//
		tableSize = controlTable->width * controlTable->height;
		heightCount = controlTable->height-1;
		widthCount = controlTable->width-1;
		
		if( keyPress & kUP )
		{	
			while( (curSelectedItem == newSelection) && heightCount-- )
			{
				controlTable->selection -= controlTable->width;

				if( controlTable->selection < 0 )
				{
					if( controlTable->pinY )
						controlTable->selection += controlTable->width;
					else
						controlTable->selection += tableSize;
				}

				newSelection = GetDITLItem( controlTable );
			}
		}

		if( keyPress & kDOWN )
		{
			while( (curSelectedItem == newSelection) && heightCount-- )
			{
				controlTable->selection += controlTable->width;
				if( controlTable->selection >= tableSize )
				{
					if( controlTable->pinY )
						controlTable->selection -= controlTable->width;
					else
						controlTable->selection -= tableSize;
				}
				
				newSelection = GetDITLItem( controlTable );
			}
		}

		if( keyPress & kLEFT )
		{
			while( (curSelectedItem == newSelection) && widthCount-- )
			{
			//
			// if we're on the left edge and pinX is false, bump to the right side
			//
				if( (controlTable->selection % controlTable->width) == 0 )
				{
					if( !controlTable->pinX )
						controlTable->selection += (controlTable->width-1);
				}
				else
					controlTable->selection -= 1;

				newSelection = GetDITLItem( controlTable );
			}
		}

		if( keyPress & kRIGHT )
		{
			while( (curSelectedItem == newSelection) && widthCount-- )
			{
			//
			// if we're on the right edge and pinX is false, bump to the left side
			//
				if( ((controlTable->selection + 1) % (controlTable->width)) == 0 )
				{
					if( !controlTable->pinX )
						controlTable->selection -= (controlTable->width-1);
				}
				else
					controlTable->selection += 1;

				newSelection = GetDITLItem( controlTable );
			}
		}
	}

	if( newSelection != curSelectedItem )
	{
		PlayDBFX ( kButtonMoveSnd, 0, 0 );
		(theList->items[curSelectedItem].ditlProc)( kDeselectDITLElement, &theList->items[curSelectedItem], 0 );
		
//
// do we need to install a new cursor?
//
		if ( theList->items[curSelectedItem].itemCursorID != theList->items[newSelection].itemCursorID )
			{
			/* yes, we do, but only if the select proc won't be doing it */
			if ( theList->items[newSelection].itemCursorID != kSelectProcSetsCursor )
				{
				SetSegaCursorImage ( theList->ditlCursor, theList->items[newSelection].itemCursorID );
				}
			}
			
		MoveSegaCursor( theList->ditlCursor, theList->items[newSelection].xPos + theList->cursorXOffset,
			 theList->items[newSelection].yPos + theList->cursorYOffset );
	
		(theList->items[newSelection].ditlProc)( kSelectDITLElement, &theList->items[newSelection], 0 );
	}
	
	VDPIdle();
	SegaOSIdle( kNonSpecificCode );

	if( theList->isDITLDone )
		return true;
	else
		return false;
}

void _InitDITLMgr( void )
{
#ifdef SIMULATOR
doDITLItemProc		dITLElementPtr;
//
// Add all the default DITL items to the database
//

	dITLElementPtr = GetTextButtonPtr();
	if( DBAddItem( kDITLProcType, kTextButton, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding text button in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );
		
	dITLElementPtr = GetMailItemPtr();
	if( DBAddItem( kDITLProcType, kMailItem, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding mail item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetAddressItemPtr();
	if( DBAddItem( kDITLProcType, kAddressItem, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding address item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetTextButtonGraphicsPtr();
	if( DBAddItem( kDITLProcType, kTextButtonGraphics, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding text button graphics in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetStatTextItemPtr();
	if( DBAddItem( kDITLProcType, kStatText, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding static text item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetStatIconGraphicsItemPtr();
	if( DBAddItem( kDITLProcType, kStatIconGraphics, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding static icon graphics item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetStatIconItemPtr();
	if( DBAddItem( kDITLProcType, kStatIcon, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding static icon item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetUserIconItemPtr();
	if( DBAddItem( kDITLProcType, kUserIcon, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding user icon item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );

	dITLElementPtr = GetPersonButtonPtr();
	if( DBAddItem( kDITLProcType, kPersonButton, (Ptr)dITLElementPtr ) )
	{
		ERROR_MESG( "Error adding person button item in InitDITLMgr" );
	}
	DisposeMemory( dITLElementPtr );
#endif	
}







