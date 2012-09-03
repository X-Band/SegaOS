/*
	File:		NewPlayer.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 8/27/94	CMY		Make the "Hit any button..." text blut.
		<11>	 8/21/94	JOE		Use a purtier backdrop for the "You can set your taunt..."
									dialog
		<10>	 8/20/94	JOE		Invalidate the New Player's Default Info & Taunt for the
									server's benefit.
		 <9>	 8/20/94	KON		DrawGraphic now takes a clutID.
		 <8>	 8/18/94	ADS		Hide gBoxID globals
		 <7>	 8/16/94	JOE		the evil kNewPlayer strikes again!
		 <6>	 8/16/94	KON		New player icon is no longer ID 0.
		 <5>	 8/13/94	CMY		Signal the player ID screens that they're being called from a
									first-time setup.
		 <4>	 8/12/94	JOE		new icon stuff
		 <3>	 8/11/94	KON		Show confirmation dialog when entering new users after the first
									time. Show nice screen the first time ever. Tell em about the
									options screen when they're done.
		 <2>	  8/4/94	CMY		Redid the whole damn thing.
		 <1>	  8/3/94	CMY		first checked in

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
#include "DBTypes.h"
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
#include "BoxSer.h"
#include "Personification.h"

#define kNewPlayerJizzleRect		0
#define kNewPlayerDescriptionRect	1

#define	kNewPlayerTitleString		0
#define kNewPlayerDescriptionString	1
#define kNewPlayerBeginString		2

typedef struct {
	TextBoxReference	jizzleReference;
	long				titlePatterns;
	long				descriptionPatterns;
	long				beginPatterns;
	GraphicReference	iconGraphic;
} NewPlayerGlobals;


static long NewPlayerPreflight( long initValue );
static void NewPlayerReset( long refCon );
static long NewPlayerSetup( ScreenParams *params );
static long NewPlayerCommand( long command, long refCon, ScreenParams *params );
static void NewPlayerKill( long refCon );

static long NewPlayerDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = NewPlayerPreflight( params->initValue );
			break;

		case kDoScreenReset:
			NewPlayerReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = NewPlayerSetup( params );
			break;

		case kDoScreenCommand:
			result = NewPlayerCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			NewPlayerKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in NewPlayer Dispatch" );
			break;
			
		}
	return result;
}

static void NewPlayerReset( long refCon )
{
NewPlayerGlobals	*new;

	new = (NewPlayerGlobals *)refCon;

}

static long NewPlayerPreflight( long initValue )
{
short				dialogResult;

	return 0;
}

static long NewPlayerSetup( ScreenParams *params )
{
NewPlayerGlobals	*new;
short				index;
DBID				*idWalker;
segaCharRect		textGDRect;
segaCharRect		jizzleRect;
Rect				textRect;
char				*description;
SegaBitMapPtr		theIcon;
short	numInactiveUsers = 0;
short	iii;

	/* This screen calls each of the user ID screens in order. We put up instructions
	 * before we call the first user ID screen, and then just behave transparently.
	 * See if this is the first time we're being called ever.
	 */

	for( iii = 3; iii >= 0; iii-- )
	{
		if ( GetLocalUserROMIconID( iii ) == kNewPlayerIcon )		/* initially, all ROM icons are the unknown player */
			numInactiveUsers++;
	}

	if ( (numInactiveUsers == 4) && (params->selection == 0) )
	{
		new = (NewPlayerGlobals *)NewMemory( kTemp, sizeof(NewPlayerGlobals) );
			
		EraseGDevice( kScrollA );
		SetBackdropID( kBlackBackground, true, 0 );
		SetCurrentDevice( kScrollA );

		GetScreenLayoutCharRect( kNewPlayerScreen, kNewPlayerJizzleRect, &jizzleRect );
		new->jizzleReference = DrawTextBox( &jizzleRect, kGreenColor, kScrollA );
		StartTextBoxAnimation( new->jizzleReference, 10 );
	
		SetCurFont( kXBandHeavy );
		SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
		new->titlePatterns = DrawScreenLayoutString( kNewPlayerScreen, kNewPlayerTitleString );
		
		theIcon = GetIconBitMap( kNewPlayerIcon );		/* draw the unknown player ROM icon */
		new->iconGraphic = DrawGraphic( theIcon, 17, 7, 0, 0, 0 );
		
		SetCurFont( kXBandLight9Font );
		SetFontColors( 0, kDarkGreenColor, kGreenColor, 0 );
		new->descriptionPatterns = BoxScreenLayoutString( kNewPlayerScreen, kNewPlayerDescriptionString,
			kNewPlayerDescriptionRect, kJustLeft );	
		
		SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
		new->beginPatterns = DrawScreenLayoutString( kNewPlayerScreen, kNewPlayerBeginString );
		
		params->selection = 1;	// Start at the first player ID screen (Handle)
		
		VDPIdle();
	}
	else
	{
		params->selection++;	// Bump to the next player ID screen
		
		if (params->selection > 2)
		{
			/* Here, the taunt and info will be the defaults.
			   Let the server know by marking them invalid for this New Player.
			 */
			InvalidateUserPersonificationParts( GetCurUserID(), kPersonificationTauntText );
			InvalidateUserPersonificationParts( GetCurUserID(), kPersonificationAboutText );

			params->selection = 6;
			EraseGDevice( kScrollA );
			SetBackdropID( kBackdrop, true, 0 );
			SetCurrentDevice( kScrollA );
			DoDialog( kTellThemAboutOptionsMenu );
		}
		
		new = nil;
	}
	return (long) new;
}

static long NewPlayerCommand( long command, long refCon, ScreenParams *params )
{
Boolean				done;
NewPlayerGlobals	*new;
short				result;
short				iii;
short				count;
short				collisionIndex;

	new = (NewPlayerGlobals *)refCon;
	result = 0;
	
	params->initValue = 1;	/* This tells the playerID screens that they're being called from here */
	
	if ( new )
	{
		/* This is the first time. Leave the description text up until the player
		 * hits a button and then go to the first user ID screen
		 */
		if (command & (kStart | kButtonA | kButtonB | kButtonC) )
			result = params->selection;
	}
	else
	{
		result = params->selection;		// Go to the selected user ID screen
	}
	return result;
}

static void NewPlayerKill( long refCon )
{
NewPlayerGlobals	*new = (NewPlayerGlobals *)refCon;

	if ( new )
	{
		FreePatternBlock( new->titlePatterns );
		FreePatternBlock( new->descriptionPatterns );
		FreePatternBlock( new->beginPatterns );
		DisposeTextBoxReference( new->jizzleReference );
		DisposeGraphicReference( new->iconGraphic );
		DisposeMemory((Ptr)new);
	}
}

Ptr GetNewPlayerDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) NewPlayerDispatch + 2));
	size = (*(long *) ((Ptr) GetNewPlayerDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for new player screen proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) NewPlayerDispatch;
#else
		CheckCodeForJTRefs((void *)NewPlayerDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
