/*
	File:		AddressCntl.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<17>	  8/9/94	CMY		Don't draw invalid users in white. It's ugly and confusing.
		<16>	  8/9/94	CMY		Moved Graphics Around for Joey.
		<15>	  8/8/94	CMY		Filled 'er full of lead, ya varmint! (Added bullets)
		<14>	 7/22/94	KON		UI poop.
		<13>	 7/22/94	KON		Set non-verified entries to grey.
		<12>	 7/22/94	KON		Make clut cycling savvy.
		<11>	 7/21/94	KON		Update for new date formats.
		<10>	 7/19/94	KON		Update to support new backgrounds.
		 <9>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		 <8>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <7>	  7/7/94	KON		Make sure w/l draws on main address book screen.
		 <6>	  7/6/94	KON		Remove special case handling of CATAPULT.
		 <5>	  7/5/94	CMY		Fixed it for debugging in Think C.
		 <4>	  7/1/94	KON		Don't show W/L or date for CATAPULT.
		 <3>	  6/5/94	KON		Fix stats for DW.
		 <2>	  6/2/94	KON		Added second level address view. Still needs an artistic
									touch....
		 <1>	  6/1/94	KON		first checked in
	To Do:
*/

/* if we're building the ROM database, do a real sega build */
#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "AddressCntl.h"
#include "Errors.h"
#include "SegaText.h"
#include "VDP.h"
#include "SegaIn.h"
#include "Heaps.h"
#include "Animators.h"
#include "MungeCode.h"
#include "PlayerDB.h"
#include "AddressBook.h"
#include "GraphicsDB.h"
#include "MailView.h"
#include "Sprites.h"
#include "StringDB.h"

static void RenderDITLAddressItem( DITLItem *theItem );
static void SelectDITLAddressItem( DITLItem *theItem );
static void DeselectDITLAddressItem( DITLItem *theItem );
static Boolean ClickDITLAddressItem( DITLItem *theItem, short controllerRead );

static Boolean DoDITLAddressItem( short message, DITLItem *theItem, short keyPress )
{
Boolean	done;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			break;
			
		case kRenderDITLElement:
			RenderDITLAddressItem( theItem );		
			break;
				
		case kSelectDITLElement:
			SelectDITLAddressItem( theItem );
			break;
			
		case kDeselectDITLElement:
			DeselectDITLAddressItem( theItem );
			break;
			
		case kClickDITLElement:
			done = ClickDITLAddressItem( theItem, keyPress );
			break;
			
		case kCloseDITLElement:
			if ( theItem->refCon )
				DisposeSprite( (SpriteRef) theItem->refCon );
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLAddressItem" );
			
	}
		
	return done;
}

void RenderDITLAddressItem( DITLItem *theItem )
{
AddressItemDesc	*theAddressItem;
short			locXpos;
short			locYpos;
PlayerInfo		*thePlayerInfo;
short			oldFont;
char			scratch[30];
char			scratch2[10];
long			littleString;
//
// Draw the mail item
//
	theAddressItem = (AddressItemDesc *)theItem->objectData;
	thePlayerInfo = theAddressItem->theAddress;
		
//
// Draw the address item that's in the refCon
//
		
	oldFont = GetCurFont();
	SetCurFont ( ((AddressItemDesc *) theItem->objectData)->font );
	
	SetFontColors( theAddressItem->color0, theAddressItem->color1, theAddressItem->color2, theAddressItem->color3 );

	locXpos = 3;
	locYpos = theItem->yPos;
		
	DrawClippedSegaText( locXpos, locYpos, 110, thePlayerInfo->userId.userName );

	locXpos = 121;
	SegaNumToDate( thePlayerInfo->dateLastPlayed, scratch, kShortDateFormat );
	DrawClippedSegaText( locXpos, locYpos, kDateLastPlayedLength, scratch );

	locXpos = 179;

	SegaNumToString( thePlayerInfo->wins, scratch );
	littleString = 0x57202D00;	//'W -'0,0
	SegaAppendText( scratch, (char *) &littleString, scratch );
	littleString = 0x20000000;	//' '0,0,0
	SegaAppendText( scratch, (char *) &littleString, scratch );
	SegaNumToString( thePlayerInfo->losses, scratch2 );
	SegaAppendText( scratch, (char *) &scratch2, scratch );
	littleString = 0x4C000000;	//'L'0,0,0
	SegaAppendText( scratch, (char *) &littleString, scratch );
	//
	// display win/loss stats
	//
	DrawClippedSegaText( locXpos, locYpos, kAddressStatsLength, scratch );
	
	if ( (thePlayerInfo->userId.box.box == -1) && !theItem->refCon)
	{
		SpriteRef bullet;
		
		bullet = CreateSprite( GetDBGraphics( kBullet ) );
		MoveSprite( bullet, kMailRectLeft - 7 + 128, theItem->yPos + kMailRectTop + 1 + 128 );
		DrawSprite( bullet );
		theItem->refCon = (long)bullet;
	}

	VDPIdle();

	SetCurFont ( oldFont );
}

//
// Blue is the magic animated color for this screen...when a new item is selected, draw it in
// blue (11, 10) and it will animate....
//

void SelectDITLAddressItem( DITLItem *theItem )
{
AddressItemDesc	*theAddressItem;

	theAddressItem = (AddressItemDesc *)theItem->objectData;
 	theAddressItem->color0 = 0;
 	theAddressItem->color1 = kAddressCntlAnimatingColor+1;
 	theAddressItem->color2 = kAddressCntlAnimatingColor;
 	theAddressItem->color3 = 0;

	RenderDITLAddressItem( theItem );
}

void DeselectDITLAddressItem( DITLItem *theItem )
{
AddressItemDesc	*theAddressItem;
PlayerInfo		*thePlayerInfo;

	theAddressItem = (AddressItemDesc *)theItem->objectData;
 	thePlayerInfo = theAddressItem->theAddress;

	theAddressItem->color0 = 0;
 	theAddressItem->color1 = 9;
 	theAddressItem->color2 = 8;
 	theAddressItem->color3 = 0;
	
	RenderDITLAddressItem( theItem );
}

Boolean ClickDITLAddressItem( DITLItem *theItem, short controllerRead )
{

// signal done if any button was pressed
	if( controllerRead & kButtonPressed )
		return true;	//signal we are done
	else
		return false;
}

doDITLItemProc GetAddressItemPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLAddressItem + 2));

	size = (*(long *) ((Ptr) GetAddressItemPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Address proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) DoDITLAddressItem;
#else
		CheckCodeForJTRefs((void *) GetAddressItemPtr, size);
		SegaBlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return (doDITLItemProc)data;
}
