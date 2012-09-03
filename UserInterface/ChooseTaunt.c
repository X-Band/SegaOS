/*
	File:		ChooseTaunt.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/16/94	CMY		Measure the text to see if it fits into the Versus screen box.
		<14>	 8/15/94	JBH		Disable deferred dialogs to avoid graphics problems.
		<13>	 8/13/94	CMY		Show existing taunt when editing taunt.
		<12>	 8/11/94	JOE		ChangeUserPersonificationPart now wants a flag
		<11>	  8/8/94	KON		Remove ClearBackdrop.
		<10>	  8/4/94	CMY		Fix the jizzler.
		 <9>	  8/4/94	CMY		Clear the background upon exit.
		 <8>	  8/3/94	JOE		ChangePersonificationPart() became
									ChangeUserPersonificationPart()
		 <7>	  8/3/94	CMY		Fixed backgrounds
		 <6>	 7/30/94	CMY		Fixed a pattern mems leak.
		 <5>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <4>	 7/29/94	CMY		New and improved UI poo.
		 <3>	 7/24/94	CMY		Updated to latest keyboard interfaces.
		 <2>	 7/22/94	CMY		Fixed reference to nonexistent background graphic.
		 <1>	 7/11/94	CMY		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "vdp.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "Errors.h"
#include "GraphicsDB.h"
#include "StringDB.h"
#include "TextUtls.h"
#include "Mail.h"
#include "SegaIn.h"
#include "SegaOS.h"
#include "Screens.h"
#include "Time.h"
#include "Events.h"
#include "Heaps.h"
#include "Backdrops.h"
#include "Keyboard.h"
#include "MungeCode.h"
#include "DITLMgr.h"
#include "GTSendData.h"
#include "PlayerDB.h"
#include "Utils.h"
#include "DialogMgr.h"
#include "Personification.h"
#include "SegaCluts.h"
#include "DeferredDialogMgr.h"
#include "VersusScreenPriv.h"

/* Fields for the New Choose Taunt entry screen */
enum
{
	kChooseTauntTauntField
};


/* Entries in the ScreenLayout for the taunt screen */
#define kTauntDescriptionRect	0
#define kTauntJizzleRect		1

#define kTauntEntryBoxLocation	0

#define	kTauntHeadlineString	0
#define kTauntDescriptionString	1


typedef struct {
	KeyboardEntryRef	keyRef;			// Keyboard entry session reference
	KeyboardEntryLayout	*keyLayout;		// Keyboard entry screen layout data
	TextBoxReference	tbRef;			// Jizzlin' text
	long				titlePatterns;	// pat mems for the title
	long				descriptionPatterns;	// pat mems for the desc
	GraphicReference	typingBox;
} ChooseTauntGlobals;


static long ChooseTauntPreflight( long initValue );
static void ChooseTauntReset( long refCon );
static long ChooseTauntSetup( ScreenParams *params );
static long ChooseTauntCommand( long command, long refCon, ScreenParams *params );
static void ChooseTauntKill( long refCon );

static long ChooseTauntDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = ChooseTauntPreflight( params->initValue );
			break;

		case kDoScreenReset:
			ChooseTauntReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = ChooseTauntSetup( params );
			break;

		case kDoScreenCommand:
			result = ChooseTauntCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			ChooseTauntKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in ChooseTaunt Dispatch" );
			break;
			
		}
	return result;
}

static void ChooseTauntReset( long refCon )
{
ChooseTauntGlobals	*choose;

	choose = (ChooseTauntGlobals *)refCon;

}

static long ChooseTauntPreflight( long initValue )
{
	return 0;
}

static Boolean TauntMeasureProc( char *taunt )
{
	Rect boxRect;
	short length = GetStringLength( taunt );
	char savedChar = taunt[length+1];
	Boolean result = true;
	
	boxRect.top = 0;
	boxRect.left = 0;
	boxRect.bottom = kTauntPatHeight * 8;
	boxRect.right = (kTauntPatHalfWidth * 2 * 8);
	
	taunt[length] = 'W';
	taunt[length+1] = 0;
	
	if ( SegaBoxText(&boxRect, taunt, kJustCenter, kDontDraw) < length )
		result = false;
	
	taunt[length] = 0;
	taunt[length+1] = savedChar;
	
	return result;
}

static long ChooseTauntSetup( ScreenParams *params )
{
ChooseTauntGlobals	*choose;
KeyboardEntryLayout *keyLayout;
segaCharRect		tbRect;
Point				boxLocation;
Rect				boxRect;

	DisableDefDialogs();

	choose = (ChooseTauntGlobals *)NewMemory( kTemp, sizeof(ChooseTauntGlobals) );
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );

	GetScreenLayoutPoint( kChooseTauntScreen, kTauntEntryBoxLocation, &boxLocation );
	choose->typingBox = DrawDBGraphicAt( 0, kTauntEntryField, boxLocation.h>>3, 
		boxLocation.v>>3, kScrollB );

	SetCurrentDevice( kScrollA );
	SetCurFont( kXBandHeavy );
	SetFontColors( 0, 11, 10, 0 );
	choose->titlePatterns = DrawScreenLayoutString( kChooseTauntScreen, kTauntHeadlineString );

	SetCurFont( kXBandLight9Font );
	SetFontColors( 0, 9, 8, 0 );
	choose->descriptionPatterns = BoxScreenLayoutString( kChooseTauntScreen, kTauntDescriptionString,
		kTauntDescriptionRect, kJustLeft );	

	GetScreenLayoutCharRect( kChooseTauntScreen, kTauntJizzleRect, &tbRect );
	choose->tbRef = DrawTextBox( &tbRect, kGreenColor, kScrollA );
	StartTextBoxAnimation( choose->tbRef, 10 );
	
	keyLayout = SetupKeyboardEntryLayout( kChooseTauntKeyboardEntry );
	choose->keyRef = InitKeyboardEntry( keyLayout, 0 );
	choose->keyLayout = keyLayout;
	SetKeyboardEntryMeasureProc( choose->keyRef, TauntMeasureProc );
	
	/* Place the existing taunt in the entry field */
	{
		char *existingTaunt;
		
		existingTaunt = (char *)GetPersonificationPart( GetCurrentLocalUser()->userID, kPersonificationTauntText );
		StuffCurrentKeyboardField( choose->keyRef, existingTaunt );
	}

	return (long) choose;
}


static long ChooseTauntCommand( long command, long refCon, ScreenParams *params )
{
Boolean			done;
ChooseTauntGlobals	*choose;
short			result;
short			iii;
short			count;
short			collisionIndex;

	choose = (ChooseTauntGlobals *)refCon;

	result = SendCommandToKeyboard(choose->keyRef, command);
	
	if (result)
	{

		/* If the user chose OK, copy the taunt into the current user's info */
		if (result == kKeyboardDone)
		{
			char *newTaunt = choose->keyLayout->fields[kChooseTauntTauntField].buffer;
			ChangeUserPersonificationPart( GetCurrentLocalUser()->userID, kPersonificationTauntText, newTaunt, false );
		}
		else
		{
			// User cancelled or other errror.
			result = 1;
		}
	}
	return result;
}

static void ChooseTauntKill( long refCon )
{
ChooseTauntGlobals	*choose = (ChooseTauntGlobals *)refCon;

	FinishKeyboardEntry( choose->keyRef);	
	DisposeKeyboardEntryLayout(choose->keyLayout);
	StopTextBoxAnimation( choose->tbRef );
	DisposeTextBoxReference( choose->tbRef );
	FreePatternBlock( choose->descriptionPatterns );
	FreePatternBlock( choose->titlePatterns );
	DisposeGraphicReference( choose->typingBox );	
	DisposeMemory((Ptr)choose);
	
	EnableDefDialogs();
}

Ptr GetChooseTauntDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) ChooseTauntDispatch + 2));
	size = (*(long *) ((Ptr) GetChooseTauntDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for choose taunt proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) ChooseTauntDispatch;
#else
		CheckCodeForJTRefs((void *)ChooseTauntDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
