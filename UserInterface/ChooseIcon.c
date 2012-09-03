/*
	File:		ChooseIcon.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<32>	 8/27/94	CMY		Make the "Hit any button..." text blue.
		<31>	 8/24/94	CMY		Slow down the blinking a bit and don't wipe out the jizzle rect.
		<30>	 8/23/94	CMY		Blink the player icon after selection so it's more obvious that
									something happened.
		<29>	 8/21/94	JOE		Add HueAcceleration
		<28>	 8/21/94	JOE		Huedoo
		<27>	 8/20/94	JOE		PlayerRAMIconExists now returns kDBReservedItemType for no RAM
									icon present
		<26>	 8/20/94	JOE		Change instructions if the user has a custom icon, make icon
									choose list just the ROM icons (Now default to RAM icon if
									present, fall back to ROM)
		<25>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<24>	 8/18/94	ADS		Hide gBoxID globals
		<23>	 8/16/94	JOE		Play Icon flip sounds as you choose yo' shit
		<22>	 8/16/94	CMY		Don't let user choose reserved icons.
		<21>	 8/12/94	JOE		update for new SegaIcons calls
		<20>	 8/11/94	JOE		ChangeUserPersonificationPart now wants a flag
		<19>	  8/9/94	CMY		Makes the initial selection your current player icon.
		<18>	  8/9/94	CMY		Plugged up a pattern mems leak.
		<17>	  8/9/94	CMY		Moved stuff around for Joey.
		<16>	  8/7/94	CMY		Use ScreenLayout DB entry instead of in-line constants. Added a
									string.
		<15>	  8/5/94	JOE		make the user's choice the ROMID, not the RAMID.
		<14>	  8/4/94	CMY		Don't let user choose icon #0.
		<13>	  8/3/94	JOE		make icon choice stick
		<12>	  8/3/94	CMY		Don't draw the icon until we've figured out which ID's are
									available.
		<11>	  8/3/94	CMY		Made icons draw less flickery.
		<10>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <9>	 7/29/94	CMY		Fixed page left/right button positions.
		 <8>	 7/28/94	KON		Fix text rect to keep strings from getting nasty clippping.
		 <7>	 7/26/94	CMY		Dispose jizzlers in addition to killing the animation.
		 <6>	 7/26/94	CMY		Mad the buttons show up the first time.
		 <5>	 7/25/94	KON		Relabled right and left arrow buttons to share with news.
		 <4>	 7/25/94	CMY		Added left/right buttons and fix some poo.
		 <3>	 7/25/94	CMY		Fancy new high-falutin' UI.
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
#include "SegaIcons.h"
#include "BoxSer.h"
#include "SegaSound.h"
#include "SegaCluts.h"
#include "Personification.h"
#include "PersonificationButton.h"

#define kLayoutJizzleRect			0
#define kLayoutDescriptionRect		1

#define kLayoutPlayerLocation		0
#define kLayoutLeftPageLocation		1
#define kLayoutRightPageLocation	2

#define kLayoutTitleString			0
#define kLayoutDescriptionString	1
#define kLayoutFinishedString		2
#define	kLayoutCustomIconString		3

#define	kHueAccelThreshold			30

typedef struct {
	short				iconIndex;
	long				titlePatterns;
	long				descriptionPatterns;
	long				finishedPatterns;
	TextBoxReference	myTextBoxMems;
	GraphicReference	iconRef;
	short				iconCount;
	Point				playerIconPoint;
	Point				leftButtonPoint;
	Point				rightButtonPoint;
	GraphicReference	leftButtonRef;
	GraphicReference	rightButtonRef;
	unsigned long		lastTicks;
	Boolean				leftButtonState;
	Boolean				rightButtonState;
	unsigned char		hueAccel;
	unsigned char		customHue;
	DBID				iconIDs[1];
} ChooseIconGlobals;


static long ChooseIconPreflight( long initValue );
static void ChooseIconReset( long refCon );
static long ChooseIconSetup( ScreenParams *params );
static long ChooseIconCommand( long command, long refCon, ScreenParams *params );
static void ChooseIconKill( long refCon );

static long ChooseIconDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = ChooseIconPreflight( params->initValue );
			break;

		case kDoScreenReset:
			ChooseIconReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = ChooseIconSetup( params );
			break;

		case kDoScreenCommand:
			result = ChooseIconCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			ChooseIconKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in ChooseIcon Dispatch" );
			break;
			
		}
	return result;
}

static void ChooseIconReset( long refCon )
{
ChooseIconGlobals	*choose;

	choose = (ChooseIconGlobals *)refCon;

}

static long ChooseIconPreflight( long initValue )
{
	return 0;
}

static void UpdateButtonImages( ChooseIconGlobals *choose, short command )
{
Boolean left = (command & kLEFT) != 0;
Boolean right = (command & kRIGHT) != 0;
GraphicReference ref;
DBID	myGraphicsID;
	
	if ( left ^ choose->leftButtonState )
	{
		choose->leftButtonState = left;
		
		myGraphicsID = kPageLeftEnableControl;
		if (left)
			myGraphicsID = kPageLeftHiliteControl;
		
		ref = DrawDBGraphicAt( 0, myGraphicsID, (choose->leftButtonPoint.h+4)>>3, 
			choose->leftButtonPoint.v>>3, kScrollA );
		
		if ( choose->leftButtonRef )
			DisposeGraphicReference( choose->leftButtonRef );
			
		choose->leftButtonRef = ref;
	}
	
	if ( right ^ choose->rightButtonState )
	{
		choose->rightButtonState = right;
		
		myGraphicsID = kPageRightEnableControl;
		if (right)
			myGraphicsID = kPageRightHiliteControl;
	
		ref = DrawDBGraphicAt( 0, myGraphicsID, (choose->rightButtonPoint.h+4)>>3, 
			choose->rightButtonPoint.v>>3, kScrollA );
		
		if ( choose->rightButtonRef )
			DisposeGraphicReference( choose->rightButtonRef );
			
		choose->rightButtonRef = ref;
	}
}


/* 
	The user can only choose among the ROM icons
 */
 
static void UpdateIcon( ChooseIconGlobals *choose )
{
	SegaBitMapPtr 		theIcon;
	RAMIconBitMapPtr	ramIcon;
	GraphicReference 	old = choose->iconRef;
	unsigned char		userID;
		
	userID = GetCurUserID();
		
	ChangeUserPersonificationPart( userID, kPersonificationROMIconID, 
											&(choose->iconIDs[choose->iconIndex]), false );

	ChangeUserPersonificationPart( userID, kPersonificationROMClutID,
											&(choose->customHue), false );
	
	theIcon = (SegaBitMapPtr)DBGetItem( kIconBitMapType, choose->iconIDs[choose->iconIndex] );
	
	SetCurrentDevice( kScrollB );
	choose->iconRef = DrawGraphic( theIcon, (choose->playerIconPoint.h+4)>>3, 
												choose->playerIconPoint.v>>3, 0, 0, choose->customHue);

	if (old)
		DisposeGraphicReference(old);

	VDPIdle();
}


static long ChooseIconSetup( ScreenParams *params )
{
ChooseIconGlobals	*choose;
DBID				*idWalker;
segaCharRect		jizzleRect;
DBID				currentROMIconID;
DBID				id;

	/* this actually gets room for DBCountItems() + 1 (which is what we want) */
	choose = (ChooseIconGlobals *)NewMemory( kTemp, sizeof(ChooseIconGlobals) +
		DBCountItems( kIconBitMapType )*sizeof(DBID) );
	
	choose->leftButtonState = 1;
	choose->rightButtonState = 1;
	choose->leftButtonRef = nil;
	choose->rightButtonRef = nil;
	
	choose->lastTicks = 0;
	choose->hueAccel = 0;
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kType2PlainBackground, true, 0 );
	SetCurrentDevice( kScrollA );

	SetCurFont( kXBandHeavy );
	SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
	choose->titlePatterns = DrawScreenLayoutString( kChooseIconScreen, kLayoutTitleString );
	
	SetCurFont( kXBandLight9Font );		/* if user has a custom icon, we change the instructions */
	SetFontColors( 0, kDarkGreenColor, kGreenColor, 0 );
	if ( PlayerRAMIconExists( GetCurrentLocalUser() ) != kDBReservedItemType )
		choose->descriptionPatterns = BoxScreenLayoutString( kChooseIconScreen,
			kLayoutCustomIconString, kLayoutDescriptionRect, kJustLeft);
	else
		choose->descriptionPatterns = BoxScreenLayoutString( kChooseIconScreen,
			kLayoutDescriptionString, kLayoutDescriptionRect, kJustLeft);
	
	SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
	choose->finishedPatterns = DrawScreenLayoutString( kChooseIconScreen, kLayoutFinishedString );
	
	GetScreenLayoutCharRect( kChooseIconScreen, kLayoutJizzleRect, &jizzleRect );
	choose->myTextBoxMems = DrawTextBox( &jizzleRect, 8, kScrollA );
	StartTextBoxAnimation( choose->myTextBoxMems, 5 );
	
	GetScreenLayoutPoint( kChooseIconScreen, kLayoutPlayerLocation, &choose->playerIconPoint );
	GetScreenLayoutPoint( kChooseIconScreen, kLayoutLeftPageLocation, &choose->leftButtonPoint );
	GetScreenLayoutPoint( kChooseIconScreen, kLayoutRightPageLocation, &choose->rightButtonPoint );
	
	choose->iconIndex = 0;
	choose->iconRef = nil;
	choose->iconCount = 0;
	idWalker = choose->iconIDs;
	
	UpdateButtonImages( choose, 0 );
	
	id = GetCurUserID();
	
	choose->customHue = (unsigned char)GetPersonificationPart( id, kPersonificationROMClutID );

	currentROMIconID = (DBID)GetPersonificationPart( id, kPersonificationROMIconID );		/* which one are they using? */

	id = DBGetFirstItemID( kIconBitMapType );
	while ( id != kDBReservedItemType )
	{
		/* 
		 * Don't let them choose a reserved icon (such as the "New Player" icon)
		 * Only add it to the list if it's not reserved.
		 */
		if ((long)id < (long)kFirstReservedIcon)
		{
			if ( currentROMIconID == id )
				choose->iconIndex = choose->iconCount;	/* they're using a ROM icon */
			*idWalker++ = id;
			choose->iconCount++;
		}
		id = DBGetNextItemID(kIconBitMapType, id);		
	}

	UpdateIcon( choose );
	VDPIdle();

	return (long) choose;
}

static void BlinkPlayerIcon( ChooseIconGlobals	*choose )
{
	char *handle = (char *)GetPersonificationPart( GetCurUserID(), kPersonificationHandle );
	short handleWidth;
	short cellWidth;
	short xCenter, xLeft;
	short iconX, iconY;
	short height;
	segaCharRect textRect;
	SpriteRef handleSprite;
	short index;
	SegaBitMap *theIcon = (SegaBitMapPtr)DBGetItem( kIconBitMapType, choose->iconIDs[choose->iconIndex] );

	SetCurFont( kMainFont );
	iconX = (choose->playerIconPoint.h+4) & ~7;
	iconY = choose->playerIconPoint.v;
	
	handleWidth = MeasureSegaText( handle );
	xCenter = (kPersonificationIconWidth >> 1) + iconX;
	xLeft = xCenter - (handleWidth >> 1);
			
	cellWidth = (handleWidth+7) >> 3;
	cellWidth = (cellWidth+3) & ~3;			// Due to our fuqdup sprite allocator

	/* Create the text sprite */
	textRect.left = (xLeft>>3);
	textRect.right = textRect.left + cellWidth;
	textRect.top = (iconY + kPersonificationIconHeight + 2) >> 3;
	height = GetCurFontHeight();
	height = (height+7) >> 3;
	textRect.bottom = textRect.top + height;
	handleSprite = CreateTextSprite( &textRect, 0 );

	/* Draw the Player's Handle into it */
	SetFontColors( 0, kDarkYellowColor, kYellowColor, 0 );
	DrawClippedSegaText( 0, 0, 128, handle );
	MoveSprite( handleSprite, xLeft+128, iconY + kPersonificationIconHeight + 130);
	
	/* Erase the buttons and text at the bottom */
	if (choose->leftButtonRef)
	{
		DisposeGraphicReference( choose->leftButtonRef );
		choose->leftButtonRef = nil;
	}
	if (choose->rightButtonRef)
	{
		DisposeGraphicReference( choose->rightButtonRef );
		choose->rightButtonRef = nil;	
	}
	
	/* Erase the "Hit Start when finished" string and erase it's name table */
	FreePatternBlock(choose->finishedPatterns);
	{
		segaCharRect eraser;
		
		eraser.top = 19;
		eraser.left = 5;
		eraser.bottom = 25;
		eraser.right = 34;
		choose->finishedPatterns = NewPatternBlock( 1 );
		FillNameTable( kScrollA, &eraser, choose->finishedPatterns );
	}
	
	DrawSprite( handleSprite );
	
	SetCurrentDevice( kScrollA );
	for (index=0; index<3; ++index)
	{
		GraphicReference grayOne;
	
		DelayTicks( 15, kNonSpecificCode );
		grayOne = DrawGraphicGray( theIcon, (choose->playerIconPoint.h+4)>>3, 
								choose->playerIconPoint.v>>3, 0, 0);
		VDPIdle();
		DelayTicks( 15, kNonSpecificCode );
		DisposeGraphicReference( grayOne );
		VDPIdle();
	}
	
	DisposeSprite( handleSprite );
}

static long ChooseIconCommand( long command, long refCon, ScreenParams *params )
{
Boolean			done;
ChooseIconGlobals	*choose;
short			result;
short			iii;
short			count;
short			collisionIndex;
unsigned long	curTicks;

	choose = (ChooseIconGlobals *)refCon;
	result = 0;
	
	UpdateButtonImages( choose, command );
	
	if (command & (kUP | kDOWN) )
	{
		curTicks = GetCurrentTime();
		if ( (curTicks - choose->lastTicks) < kHueAccelThreshold )
		{
			if (choose->hueAccel < 0x7F )
				choose->hueAccel++;
		}
		else
			choose->hueAccel = 1;
		choose->lastTicks = curTicks;
	}
	
	if (command & kLEFT)
	{
		if (choose->iconIndex == 0)
			choose->iconIndex = choose->iconCount-1;
		else
			choose->iconIndex--;
		choose->customHue = 0;					/* reset Hue & accel */
		choose->hueAccel = 1;
		PlayDBFX( kIconFlipSnd, 0, 0 );
		UpdateIcon(choose);
	}
	else if (command & kRIGHT)
	{
		choose->iconIndex = (choose->iconIndex+1) % choose->iconCount;
		choose->customHue = 0;					/* reset Hue & accel */
		choose->hueAccel = 1;
		PlayDBFX( kIconFlipSnd, 0, 0 );
		UpdateIcon(choose);
	}
	else if (command & kUP)
	{
		choose->customHue += choose->hueAccel;
		UpdateIcon( choose );
	}
	else if (command & kDOWN)
	{
		choose->customHue -= choose->hueAccel;
		UpdateIcon( choose );
	}
	else
		result = (command & kButtonPressed) != 0;
	
	if (result)
	{
		/* They hit a button to select the icon */
		BlinkPlayerIcon( choose );
	}
	
	VDPIdle();
	return result;
}

static void ChooseIconKill( long refCon )
{
ChooseIconGlobals	*choose = (ChooseIconGlobals *)refCon;

	if (choose->iconRef)
		DisposeGraphicReference(choose->iconRef);
	DisposeTextBoxReference(choose->myTextBoxMems);
	FreePatternBlock(choose->titlePatterns);
	FreePatternBlock(choose->descriptionPatterns);
	if (choose->finishedPatterns)
		FreePatternBlock(choose->finishedPatterns);
	if (choose->leftButtonRef)
		DisposeGraphicReference( choose->leftButtonRef );
	if (choose->rightButtonRef)
		DisposeGraphicReference( choose->rightButtonRef );
	DisposeMemory((Ptr)choose);
}

Ptr GetChooseIconDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) ChooseIconDispatch + 2));
	size = (*(long *) ((Ptr) GetChooseIconDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for choose icon proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) ChooseIconDispatch;
#else
		CheckCodeForJTRefs((void *)ChooseIconDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
