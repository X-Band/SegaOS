/*
	File:		ChoosePassword.c

	Contains:	New password chooser

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<17>	 8/27/94	KON		Install the empty password, not the (random) unconfirmed one in
									the case of clearing the password.
		<16>	 8/27/94	JBH		Still don't verify empty password, but we do install it.
		<15>	 8/25/94	CMY		Don't ask for verify on empty password.
		<14>	 8/16/94	JOE		Slide the splats over so they all fit in the input rect, add
									splatty sound fx
		<13>	 8/11/94	JOE		ChangeUserPersonificationPart now wants a flag
		<12>	  8/9/94	CMY		Fixed the text so it doesn't overwrite itself.
		<11>	  8/5/94	CMY		Added some text so you know what to do.
		<10>	  8/3/94	JOE		Fixed, made 2nd param to AddPWChar() a long, not a short (all
									ChoosePWProcs have a long 2nd param), added a VDPIdle() so u can
									see the splats
		 <9>	  8/3/94	JOE		ChangePersonificationPart() became
									ChangeUserPersonificationPart()
		 <8>	  8/3/94	CMY		Fixed backgrounds.
		 <7>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <6>	 7/22/94	CMY		Fixed reference to nonexistent background graphic.
		 <5>	 7/21/94	ADS		Fixed colors, but don't know if it works (we crash here)
		 <4>	 7/19/94	ADS		Vector Mania
		 <3>	 7/15/94	ADS		This time it really works
		 <2>	 7/15/94	ADS		Stupid idiot checkin piece of shit script
		 <1>	 7/15/94	ADS		first checked in (display, no function)

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
#include "SegaCluts.h"
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
#include "Personification.h"
#include "Sprites.h"
#include "SegaSound.h"


typedef (*ChoosePasswordProc) (void* globals, long command);

typedef struct {
	short			txX, txY;
	PasswordString	newPW;
	short			newPWFill;
	SpriteRef		charSprite;
	void*			textState;
	
	TextBoxReference	jizzleRef;
	long				titlePatterns;
	long				descriptionPatterns;
	long				donePatterns;
	GraphicReference	boxGraphic;
	
	ChoosePasswordProc	addPWChar;
	ChoosePasswordProc	doubleCheckAndSetPassword;
	ChoosePasswordProc	resetEnteredPassword;
	ChoosePasswordProc	setupPWChar;
	
} ChoosePasswordGlobals;



	//  SHOULD BE IN SPRITES.H !!!!
	
#define kSpriteOffset	128

#define kLayoutJizzleRect			0
#define kLayoutDescriptionRect		1

#define kLayoutTitleString			0
#define kLayoutDescriptionString	1
#define kLayoutDoneString			2

#define kLayoutBoxPoint				0


// set up magic numbers for up to kMaxPWLength password bullets

#define kDotColor0	(0)
#define kDotColor1	(kDarkRedColor)
#define kDotColor2	(kRedColor)
#define kDotColor3	(0)

#define kDotSpacing	(8)									// pixels per dot
#define kDotXAdjust	(16)								// tweak to fit
#define kDotRange	(kDotSpacing * (kMaxPWLength-1))
#define kDotText	'X'

#define	kBoxTop		(155)
#define	kBoxLeft	(80)
#define	kBoxBottom	(168)
#define	kBoxRight	(190)
#define kBoxWidth	(kBoxRight - kBoxLeft)
#define kBoxHeight	(kBoxBottom - kBoxTop)

#define kAllLetters	( kButtonA | kButtonB | kButtonC )




static long ChoosePasswordPreflight( long initValue );
static void ChoosePasswordReset( long refCon );
static long ChoosePasswordSetup( ScreenParams *params );
static long ChoosePasswordCommand( long command, long refCon, ScreenParams *params );
static void ChoosePasswordKill( long refCon );

static void _AddPWChar( ChoosePasswordGlobals* choose, long command );
static long	_DoubleCheckAndSetPassword( ChoosePasswordGlobals* choose );
static void _ResetEnteredPassword( ChoosePasswordGlobals* choose );
static void _SetupPWChar( ChoosePasswordGlobals* choose );

#define AddPWChar( glob, command )			((glob->addPWChar) ( glob, command ))
#define DoubleCheckAndSetPassword( glob )	((glob->doubleCheckAndSetPassword) ( glob, 0 ))
#define ResetEnteredPassword( glob )		((glob->resetEnteredPassword) ( glob, 0 ))
#define SetupPWChar( glob )					((glob->setupPWChar) ( glob, 0 ))




static long ChoosePasswordDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = ChoosePasswordPreflight( params->initValue );
			break;

		case kDoScreenReset:
			ChoosePasswordReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = ChoosePasswordSetup( params );
			break;

		case kDoScreenCommand:
			result = ChoosePasswordCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			ChoosePasswordKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in ChoosePassword Dispatch" );
			break;
			
		}
	return result;
}



static void ChoosePasswordReset( long refCon )
{
ChoosePasswordGlobals	*choose;

	choose = (ChoosePasswordGlobals *)refCon;

}


// return 0 if it's OK to run this screen

static long ChoosePasswordPreflight( long initValue )
{
	if (VerifyPlayerPassword(GetCurrentLocalUser()->userID) )
		return 0;
	else
		return 1;
}




static long ChoosePasswordSetup( ScreenParams *params )
{
ChoosePasswordGlobals	*choose;
Point					boxLocation;
segaCharRect			tbRect;

	choose = (ChoosePasswordGlobals *)NewMemory( kTemp, sizeof(ChoosePasswordGlobals));
	
	choose->addPWChar					= (ChoosePasswordProc) _AddPWChar;
	choose->doubleCheckAndSetPassword	= (ChoosePasswordProc) _DoubleCheckAndSetPassword;
	choose->resetEnteredPassword		= (ChoosePasswordProc) _ResetEnteredPassword;
	choose->setupPWChar					= (ChoosePasswordProc) _SetupPWChar;
		
	choose->charSprite = 0;		// zero allocations
	choose->textState = 0;
	
	EraseGDevice ( kScrollA );
	SetBackdropID ( kBlackBackground, true, 0 );
	SetCurrentDevice( kScrollA );
	
	GetScreenLayoutPoint( kChoosePasswordScreen, kLayoutBoxPoint, &boxLocation );
	choose->boxGraphic = DrawDBGraphicAt( 0, kPasswordBox, boxLocation.h>>3, 
		boxLocation.v>>3, kScrollA );

	SetCurrentDevice( kScrollA );
	SetCurFont( kXBandHeavy );
	SetFontColors( 0, 11, 10, 0 );
	choose->titlePatterns = DrawScreenLayoutString( kChoosePasswordScreen, kLayoutTitleString );

	SetCurFont( kXBandLight9Font );
	SetFontColors( 0, 9, 8, 0 );
	choose->descriptionPatterns = BoxScreenLayoutString( kChoosePasswordScreen, kLayoutDescriptionString,
		kLayoutDescriptionRect, kJustLeft );	

	SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
	choose->donePatterns = DrawScreenLayoutString( kChoosePasswordScreen, kLayoutDoneString );

	GetScreenLayoutCharRect( kChoosePasswordScreen, kLayoutJizzleRect, &tbRect );
	choose->jizzleRef = DrawTextBox( &tbRect, kGreenColor, kScrollA );
	StartTextBoxAnimation( choose->jizzleRef, 10 );
	
	SetupPWChar( choose );

	return (long) choose;
}



static long ChoosePasswordCommand( long command, long refCon, ScreenParams *params )
{
Boolean					done;
ChoosePasswordGlobals*	choose = (ChoosePasswordGlobals *)refCon;
long					result;

	choose = (ChoosePasswordGlobals *)refCon;
	result = 0;
	
	VDPIdle();
	
	if (command)
		AddPWChar( choose, command );
		
	if (command & kStart)
		result = DoubleCheckAndSetPassword( choose );
	
	return result;
}


static void ChoosePasswordKill( long refCon )
{
ChoosePasswordGlobals	*choose = (ChoosePasswordGlobals *)refCon;

	if (choose->textState)
		DisposeTextStateReference( choose->textState );
	if (choose->charSprite)
		DisposeSprite( choose->charSprite );
	
	FreePatternBlock( choose->titlePatterns );
	FreePatternBlock( choose->descriptionPatterns );
	FreePatternBlock( choose->donePatterns );
	
	DisposeTextBoxReference( choose->jizzleRef );
	
	DisposeGraphicReference( choose->boxGraphic );
	
	DisposeMemory( (Ptr)choose);
}






static void _AddPWChar( ChoosePasswordGlobals* choose, long command )
{
short			tempX;
char			dotText[10];
short			index;
	
	// if it's "A" "B" or "C", add it to the string
		
	if (( command & kAllLetters) && !( command & ~kAllLetters) )	{
	
		PlayDBFX( kPasswordEntrySnd, 0, 0 );

		if (choose->newPWFill >= kMaxPWLength)	{
			return;
		}
		choose->newPW[choose->newPWFill++] = command;			// record a stroke
			
		tempX = choose->txX;
		
		for (index=0; index<choose->newPWFill; ++index)
			dotText[index] = kDotText;
		dotText[choose->newPWFill] = 0;

		SetCurFont(kMain2BitFont);
		SetFontColors( kDotColor0, kDotColor1, kDotColor2, kDotColor3 );
		DrawSegaString( &tempX, &choose->txY, dotText );
		VDPIdle();
	}
}



static long	_DoubleCheckAndSetPassword( ChoosePasswordGlobals* choose )
{
PasswordString	secondPW;
Boolean			match;

	//	0.	Check for empty password. If so, just accept it.
	
	if ( choose->newPW[0] == 0 )
	{
		match = true;
	}
	else
	{
		//	1.  Ask user to re-enter the same password
		
		DoDialogParam( kReenterNewPassword, &secondPW );
		
		//	2.  Find out if 1st matched second
		
		match = ComparePW( &secondPW, &choose->newPW);
		
	}
	
	//	3.  Accept if it's a match, reject if not
	if (match)
	{
		ChangeUserPersonificationPart( GetCurrentLocalUser()->userID,
										kPersonificationPassword, &choose->newPW, false );
	}
	else
	{
		DoDialog( kNewPasswordNoMatch );
		RestoreTextState( choose->textState);	// since dialogs messed us up
		ResetEnteredPassword( choose );
	}
	
	//	4.  Return true/false result	- true means we're done with this screen

	return match;
}


static void _ResetEnteredPassword( ChoosePasswordGlobals* choose )
{
long*			dummy;

	EraseTextGDevice(kDotColor0);			// blank out entered chars
	
	choose->txX = ((kBoxWidth - kDotRange) >> 1) + kDotXAdjust;	// reset cursor
	choose->txY = 0;
	
	dummy = (long*) &choose->newPW;			// preset new password to zero
	*dummy++ = 0;
	*dummy++ = 0;
	choose->newPWFill = 0;
}	


static void _SetupPWChar( ChoosePasswordGlobals* choose )
{
short			txX, txY;
segaCharRect	charRect;

	// convert the numbers to 8*8 patches
	
	charRect.top = 0;
	charRect.left = 0;
	charRect.bottom = (kBoxHeight + 7) >> 3;
	charRect.right = (kBoxWidth + 7) >> 3;
	
	// make sure they are in groups of 4
	
	charRect.bottom = (charRect.bottom + 3) & -4;
	charRect.right = (charRect.right + 3) & -4;
	
	// make a sprite now

	choose->charSprite = CreateTextSprite( &charRect, 0 );
	MoveSprite ( choose->charSprite, kBoxLeft + kSpriteOffset, kBoxTop + kSpriteOffset );
	DrawSprite ( choose->charSprite );
	
	// clear out the PW entry
	
	ResetEnteredPassword( choose );
	
	//  save the password entry state
	
	choose->textState = GetTextStateReference();
}

	


Ptr GetChoosePasswordDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) ChoosePasswordDispatch + 2));
	size = (*(long *) ((Ptr) GetChoosePasswordDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for choose icon proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) ChoosePasswordDispatch;
#else
		CheckCodeForJTRefs((void *)ChoosePasswordDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
