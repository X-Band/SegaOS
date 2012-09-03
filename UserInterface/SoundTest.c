/*
	File:		SoundTest.c

	Contains:	xxx put contents here xxx

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<16>	 8/22/94	JOE		now u can play with the FX freqs
		<15>	 8/20/94	JOE		made it more like a real screen
		<14>	 8/16/94	JOE		more cheez
		<13>	 8/15/94	JOE		String Diddlin'
		<13>	 8/15/94	JOE		String Diddlin'
		<12>	 8/15/94	JBH		EraseGDevice after screen, so deferred dialogs don't look like
									poo
		<11>	 8/13/94	JOE		call osidle every now and then
		<10>	  8/8/94	JOE		don't flicker the FX number
		 <9>	  8/8/94	KON		Remove ClearBackdrop.
		 <8>	  8/7/94	JOE		more skanky wiggliness
		 <7>	  8/6/94	JOE		update for new FXID/BMID format
		 <6>	  8/6/94	JOE		added mapping editing features
		 <5>	  8/4/94	JOE		Clear backdrop
		 <4>	 7/31/94	KON		Updated to use writeable string calls rather than hitting the
									database directly.
		 <3>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <2>	 7/29/94	JOE		VBL clut cycler uses VDPIsBusy() to decide if it's safe to whack
									the VDP, and defers a frame if not.
		 <1>	 7/28/94	JOE		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "DitlMgr.h"
#include "vdp.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "GameDB.h"
#include "UsrConfg.h"
#include "SegaIn.h"
#include "Time.h"
#include "Heaps.h"
#include "DataBase.h"
#include "StringDB.h"
#include "TextUtls.h"
#include "HistView.h"
#include "HistViewPriv.h"
#include "Screens.h"
#include "MungeCode.h"
#include "Errors.h"
#include "Backdrops.h"
#include "GraphicsDB.h"
#include "DITLItemSetup.h"
#include "StatText.h"
#include "StatIconGraphics.h"
#include "SegaIcons.h"
#include "RankingMgr.h"
#include "SegaSound.h"
#include "TextButt.h"
#include "DBTypes.h"
#include "Personification.h"
#include "VBLManager.h"
#include "SegaVDP.h"
#include "StringDB.h"
#include "Sprites.h"
#include "Math.h"
#include "Utils.h"
#include "SegaCluts.h"
#include "SoundTestPriv.h"


static long SoundTestDispatch( short action, long command, long *refCon, ScreenParams *params );
static long SoundTestSetup( ScreenParams *params );
static long SoundTestCommand( long command, long refCon, ScreenParams *params );
static void SoundTestKill( long refCon );

static void SetupTextPats( segaCharRect *r, long *pats, short left, short top, short right, short bottom, short color );
static void PadLR( SoundTestRefCon *STG, short command );
static void ZeroFreq( SoundTestRefCon *STG );
static void BlinkSelection( SoundTestRefCon *STG, short color );
static void DrawFXName( unsigned char select, segaCharRect *r, long pats, short c1, short c2, short pad );
static void DrawANum( unsigned long num, segaCharRect *r, long pats, short c1, short c2, short pad );
static void DrawAString( char *s, segaCharRect *r, long pats, short c1, short c2, short pad );
static long myCLUTvbl( SoundTestRefCon	*STG );
static TextBoxReference ZoomJizzlers( segaCharRect *endRect, short color, short device, short jizzleDelay );


static long SoundTestDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = 0;
			break;

		case kDoScreenReset:
			break;

		case kDoScreenSetupCommand:
			*refCon = SoundTestSetup( params );
			break;

		case kDoScreenCommand:
			result = SoundTestCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			SoundTestKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Sound Test Dispatch" );
			break;
			
		}
	return result;
}


static long SoundTestSetup( ScreenParams *params )
{
SoundTestRefCon	*STG;
short			clut[16];
segaCharRect	r;
char 			*sp;
char			s[10];
BMID			*curBGMID;
FXID			*curPatchID;

	STG = (SoundTestRefCon *) NewMemory( kTemp, sizeof(SoundTestRefCon) );

	StopBGM();
	
	STG->oldFont = GetCurFont ();
	SetCurFont( kXBandHeavy );
	
	STG->itemSelected = kBGMItem;
	
	STG->color = kDarkYellowColor;
	STG->blink = 0;
	
	EraseGDevice ( kScrollA );

	SetBackdropID ( kType2PlainBackground, true, 0 );

	SetCurrentDevice( kScrollA );
		
	GetClut( 0, clut );
	STG->c11 = clut[11];

	STG->s1 = 0L;			// don't spin the sheep until after the zoom
	
	STG->myvbl = AddVBLRequest( (VBLRequestProc)myCLUTvbl, (long)STG, kSoundTestClutCycleRate );

	r.left = 3; r.top = 2;
	r.right = 36; r.bottom = 25;
	STG->myTextBoxMems = ZoomJizzlers( &r, 11, kScrollA, 20 );
		
	SetupTextPats( &r, &(STG->titlePats), 10, 5, 30, 7, kDarkYellowColor );
	sp = GetWriteableString( kSoundTestText );
	DrawCenteredClippedSegaText( 20<<2, 0, MeasureSegaText( sp ), sp );
		
	SetupTextPats( &r, &(STG->bgmPromptPats), 11, 10, 19, 13, kDarkYellowColor );
	sp = GetWriteableString( kMusicTrackText );
	DrawClippedSegaText( 0, 0, MeasureSegaText( sp ), sp );

	SetupTextPats( &r, &(STG->musicPromptPats), 11, 14, 15, 17, kDarkYellowColor );
	sp = GetWriteableString( kMusicText );
	DrawClippedSegaText( 0, 0, MeasureSegaText( sp ), sp );

	SetupTextPats( &(STG->musicSetRect), &(STG->musicSetPats), 16, 14, 16+6, 17,kDarkYellowColor );
	STG->musicDisable = GetBGMDisable();
	if ( STG->musicDisable )
		sp = GetWriteableString( kOffText );
	else
		sp = GetWriteableString( kOnText );
	DrawClippedSegaText( 0, 0, MeasureSegaText( sp ), sp );
	

	STG->patchSelect = 0;
	curPatchID = (FXID *)DBGetItem( kSoundEffectRef, STG->patchSelect );
	if ( curBGMID )
		(STG->patchID).theFX = curPatchID->theFX;
	else
		(STG->patchID).theFX = 255;
	(STG->patchID).altFreq = 0;
	
	
	STG->bgmSelect = GetCurScreenIdentifier();							// where were we invoked from?
	curBGMID = (BMID *)DBGetItem( kBackgroundMusicRef, STG->bgmSelect ); 
	if ( curBGMID )
		(STG->bgmID).theBGM = curBGMID->theBGM;
	else
		(STG->bgmID).theBGM = 255;
	(STG->bgmID).pad = 0;
	
	
	SetupTextPats( &(STG->bgmIDRect), &(STG->bgmIDPats), 21, 10, 25, 13, kDarkYellowColor );
	SegaNumToString( STG->bgmSelect, s );
	DrawClippedSegaText( 0, 0, 24, s );

	SetupTextPats( &(STG->bgmRect), &(STG->bgmPats), 26, 10, 31, 13, kDarkYellowColor );
	SegaNumToString( (STG->bgmID).theBGM, s );
	DrawClippedSegaText( 0, 0, 24, s );

	SetupTextPats( &(STG->fxIDRect), &(STG->fxIDPats), 11, 18, 25, 21, kDarkYellowColor );
	DrawFXName( STG->patchSelect, &(STG->fxIDRect), STG->fxIDPats, 13, 12, kFXNamePixelWidth );

	SetupTextPats( &(STG->fxRect), &(STG->fxPats), 26, 18, 31, 20, kDarkYellowColor );
	SegaNumToString( (STG->patchID).theFX, s );
	DrawClippedSegaText( 0, 0, 24, s );
						
	STG->fxFreq = 0;
	SetupTextPats( &(STG->fxFreqRect), &(STG->fxFreqPats), 26, 21, 31, 24, kDarkYellowColor );
	SegaNumToString( STG->fxFreq, s );
	DrawClippedSegaText( 0, 0, 24, s );
						
					
	STG->theta = 0;	
	STG->s1 = CreateSprite( (SegaBitMap *)DBGetItem( kBitMapType, 87 ) );
	STG->sx = Sine(STG->theta)>>1;
	STG->sy = Cosine(STG->theta)>>1;
	MoveSprite( STG->s1, 128+160-32+(STG->sx), 128+120-32+(STG->sy) );
	DrawSprite( STG->s1 );
	
	return (long) STG;
}


static void SoundTestKill( long refCon )
{
SoundTestRefCon	*STG;
short			clut[16];

	STG = (SoundTestRefCon *)refCon;

	SetCurFont ( STG->oldFont );

	RemoveVBLRequest( STG->myvbl );

	StopTextBoxAnimation( STG->myTextBoxMems );
	DisposeTextBoxReference( STG->myTextBoxMems );

	DisposeSprite( STG->s1 );

	FreePatternBlock( STG->musicPromptPats );
	FreePatternBlock( STG->musicSetPats );
	FreePatternBlock( STG->fxIDPats );
	FreePatternBlock( STG->bgmIDPats );
	FreePatternBlock( STG->fxPats );
	FreePatternBlock( STG->bgmPats );
	FreePatternBlock( STG->titlePats );
	FreePatternBlock( STG->bgmPromptPats );
	FreePatternBlock( STG->fxFreqPats );

	GetClut( 0, clut );
	clut[11] = STG->c11;
	SetupClut( 0, clut, false );
	
	EraseGDevice( kScrollA );

	DisposeMemory( (Ptr)refCon );
}


static long SoundTestCommand( long command, long refCon, ScreenParams *params )
{	
SoundTestRefCon		*STG;

	STG = (SoundTestRefCon *) refCon;
	

	STG->blink++;
	if (STG->blink == 6)
	{
		STG->blink = 0;
		if (STG->color == 13)
			STG->color = 15;
		else
			STG->color = 13;
	}

	
	if ( !(command & kStart) )
	{
			
		if (command & kButtonA)
			switch ( STG->itemSelected )
			{
				case kBGMItem:
							StartDBBGM( STG->bgmSelect );
							break;
				
				case kSoundFXItem:
				case kSoundFXMapItem:
				case kSoundFXFreq:
							PlayDBFX( STG->patchSelect, 0, STG->fxFreq );
							break;
			}
			
		if ( command & (kLEFT | kRIGHT) )
			PadLR( STG, command );
		
		if ( (command & kUP) && (STG->itemSelected < kBGMItem) )
		{
			BlinkSelection( STG, kDarkYellowColor );
			STG->itemSelected++;
		}

		if ( (command & kDOWN) && (STG->itemSelected != 0) )
		{
			BlinkSelection( STG, kDarkYellowColor );
			STG->itemSelected--;
		}

		BlinkSelection( STG, STG->color );

		if ( STG->s1 && (STG->blink & 1) )
		{
			STG->theta+=2;							// spin the sheep
			STG->sx = Sine(STG->theta)>>1;
			STG->sy = Cosine(STG->theta)>>1;
			MoveSprite( STG->s1, 128+160-32+(STG->sx), 128+120-32+(STG->sy) );
			DrawSprite( STG->s1 );
		}

		VDPIdle();
		
		return ( 0 );
	}
	else
		return ( 1 );			// start --> we're done
		
}


static void PadLR( SoundTestRefCon *STG, short command )
{
	switch ( STG->itemSelected )
	{
		case kBGMItem:
						if ( command & kLEFT )
							(STG->bgmID).theBGM--;
						else
							(STG->bgmID).theBGM++;
						DBAddItemPtrSize( kBackgroundMusicRef, STG->bgmSelect, &(STG->bgmID), sizeof(BMID) );
						break;
						
		case kMusicDisableItem:
						STG->musicDisable = !(STG->musicDisable);
						SetBGMDisable( STG->musicDisable );
						break;
						
		case kSoundFXItem:
						if ( command & kLEFT )
						{
							if (DBGetItem( kSoundEffectRef, (STG->patchSelect)-1 ))
							{
								ZeroFreq( STG );
								(STG->patchSelect)--;
							}
						}
						else
						{
							if (DBGetItem( kSoundEffectRef, (STG->patchSelect)+1 ))
							{
								ZeroFreq( STG );
								(STG->patchSelect)++;
							}
						}
						if (DBGetItem( kSoundEffectRef, STG->patchSelect ))
							(STG->patchID).theFX = ((FXID *)DBGetItem( kSoundEffectRef, STG->patchSelect ))->theFX;
						else
							(STG->patchID).theFX = 255;
						DrawANum( (STG->patchID).theFX, &(STG->fxRect), STG->fxPats, kDarkYellowColor, kDarkYellowColor-1, 24 );				
						break;
						
		case kSoundFXMapItem:
						if ( command & kLEFT )
						{
							ZeroFreq( STG );
							(STG->patchID).theFX--;
						}
						else
						{
							ZeroFreq( STG );
							(STG->patchID).theFX++;
						}
						DBAddItemPtrSize( kSoundEffectRef, STG->patchSelect, &(STG->patchID), sizeof(FXID) );
						break;
				
		case kSoundFXFreq:
						if ( command & kLEFT )
						{
							if ( STG->fxFreq > 0 )
								(STG->fxFreq)--;
						}
						else
						{
							if ( STG->fxFreq < nB5 )
								(STG->fxFreq)++;
						}
						break;
	}
}


static void ZeroFreq( SoundTestRefCon *STG )
{
short	temp;

	STG->fxFreq = 0;
	temp = STG->itemSelected;
	STG->itemSelected = kSoundFXFreq;
	BlinkSelection( STG, kDarkYellowColor );
	STG->itemSelected = temp;
}


static void BlinkSelection( SoundTestRefCon *STG, short color )
{
BMID	*curBGMID;
FXID	*curPatchID;
char 	*sp;

	switch ( STG->itemSelected )
	{
		case kBGMItem:
						DrawANum( (STG->bgmID).theBGM, &(STG->bgmRect), STG->bgmPats, color, color-1, 24 );
						break;
						
		case kMusicDisableItem:
						if ( STG->musicDisable )
							sp = GetWriteableString( kOffText );
						else
							sp = GetWriteableString( kOnText );
						DrawAString( sp, &(STG->musicSetRect), STG->musicSetPats, color, color-1, 6*8 );
						break;
						
		case kSoundFXItem:
						DrawFXName( STG->patchSelect, &(STG->fxIDRect), STG->fxIDPats, color, color-1, kFXNamePixelWidth );
						break;
						
		case kSoundFXMapItem:
						DrawANum( (STG->patchID).theFX, &(STG->fxRect), STG->fxPats, color, color-1, 24 );				
						break;

		case kSoundFXFreq:
						DrawANum( STG->fxFreq, &(STG->fxFreqRect), STG->fxFreqPats, color, color-1, 24 );				
						break;
	}
}


void SetupTextPats( segaCharRect *r, long *pats, short left, short top, short right, short bottom, short color )
{
	r->left = left; r->right = right;
	r->top = top; r->bottom = bottom;
	*pats = LinearizeScreenArea( r, (0 | kMakeThisFrontPlane) );
	SetupTextGDevice( r, *pats );
	SetFontColors( 0, color, color