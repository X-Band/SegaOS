/*
	File:		AddressBook.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<65>	 8/24/94	CMY		Blow away the aux background graphic when bringing up dialogs.
		<64>	 8/22/94	CMY		Reordered the button id's so they draw nicer.
		<63>	 8/16/94	JOE		Play the system "select" sound when a selection is made
		<62>	 8/15/94	JBH		Disable deferred dialogs here to avoid graphics problems. Tell
									backdrops we're exiting so we don't later use the aux graphic by
									accident.
		<61>	 8/10/94	JBH		Use a unique clut to color animate the selected text. Also use
									Kon's AuxGraphic stuff to draw the two people graphic, so they
									come back if a dialog blows away the background.
		<60>	 8/10/94	JBH		Remove the VBL task in the proper order, so we don't cycle after
									restoring CLUT 0.
		<59>	  8/9/94	CMY		Don't change the colors of the entries when not selected.
		<58>	  8/8/94	KON		Don't set graphics device to scrollB when we don't need to.
		<57>	  8/8/94	KON		Remove ClearBackdrop.
		<56>	  8/6/94	CMY		Fixed a problem where the wrong text gdevice was being set up
									for selecting entries. Poo-be-gone!
		<55>	  8/6/94	JOE		Don't let VBL routine dangle
		<54>	  8/4/94	CMY		Moved the background graphic. Attempted to dispose background
									during dialogs to prevent screen-blackout. But it's a bit
									funked.
		<53>	  8/3/94	CMY		Revectored the DITL buttons
		<52>	  8/3/94	CMY		Changed backgrounds, button layout, jizzle, etc.
		<51>	 7/30/94	CMY		Kill the VBL clut cycling when preflighting other screens so
									dialogs work right and backgrounds can be redrawn correctly.
		<50>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<49>	 7/29/94	JOE		Use VBI (er, L) task to cycle colors
		<48>	 7/27/94	KON		Change jizzle color to blue.
		<47>	 7/26/94	CMY		Dispose your jizzlers, darn it!
		<46>	 7/22/94	KON		UI poop.
		<45>	 7/22/94	CMY		Speed up color cycling and change colors to be whiter.
		<44>	 7/22/94	KON		Set un-verified entries to grey.
		<43>	 7/22/94	KON		Part 2 of clut cycling.
		<42>	 7/22/94	CMY		Fix jizzle rect bounds. Add clut-cycling for selected entry.
		<40>	 7/21/94	KON		Fix pattern mem leaks.
		<39>	 7/21/94	KON		Jizzle text even if there are no entries.
		<38>	 7/20/94	KON		Joey turn.
		<37>	 7/19/94	KON		Very little.
		<36>	 7/19/94	KON		Update for jizzlin borders.
		<35>	 7/19/94	KON		Support new backgrounds.
		<34>	 7/12/94	KON		Add confirmation on delete dialog.
		<33>	 7/11/94	KON		Allow connection to a specified opponent.
		<32>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<31>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<30>	  7/6/94	KON		Remove special case handling of CATAPULT.
		<29>	  7/5/94	CMY		Two-button address book screen when the user has no addresses.
		<28>	  7/5/94	KON		Only net register after successful server connect. Check
									Register before connect.
		<27>	  7/4/94	KON		Add sorting.
		<26>	  7/4/94	ADS		New interface for screen control
		<25>	  7/2/94	CMY		Remove CES dialog, because NEW works.
		<24>	  7/1/94	KON		Make Connect to CATAPULT the same as FindOpponent.
		<23>	  7/1/94	KON		Remove CES specific stuff.
		<22>	 6/20/94	KON		Put up a dialog if there are no address book entries for CES.
		<21>	 6/20/94	KON		Update for CES look.
		<20>	 6/17/94	KON		Return a reasonable result when coming out of NetRegister
									connect.
		<19>	 6/17/94	KON		Added better screen reset support.
		<18>	 6/17/94	KON		Add support for NetRegister Manager.
		<17>	 6/17/94	KON		More address book delete and ditl stuff.
		<16>	 6/16/94	KON		Fix the delete key.
		<15>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		<14>	 6/13/94	KON		Make selection parameter to screens a long.
		<13>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		<12>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		<11>	 6/10/94	KON		Add screen reset and preflight functions.
		<10>	 6/10/94	KON		Update to use screen dispatcher.
		 <9>	  6/5/94	KON		Fix stats for DW.
		 <8>	  6/5/94	KON		Reposition stuff for new background screen.
		 <7>	  6/5/94	KON		Added fourth button control.
		 <6>	  6/4/94	KON		Update to use new graphics DB ID's.
		 <5>	  6/3/94	DJ		updating to revised DrawDBGraphic call
		 <4>	  6/3/94	KON		Add cursor X and Y offset.
		 <3>	  6/2/94	KON		Change fonts and colors for Joey.
		 <2>	  6/2/94	KON		Added second level address book. Still needs a major artistic
									overhaul.
		 <1>	  6/1/94	KON		first checked in

	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "DitlMgr.h"
#include "AddressCntl.h"
#include "PlayerDB.h"
#include "SegaScrn.h"
#include "DITLItemSetup.h"
#include "SegaText.h"
#include "StringDB.h"
#include "VDP.h"
#include "Heaps.h"
#include "Errors.h"
#include "SegaIn.h"
#include "Time.h"
#include "MailView.h"
#include "TextUtls.h"
#include "GraphicsDB.h"
#include "VBLManager.h"
#include "AddressBook.h"
#include "AddressBookPriv.h"
#include "Screens.h"
#include "MungeCode.h"
#include "Backdrops.h"
#include "BoxSer.h"
#include "DialogMgr.h"
#include "NetRegister.h"
#include "QuickSort.h"
#include "Challnge.h"
#include "SegaCluts.h"
#include "SegaVDP.h"
#include "DeferredDialogMgr.h"
#include "SegaSound.h"

static long AddressBookDispatch( short action, long command, long *refCon, ScreenParams *params );
static long AddressBookPreflight( long initValue );
static void AddressBookReset( long refCon );
static long AddressBookSetup( ScreenParams *params );
static long AddressBookCommand( long command, long refCon, ScreenParams *params );
static void AddressBookKill( long refCon );

static long addrBookvbl( AddressBookRefCon *addressGlobals );

static short QSortCompareAddresses( void *sortArray, short index1, short index2 );
static void QSortSwapAddresses( void *sortArray, short index1, short index2 );

static long AddressBookDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = AddressBookPreflight( params->initValue );
			break;

		case kDoScreenReset:
			AddressBookReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = AddressBookSetup( params );
			break;

		case kDoScreenCommand:
			result = AddressBookCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			AddressBookKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Address Book Dispatch" );
			break;
			
		}
	return result;
}

static long AddressBookPreflight( long initValue )
{
	return 0;
}

static void AddressBookReset( long refCon )
{
AddressBookRefCon	*addressGlobals;

	addressGlobals = (AddressBookRefCon	*)refCon;
	
	/* Fire up that VBL task again */
	if ( addressGlobals->colorCycleClut )
	{
		addressGlobals->vblRef = AddVBLRequest( (VBLRequestProc)addrBookvbl, (long)addressGlobals, kAddrBookClutCycleRate );
	}
	else
	{
		addressGlobals->vblRef = (VBLProcRef) 0L;
	}

	ClearDITLDone( addressGlobals->addressItemDitlList );
	ClearDITLDone( addressGlobals->textButtDitlList );
	ShowBackdrop( true );
}

#define PACK_SEGA_COLOR(r,g,b) ( ((r & 7)<<1) | ((g & 7)<<5) | ((b & 7)<<9) )
static long AddressBookSetup( ScreenParams *params )
{
DITLItem		addressItem[kMaxAddressBookEntries];
DITLItem *		aItem;
AddressItemDesc	*addressDesc;
AddressItemDesc *aDesc;
DITLItemList 	*addressItemDitlList;
ControlTable 	*addressItemControlTable;
ControlTable 	*textButtControlTable;
DITLItemList 	*textButtDitlList;
AddressBookRefCon	*addressGlobals;
segaCharRect 	myRect;
short			ypos;
short			xpos;
short			count;
short			iii;
long			patternStart;
PlayerInfo		*theAddress;
char			*theCString;

	DisableDefDialogs();

	addressGlobals = (AddressBookRefCon	*) NewMemory( kTemp, sizeof(AddressBookRefCon) );
	addressDesc = (AddressItemDesc*) NewMemory( kTemp, sizeof(AddressItemDesc) * kMaxAddressBookEntries );
	addressGlobals->disposeAddressDesc = addressDesc;
	
	EraseGDevice ( kScrollA );
	SetBackdropIDAndAuxBackgroundGraphic( kBlackBackground,