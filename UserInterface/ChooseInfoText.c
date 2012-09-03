/*
	File:		ChooseInfoText.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 8/16/94	CMY		Stuff existing info into the edit field.
		<12>	 8/15/94	JBH		Disable deferred dialogs to avoid graphics problems.
		<11>	 8/11/94	JOE		ChangeUserPers.... now wants a flag
		<10>	  8/8/94	KON		Remove ClearBackdrop.
		 <9>	  8/4/94	CMY		Clear the background upon exit.
		 <8>	  8/3/94	JOE		ChangePersonificationPart() became
									ChangeUserPersonificationPart()
		 <7>	  8/3/94	CMY		Gave it a decent UI.
		 <6>	  8/3/94	CMY		Fixed backgrounds.
		 <5>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <4>	 7/24/94	CMY		Updated to latest keyboard interfaces.
		 <3>	 7/22/94	CMY		Fixed reference to nonexistent background graphic.
		 <2>	 7/11/94	CMY		Stuff the results into the info text field, not the taunt field.
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
#include "PlayerDB.h"
#include "Utils.h"
#include "DialogMgr.h"
#include "SegaCluts.h"
#include "Personification.h"
#include "DeferredDialogMgr.h"

/* Entries in the ScreenLayout for the choose Personal Info screen */
#define kPInfoDescriptionRect	0
#define kPInfoJizzleRect		1

#define kPInfoEntryBoxLocation	0

#define	kPInfoHeadlineString	0
#define kPInfoDescriptionString	1


/* Fields for the Info Text entry screen */
enum
{
	kChooseInfoInfoField
};

typedef struct {
	KeyboardEntryRef	keyRef;				// Keyboard entry session reference
	KeyboardEntryLayout	*keyLayout;			// Keyboard entry screen layout data
	long				headlinePatterns;	// Patterns for the headline
	long				descriptionPatterns;// 	"	"	"	"	description
	TextBoxReference	jizzleRef;
	GraphicReference	typingBox;
} ChooseInfoGlobals;


static long ChooseInfoPreflight( long initValue );
static void ChooseInfoReset( long refCon );
static long ChooseInfoSetup( ScreenParams *params );
static long ChooseInfoCommand( long command, long refCon, ScreenParams *params );
static void ChooseInfoKill( long refCon );

static long ChooseInfoDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = ChooseInfoPreflight( params->initValue );
			break;

		case kDoScreenReset:
			ChooseInfoReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = ChooseInfoSetup( params );
			break;

		case kDoScreenCommand:
			result = ChooseInfoCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			ChooseInfoKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in ChooseInfo Dispatch" );
			break;
			
		}
	return result;
}

static void ChooseInfoReset( long refCon )
{
ChooseInfoGlobals	*choose;

	choose = (ChooseInfoGlobals *)refCon;

}

static long ChooseInfoPreflight( long initValue )
{
	return 0;
}

static long ChooseInfoSetup( ScreenParams *params )
{
ChooseInfoGlobals	*choose;
KeyboardEntryLayout *keyLayout;
segaCharRect		tbRect;
Point				boxLocation;
Rect				boxRect;

	DisableDefDialogs();

	choose = (ChooseInfoGlobals *)NewMemory( kTemp, sizeof(ChooseInfoGlobals) );
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );

	GetScreenLayoutPoint( kChooseInfoTextScreen, kPInfoEntryBoxLocation, &boxLocation );
	choose->typingBox = DrawDBGraphicAt( 0, kPersonalInfoBox, boxLocation.h>>3, 
		boxLocation.v>>3, kScrollB );

	SetCurrentDevice( kScrollA );
	SetCurFont( kXBandHeavy );
	SetFontColors( 0, 11, 10, 0 );
	choose->headlinePatterns = DrawScreenLayoutString( kChooseInfoTextScreen, kPInfoHeadlineString );

	SetCurFont( kXBandLight9Font );
	SetFontColors( 0, 9, 8, 0 );
	choose->descriptionPatterns = BoxScreenLayoutString( kChooseInfoTextScreen, kPInfoDescriptionString,
		kPInfoDescriptionRect, kJustLeft );	

	GetScreenLayoutCharRect( kChooseInfoTextScreen, kPInfoJizzleRect, &tbRect );
	choose->jizzleRef = DrawTextBox( &tbRect, kGreenColor, kScrollA );
	StartTextBoxAnimation( choose->jizzleRef, 10 );
	
	keyLayout = SetupKeyboardEntryLayout( kChooseInfoTextKeyboardEntry );
	choose->keyRef = InitKeyboardEntry( keyLayout, 0 );
	choose->keyLayout = keyLayout;
	
	/* Place the existing taunt in the entry field */
	{
		char *existingInfo;
		
		existingInfo = (char *)GetPersonificationPart( GetCurrentLocalUser()->userID, kPersonificationAboutText );
		StuffCurrentKeyboardField( choose->keyRef, existingInfo );
	}

	return (long) choose;
}

static long ChooseInfoCommand( long command, long refCon, ScreenParams *params )
{
Boolean			done;
ChooseInfoGlobals	*choose;
short			result;
short			iii;
short			count;
short			collisionIndex;

	choose = (ChooseInfoGlobals *)refCon;

	result = SendCommandToKeyboard(choose->keyRef, command);
	
	if (result)
	{

		/* If the user chose OK, copy the taunt into the current user's info */
		if (result == kKeyboardDone)
		{
			char *newTaunt = choose->keyLayout->fields[kChooseInfoInfoField].buffer;
			ChangeUserPersonificationPart( GetCurrentLocalUser()->userID, kPersonificationAboutText, newTaunt, false );
		}
		else
		{
			// User cancelled or other errror.
			result = 1;
		}
	}
	return result;
}

static void ChooseInfoKill( long refCon )
{
ChooseInfoGlobals	*choose = (ChooseInfoGlobals *)refCon;

	FreePatternBlock( choose->headlinePatterns );
	FreePatternBlock( choose->descriptionPatterns );
	DisposeTextBoxReference( choose->jizzleRef );
	DisposeGraphicReference( choose->typingBox );
	FinishKeyboardEntry( choose->keyRef);	
	DisposeKeyboardEntryLayout(choose->keyLayout);	
	DisposeMemory((Ptr)choose);
	
	EnableDefDialogs();
}

Ptr GetChooseInfoDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) ChooseInfoDispatch + 2));
	size = (*(long *) ((Ptr) GetChooseInfoDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for choose taunt proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) ChooseInfoDispatch;
#else
		CheckCodeForJTRefs((void *)ChooseInfoDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
