/*
	File:		MailView.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<81>	 8/28/94	JBH		No longer sort on whether a mail item has been read.
		<80>	 8/22/94	CMY		Disabled the Collect and Write buttons.
		<79>	 8/16/94	JOE		Play the system "select" sound when a selection is made
		<78>	 8/15/94	JBH		Disable deferred dialogs to avoid graphics problems
		<77>	 8/10/94	JBH		Use a real CLUT to do the text color cycling, so we don't get
									screwed by graphics expecting to get clut 0.
		<76>	 8/10/94	JBH		Remove CLUT cycle VBL task before restoring CLUT 0, so we don't
									leave it in a bad way.
		<75>	  8/9/94	CMY		Fixed the backdrops to use KON's aux backdrop shit. Make it
									kwiker and less flikkery.
		<74>	  8/9/94	CMY		Make the "Collect" button work when user has no mail.
		<73>	  8/9/94	KON		After hiding the background, redraw line2.
		<72>	  8/8/94	JOE		don't let vbl proc dangle
		<71>	  8/8/94	CMY		Moved the envelope as per Joey's spec.
		<70>	  8/8/94	KON		Also move the background graphic when it's temporarily hidden
									when a dialog comes up.
		<69>	  8/8/94	KON		Remove ClearBackdrop.
		<68>	  8/4/94	CMY		Moved the background grafix around a bit.
		<67>	  8/4/94	JBH		Made mail view sort use more fields of mail if dates are equal
									(has been read, sender name, title).
		<66>	  8/3/94	CMY		Added a line and revectored the DITL buttons.
		<65>	  8/3/94	CMY		Fixed backgrounds. Changed buttons. Changed jizzle rect.
		<64>	 7/30/94	CMY		Kill the VBL clut cycling when preflighting other screens so
									dialogs work right and backgrounds can be redrawn correctly.
		<63>	 7/30/94	JOE		Color cycle via VBL
		<62>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<61>	 7/26/94	KON		Add check to mail controls when there's no mail in the in-box.
		<60>	 7/26/94	CMY		Don't do VBL syncing when animating the clut, cuz it's too slow.
		<59>	 7/22/94	KON		UI poop.
		<58>	 7/22/94	CMY		Changed colors for color cycling selection
		<57>	 7/22/94	KON		Add color cycling and remove lame cursor.
		<56>	 7/22/94	CMY		Fix the jizzler rect
		<55>	 7/21/94	KON		Fix pattern mem leaks.
		<54>	 7/21/94	KON		Jizzle text even if there are no entries.
		<53>	 7/20/94	KON		Joey turn.
		<52>	 7/19/94	KON		Update for jizzlin borders.
		<51>	 7/19/94	KON		Add titles.
		<50>	 7/18/94	KON		Bumped text rect in by 8 dots.
		<49>	 7/17/94	KON		Make font colors blue for unread, and red for read.
		<48>	 7/17/94	KON		Update for new font stuff.
		<47>	 7/16/94	KON		Update to use new XBandBold font.
		<46>	 7/14/94	KON		If box is net registered, don't allow user to update mail.
		<45>	 7/12/94	CMY		Made "New" work on the no mail version
		<44>	 7/12/94	KON		Add delete verification.
		<43>	 7/12/94	CMY		Set up the userID properly for the "Check Mail" button.
		<42>	 7/12/94	CMY		Added check mail button.
		<41>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<40>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<39>	  7/5/94	CMY		Added two-button mail screen for when user has no mail.
		<38>	  7/4/94	KON		Quicksort compare function now takes a short rather than a
									Boolean.
		<37>	  7/4/94	ADS		New interface for screen control
		<36>	  7/3/94	KON		Add mail sorting.
		<35>	  7/3/94	KON		Update delete to work with sorted mail.
		<34>	  7/2/94	CMY		We *DO* know how to handle new, Kon.
		<33>	  7/2/94	KON		Add sorting of mail entries.
		<32>	  7/1/94	KON		Remove most of the CES specific stuff...Still need to figure out
									NEW.
		<31>	 6/20/94	KON		Update for new CES look.
		<30>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		<29>	 6/13/94	KON		Make selection parameter to screens a long and store mailTarget
									position info so it can be restored across screens.
		<28>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		<27>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		<26>	 6/11/94	KON		Pass the current piece of mail to the entry screen so it can be
									found and acted on (such as delete).
		<25>	 6/11/94	KON		Make Delete Mail work.
		<24>	 6/10/94	KON		Updated to support new screen dispatcher calls for preflight and
									reset.
		<23>	 6/10/94	KON		Update to use new screen dispatcher.
		<22>	  6/5/94	KON		Text box adjustments. Removed ability to reply to mail since we
									don't have enough room in SRAM for the keyboard.
		<21>	  6/4/94	KON		Check for mail overflow.
		<20>	  6/4/94	KON		Add kFourWayMail screen.
		<19>	  6/4/94	KON		Update to use new graphics DB ID's.
		<18>	  6/3/94	DJ		updating to revised DrawDBGraphic call
		<17>	  6/3/94	KON		Added new background and changed text layout to match.
		<16>	  6/2/94	CMY		Made the "New" button jump into the mail editing code.
		<15>	  6/2/94	KON		Change fonts and font colors around for Joey.
		<14>	  6/2/94	KON		Added granite background, monkeyed around with fonts and pattern
									memory.
		<13>	  6/1/94	KON		Removed some unused stack variables.
		<12>	 5/31/94	SAH		Killed SegaMisc.h. Fix the colors more for the sega. Make the
									init code a little smaller.
		<11>	 5/31/94	SAH		Adjusted the mail text body colors so it shows up.
		<10>	 5/31/94	KON		Update more of the stuff to read info from database. Also added
									control tables to the DB.
		 <9>	 5/30/94	KON		Use the database to setup ditl items via DITLItemSetup. Only
									text buttons implemented so far.
		 <8>	 5/30/94	SAH		Include Time.h
		 <7>	 5/29/94	KON		Uses graphics DB, clear the VDP before drawing, added the dual
									DITL stuff.
		 <6>	 5/28/94	KON		Can now delete mail. Date is a long instead of a string.
		 <5>	 5/28/94	KON		First cut at displaying a piece of mail...
		 <4>	 5/28/94	KON		Pass controller reads into DITL proc.
		 <3>	 5/27/94	KON		Use a mail cursor.
		 <2>	 5/27/94	KON		Made DITL item positions pixel based instead of character based
									so the cursor appears.
		 <1>	 5/27/94	KON		first checked in

	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "DITLMgr.h"
#include "SegaScrn.h"
#include "DITLItemSetup.h"
#include "SegaText.h"
#include "StringDB.h"
#include "vdp.h"
#include "Heaps.h"
#include "Errors.h"
#include "SegaVDP.h"
#include "Mail.h"
#include "SegaBM.h"
#include "MailCntl.h"
#include "SegaIn.h"
#include "Events.h"
#include "TextUtls.h"
#include "TextButt.h"
#include "GraphicsDB.h"
#include "Time.h"
#include "MailEdit.h"
#include "MungeCode.h"
#include "Screens.h"
#include "Backdrops.h"
#include "DialogMgr.h"
#include "QuickSort.h"
#include "Challnge.h"
#include "NetRegister.h"
#include "VBLManager.h"
#include "MailView.h"
#include "MailViewPriv.h"
#include "DeferredDialogMgr.h"
#include "SegaSound.h"

static long DisplayMailDispatch( short action, long command, long *refCon, ScreenParams *params );
static long DisplayMailPreflight( long initValue );
static void DisplayMailReset( long refCon );
static long DisplayMailSetup( ScreenParams *params );
static long DisplayMailCommand( long command, long refCon, ScreenParams *params );
static void DisplayMailKill( long refCon );

static long MailViewVBL( MailViewRefCon *mailRef );

static short QSortCompareMail( void *sortArray, short index1, short index2 );
static void QSortSwapEntries( void *sortArray, short index1, short index2 );

static long DisplayMailDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = DisplayMailPreflight( params->initValue );
			break;

		case kDoScreenReset:
			DisplayMailReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = DisplayMailSetup( params );
			break;

		case kDoScreenCommand:
			result = DisplayMailCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			DisplayMailKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Display Mail Dispatch" );
			break;
			
		}
	return result;
}

static void DisplayMailReset( long refCon )
{
MailViewRefCon	*mailRef;

	mailRef = (MailViewRefCon *)refCon;

	/* start the CLUT animating again */
	if ( mailRef->colorCycleClut )
	{
		mailRef->vblRef = AddVBLRequest( (VBLRequestProc)MailViewVBL, (long)mailRef, kMailViewClutCycleRate );
	}
	else
	{
		mailRef->vblRef = (VBLProcRef) 0L;
	}

	ClearDITLDone( mailRef->textButtDitlList );
	ShowBackdrop( true );
}

static long DisplayMailPreflight( long initValue )
{
	return 0;
}

#define PACK_SEGA_COLOR(r,g,b) ( ((r & 7)<<1) | ((g & 7)<<5) | ((b & 7)<<9) )
static long DisplayMailSetup( ScreenParams *params )
{
DITLItem		mailItem[kMaxInBoxEntries];
DITLItem 		*mItem;
MailItemDesc	*mailDesc;
MailItemDesc 	*mDesc;
DITLItemList 	*mailItemDitlList;
ControlTable 	*mailItemControlTable;
DITLItemList 	*textButtDitlList;
ControlTable 	*textButtControlTable;
Boolean			done;
short			ypos;
short			count;
short			iii;
unsigned char	curTextButtonSelection;
segaCharRect 	myRect;
MailViewRefCon	*mailRef;
long	patternStart;
short	xpos;
Mail				*theMail;
char			*theCString;

	mailRef = (MailViewRefCon *)NewMemory( kTemp, sizeof(MailViewRefCon) );
	mailDesc = (MailItemDesc * )NewMemory( kTemp, sizeof(MailItemDesc) * kMaxInBoxEntries );
	mailRef->disposeMailDesc = mailDesc;
	
	EraseGDevice ( kScrollA );
	
	SetBackdropIDAndAuxBackgroundGraphic( kBlackBackground, true, 0, kEnvelopeBGGraphic, 5, 3, 0 );
	mailRef->line2 = DrawDBGraphicAt( 0, kGrayHorizontalLine, 5, 19, kScrollB );

	ShowDefDialogs( kDDMainEventLoop );		/* flush 'em out, in case user stays in mail a while */

	DisableDefDialogs();

	// Set up the color cycling stuff
	mailRef->colorCycleFrame = 0;
	for (iii=0; iii<5; ++iii)
	{
		mailRef->colorCycleColors[iii] = PACK_SEGA_COLOR( 7-iii, 7-iii, 7 );	
		mailRef->colorCycleColors[iii+5] = PACK_SEGA_COLOR( iii+3, iii+3, 7 );
	}

	/* This requests the CLUT, but doesn't kick off the VBL */
	/* We need this because we need the CLUT number for LinearizeScreenArea */
	{
		short clut[16];
	
		GetClut( 0, clut );
		clut[kMailCntlAnimatingColor] = 
			mailRef->colorCycleColors[mailRef->colorCycleFrame];
		clut[kMailCntlAnimatingColor+1] = 
			(mailRef->colorCycleColors[mailRef->colorCycleFrame]>>1) & 0xEEE;
			
		mailRef->colorCycleClut = RequestUniqueClut( clut );
	}

	myRect.left = 3;
	myRect.top = 2;
	myRect.right = 36;
	myRect.bottom = 25;
	
	SetCurrentDevice( kScrollA );
	mailRef->myTextBoxMems = DrawTextBox( &myRect, 8, kScrollA ); 
	StartTextBoxAnimation( mailRef->myTextBoxMems, 10 );

	myRect.left = 5;
	myRect.top = 3;
	myRect.right = 35;
	myRect.bottom = 19;

	patternStart = LinearizeScreenArea( &myRect, mailRef->colorCycleClut );
	SetupTextGDevice( &myRect, patternStart );
	mailRef->textPatterns = patternStart;
	mailRef->textRect = myRect;

//
// Draw the From, Title, and Date Sent strings
//
	SetFontColors( 0, 11, 10, 0 );
	SetCurFont( kXBandBold10Font );

	theCString = GetSegaString( kFrom );
	xpos = 0;
	ypos = 2;
	DrawSegaString( &xpos, &ypos, theCString );

	theCString = GetSegaString( kTitle );
	xpos = 121;
	DrawSegaString( &xpos, &ypos, theCString );

#if 0
	theCString = GetSegaString( kDate );
	xpos = 192;
	DrawSegaString( &xpos, &ypos, theCString );
#endif

	count = CountInBoxEntries();
	if( count == 0 )
	{
		// Set up the DITL for the buttons at the bottom
		textButtDitlList = SetupDITLItemList( kEmptyMailLayout );	
		textButtControlTable = SetupControlTable( kEmptyMailLayout );

		NewDITL( textButtControlTable, textButtDitlList );
		
		// There's no DITL for the mail items
		mailRef->mailItemDitlList = nil;
		mailRef->mailItemControlTable = nil;
		mailRef->textButtDitlList = textButtDitlList;
		mailRef->textButtControlTable = textButtControlTable;
		mailRef->sortedMail = nil;		
	}
	else
	{
		mailRef->sortedMail = (MailViewSortType *)NewMemory( kTemp, sizeof(MailViewSortType) * kMaxInBoxEntries );
	
		textButtDitlList = SetupDITLItemList( kMailLayout );	
		textButtControlTable = SetupControlTable( kMailLayout );
					
		//
		// Sort the mail
		//
		for( iii = 0; iii < count; iii++ )
		{
			theMail = GetIndexInBoxMail( iii );
			mailRef->sortedMail[iii].entry = iii;
			mailRef->sortedMail[iii].theMail = theMail;
		}
		
		QSort( count, mailRef->sortedMail, QSortCompareMail, &QSortSwapEntries );
		
		//
		// Walk the In Box creating a mail item control for each entry
		//
	
		mailItemDitlList = (DITLItemList *) NewMemory( kTemp,  sizeof(DITLItemList) + count*sizeof(DITLItem) );
	
		ypos = 19;
	
		mDesc = &mailDesc[0];
		mItem = &mailItem[0];
		for( iii = 0; iii < count; iii++ )
		{
			mDesc->theMail = GetIndexInBoxMail( mailRef->sortedMail[iii].entry );
			mDesc->color1 = 11;	//dk blue
			mDesc->color2 = 10;	//lt blue
			mDesc->color0 = 0;	
			mDesc->color3 = 0;
			mDesc->palette = 1;
			mDesc->font = kXBandLight9Font;
			mDesc++;
	
			mItem->objectData = (Ptr)&mailDesc[iii];
			mItem->refCon = 0;
			// x position is ignored for mail items
			mItem->xPos = 2;
			mItem->yPos = ypos;
			mItem->itemTypeID = kMailItem;
			mItem->itemCursorID = -1;
			mItem++;
			ypos += 11;
			
			mailItemDitlList->items[iii] = mailItem[iii];
	
		}
		mailItemDitlList->count = count;
		mailItemDitlList->cursorXOffset = (myRect.left - 1) * 8 + 6;
		mailItemDitlList->cursorYOffset = (myRect.top - 1) * 8 - 1;
	
		//
		// Simple linear control table for mail items
		//
		// When we add controls for the x direction, this will be more complicated!!!
		//
		// Maybe we need function based control procs....
		//
		mailItemControlTable = (ControlTable*)NewMemory( kTemp,  sizeof(ControlTable) + count*sizeof(unsigned char) );
	
		ASSERT( mailItemControlTable );
		mailItemControlTable->pinX = 1;
		mailItemControlTable->pinY = 1;
		mailItemControlTable->width = 1;
		mailItemControlTable->height = count;
		mailItemControlTable->selection = params->selection;
		if( mailItemControlTable->selection >= count )
			mailItemControlTable->selection = 0;
		
		for( iii = 0; iii < count; iii++ )
		{
			mailItemControlTable->theTable[iii] = iii;
		}
	
		//
		// Install a DITL to handle the buttons across the bottom:
		// Read, Delete, New, Exit
		//
	
		NewDITL( mailItemControlTable, mailItemDitlList );
		NewDITL( textButtControlTable, textButtDitlList );
		
		mailRef->mailItemDitlList = mailItemDitlList;
		mailRef->mailItemControlTable = mailItemControlTable;
		mailRef->textButtDitlList = textButtDitlList;
		mailRef->textButtControlTable = textButtControlTable;
	}

	/* This kicks off the color cycle VBL */
	if ( mailRef->colorCycleClut )
	{
		mailRef->vblRef = AddVBLRequest( (VBLRequestProc)MailViewVBL, (long)mailRef, kMailViewClutCycleRate );
	}
	else
	{
		mailRef->vblRef = (VBLProcRef) 0L;
	}

	return (long)mailRef;
}	
	
static long DisplayMailCommand( long command, long refCon, ScreenParams *params )
{
MailViewRefCon	*mailRef;
Boolean			done;
long			result;
short			mailTarget;
short			mailItemInList;
short			count;
short			dialogResult;

	mailRef = (MailViewRefCon *)refCon;

	result = 0;		//not done, keep looping
	params->initValue = 0;
	
	if ( mailRef->line2 == nil )
	{
		mailRef->line2 = DrawDBGraphicAt( 0, kGrayHorizontalLine, 5, 19, kScrollB );
	}


	done = false;

	SetupTextGDevice( &mailRef->textRect, mailRef->textPatterns );

	if( (command & kRIGHT) || (command & kLEFT) )
		done = GiveDITLTime( mailRef->textButtControlTable, mailRef->textButtDitlList, command );
	else	
	if( (command & kUP) || (command & kDOWN) )
	{
		if (mailRef->mailItemControlTable)
			done = GiveDITLTime( mailRef->mailItemControlTable, mailRef->mailItemDitlList, command );
	}
	else
	if( command & kButtonPressed ) 
		done = true;
	
	if (mailRef->mailItemControlTable)	
		params->selection = mailItemInList = GetDITLItem( mailRef->mailItemControlTable );
	else
		params->selection = mailItemInList = 0;

	if( done )
	{
		PlayDBFX( kButtonSelectSnd, 0, 0 );
	
		if (mailRef->vblRef )
		{
			RemoveVBLRequest( mailRef->vblRef );			// nuke the clut cycler
			mailRef->vblRef = 0;							// remember that we nuked it
		}
		
		HideBackdrop( true );
				
	//
	// Should perform the action specified by textButtControlTable on mail item from mailItemControlTable
	//
	
		result = GetDITLItem( mailRef->textButtControlTable );

		if (mailRef->mailItemControlTable)	//do these if there is mail
		{
			mailTarget = mailRef->sortedM