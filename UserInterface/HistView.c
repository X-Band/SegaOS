/*
	File:		HistView.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<47>	 8/28/94	JOE		Play the standard system select sound on exit
		<46>	 8/27/94	JOE		changed for joey
		<45>	 8/21/94	KON		DrawSpriteInFront now takes a hueShift.
		<44>	 8/18/94	JOE		Erase Sekrit Stats text GDevice when we flip to another stat
		<43>	  8/8/94	KON		Remove ClearBackdrop.
		<42>	  8/3/94	CMY		Oops. Fix a leak.
		<41>	  8/3/94	CMY		Added banner graphics and moved the return button
		<40>	  8/1/94	JOE		add support for secret stats
		<39>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<38>	 7/28/94	JOE		moved soundtest to its own file
		<37>	 7/27/94	CMY		Fix another pattern memory leak.
		<36>	 7/27/94	CMY		Fix a pattern memory leak.
		<35>	 7/27/94	JOE		Added BGM mapping display, fix clut bug
		<34>	 7/27/94	JOE		Added sound test page hack
		<33>	 7/26/94	JOE		Added a bunch of poo fo new UI
		<32>	 7/26/94	CMY		Changed the background to the correct one.
		<31>	 7/23/94	JOE		Joey-ified
		<30>	 7/22/94	CMY		Fixed the jizzle rect
		<29>	 7/21/94	JOE		trigger page flip FX
		<28>	 7/20/94	JOE		added jizzlin box
		<27>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<26>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<25>	  7/7/94	JOE		use ranking manager
		<24>	  7/5/94	JOE		user personification mgr to get handle & town now
		<23>	  7/4/94	ADS		New interface for screen control
		<22>	  7/4/94	SAH		(JOE) Fixed problem with kButtonPress in a switch statement on
									sega.
		<21>	  7/3/94	JOE		now support usericon, News-Style scroll buttons.  If the buttons
									stay, will need to add sounds here.
		<20>	  7/1/94	JOE		add support for multiple rankings per user, show info for
									current user, try to show stats for current cart.  KON's checkin
									made me merge, fucker.
		<19>	  7/1/94	KON		Killed a bunch of #if 0 shit.
		<18>	 6/30/94	JOE		more hacking.
		<17>	 6/29/94	JOE		History page is now a DITL with static fields.
		<16>	 6/28/94	SAH		Killed Results.h
		<15>	 6/28/94	SAH		#ifdefed some unused code for old history stuff.
		<14>	 6/21/94	KON		Show the stats background and rip out everything else for CES.
		<13>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		<12>	 6/14/94	SAH		Make sure GetHistoryViewDispatchPtr is the last function in the
									file.
		<11>	 6/13/94	KON		Make selection parameter to screens a long.
		<10>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <9>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
		 <8>	 6/10/94	KON		Added support for screen Preflight and Reset calls.
		 <7>	 6/10/94	KON		Update to use screen dispatcher.
		 <6>	  6/4/94	KON		Add some fun includes.
		 <5>	  6/4/94	KON		Use stringDB rather than hard coded strings.
		 <4>	  6/2/94	KON		Updated font colors to use new color table scheme.
		 <3>	 5/31/94	SAH		Killed SegaMisc.h.
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.
		 <5>	 5/23/94	KON		Add checks for NULL game names, user names, or opponent names.
		 <4>	 5/20/94	SAH		Updated call to LinearizeScreenArea.
		 <3>	 5/18/94	HEC		long pattern types
		 <2>	 5/17/94	KON		Update to use SetFontColors.

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
#include "SegaCluts.h"

static long HistoryViewDispatch( short action, long command, long *refCon, ScreenParams *params );
static long HistoryViewPreflight( long initValue );
static void HistoryViewReset( long refCon );
static long HistoryViewSetup( ScreenParams *params );
static long HistoryViewCommand( long command, long refCon, ScreenParams *params );
static void HistoryViewKill( long refCon );

static void UpdateRankingDisplay( HistoryViewRefCon *historyGlobals, RankUpdate *theRank );
static void DrawHiddenStat( HistoryViewRefCon *historyGlobals, char *s );
void DrawNextLevelShit( HistoryViewRefCon *historyGlobals );

static void DrawLeftFlipper( HistoryViewRefCon *historyGlobals, DBID theFlipper );
static void DrawRightFlipper( HistoryViewRefCon *historyGlobals, DBID theFlipper );


static long HistoryViewDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = HistoryViewPreflight( params->initValue );
			break;

		case kDoScreenReset:
			HistoryViewReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = HistoryViewSetup( params );
			break;

		case kDoScreenCommand:
			result = HistoryViewCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			HistoryViewKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in History View Dispatch" );
			break;
			
		}
	return result;
}



static void HistoryViewReset( long refCon )
{
HistoryViewRefCon	*historyGlobals;

	historyGlobals = (HistoryViewRefCon *) refCon;
}



static long HistoryViewPreflight( long initValue )
{
	return 0;
}


static long HistoryViewSetup( ScreenParams *params )
{
HistoryViewRefCon	*historyGlobals;
segaCharRect		myRect;
Point				myPoint, myPoint2;
SegaBitMapPtr		icon;
unsigned char		hue;
userIdentification	*curUser;
char				*s;
short				center;
TextButtonDesc		*TBDPtr;
xyString			*xys;
Rect				r;

	historyGlobals = (HistoryViewRefCon	*) NewMemory( kTemp, sizeof(HistoryViewRefCon) );

	historyGlobals->rightButtRef = 0L;
	historyGlobals->rightButtState = 0;
	historyGlobals->leftButtRef = 0L;
	historyGlobals->leftButtState = 0;
	historyGlobals->playerIcon = 0L;

	historyGlobals->secretRankDisplayed = false;
	
	EraseGDevice ( kScrollA );
	SetBackdropID( kType2PlainBackground, true, 0 );

	SetCurrentDevice( kScrollA );
	
	historyGlobals->theDitlList = SetupDITLItemList( kRankingScreen );	
	historyGlobals->myControlTable = SetupControlTable( kRankingScreen );

	curUser = GetCurrentLocalUser();

// Game Name data field (Centered, long)

	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewGameNameRect, &(historyGlobals->gameNameRect) );
	historyGlobals->gameNamePats = LinearizeScreenArea( &(historyGlobals->gameNameRect), 0 );

// Horz line & logo

	GetScreenLayoutPoint( kHistoryViewScreen, kHistViewStatsLineCoord, &myPoint );
	historyGlobals->statsLine = DrawDBGraphicAt( 0, kStatsLine, myPoint.h, myPoint.v, kScrollB );

	SetCurFont( kXBandBold10Font );
	SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
	
// XBand Points:
					
	historyGlobals->curPointsTextPats = DrawScreenLayoutString( kHistoryViewScreen, kHistViewXBandPointsString );
	
// XBand Points data field <in Scroll B>

	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewCurPointsRect, &(historyGlobals->curPointsRect) );
	historyGlobals->curPointsPats = LinearizeScreenArea( &(historyGlobals->curPointsRect), 0 );
					
// XBand Level:

	historyGlobals->curLevelTextPats = DrawScreenLayoutString( kHistoryViewScreen, kHistViewXBandLevelString );

// XBand Level data field < in Scroll B>

	SetCurrentDevice( kScrollB );
	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewCurLevelRect, &(historyGlobals->curLevelRect) );
	historyGlobals->curLevelPats = LinearizeScreenArea( &(historyGlobals->curLevelRect), 0 );
	SetCurrentDevice( kScrollA );
					
// Icon Frame 

	GetScreenLayoutPoint( kHistoryViewScreen, kHistViewStatsFrameCoord, &myPoint );
	historyGlobals->iconFrame = CreateSpriteInFront( GetDBGraphics( kStatsIconFrame ), 0 );
	MoveSprite( historyGlobals->iconFrame, 128+myPoint.h, 128+myPoint.v );
	DrawSprite( historyGlobals->iconFrame );

// Spudboy's Icon

	icon = GetPlayerIconBitMap( curUser );
	hue = (unsigned char)GetPersonificationPart( curUser->userID, kPersonificationROMClutID );	
	historyGlobals->playerIcon = CreateSpriteInFront( icon, hue );
	GetScreenLayoutPoint( kHistoryViewScreen, kHistViewPlayerIconCoord, &myPoint );
	MoveSprite( historyGlobals->playerIcon, 128+myPoint.h, 128+myPoint.v );
	DrawSprite( historyGlobals->playerIcon );

// Name

	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewPlayerNameRect, &(historyGlobals->playerNameRect) );
	historyGlobals->playerNamePats = LinearizeScreenArea( &(historyGlobals->playerNameRect), 0 );
	s = GetPersonificationPart( curUser->userID, kPersonificationHandle );
	SetupTextGDevice( &(historyGlobals->playerNameRect), historyGlobals->playerNamePats );
	SetCurFont( kXBandHeavy );
	SetFontColors( 0, kDarkRedColor, kRedColor, 0 );
	center = (((historyGlobals->playerNameRect).right - (historyGlobals->playerNameRect).left)) << 2;
	DrawCenteredClippedSegaText( center, 4, center << 1, s ); 

// Town <in Scroll B>

	SetCurrentDevice( kScrollB );
	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewPlayerTownRect, &(historyGlobals->playerTownRect) );
	historyGlobals->playerTownPats = LinearizeScreenArea( &(historyGlobals->playerTownRect), 0 );
	s = GetPersonificationPart( curUser->userID, kPersonificationTown );
	if ( CompareStrings( s, GetWriteableString( kNoneString ) ) )
	{
		SetupTextGDevice( &(historyGlobals->playerTownRect), historyGlobals->playerTownPats );
		SetCurFont( kXBandBold10Font );
		SetFontColors( 0, kDarkRedColor, kRedColor, 0 );
		center = (((historyGlobals->playerTownRect).right - (historyGlobals->playerTownRect).left)) << 2;
		DrawCenteredClippedSegaText( center, 0, center << 1, s ); 
	}
	SetCurrentDevice( kScrollA );
	
// BotRect:  used for Next & Secret fields

	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewBotRect, &(historyGlobals->botRect) );
	historyGlobals->botPats = LinearizeScreenArea( &(historyGlobals->botRect), 0 );

	DrawNextLevelShit( historyGlobals );

// Return Button (pound off the Return text)

	xys = (xyString *)DBGetItem( kStringType, 0 );						// ID 0 is the system null string
	TBDPtr = (TextButtonDesc *)((historyGlobals->theDitlList)->items[kRankReturn].objectData);
	TBDPtr->myCString = xys->cString;
	NewDITL( historyGlobals->myControlTable, historyGlobals->theDitlList );
	
// Set up the Jizzlin' Border

	GetScreenLayoutCharRect( kHistoryViewScreen, kHistViewJizzleRect, &myRect );
	historyGlobals->myTextBoxMems = DrawTextBox( &myRect, 11, kScrollA );
	StartTextBoxAnimation( historyGlobals->myTextBoxMems, 10 );
	{
		RankUpdate 			theRank;
		GetFirstRanking( &theRank );
		UpdateRankingDisplay( historyGlobals, &theRank );
	}
	
	VDPIdle();							// let the shit update
	return (long) historyGlobals;
}


void DrawNextLevelShit( HistoryViewRefCon *historyGlobals )
{
Rect	r;
Point	myPoint, myPoint2;
char	*s;

	SetCurFont( kXBandBold10Font );

// Next Level:  (at left edge of botRect, wrapped & left justified)

	s = GetScreenLayoutString( kHistoryViewScreen, kHistViewNextLevelString );

	GetScreenLayoutPoint( kHistoryViewScreen, kHistViewNextLevelYAndWidth, &myPoint2 );
	
	r.left = 0;
	r.top = myPoint2.v;
	r.right = myPoint2.h;
	r.bottom = ( (historyGlobals->botRect).bottom - (historyGlobals->botRect).top ) << 3;

	SetupTextGDevice( &(historyGlobals->botRect), historyGlobals->botPats );
	EraseTextGDevice( 0 );
	SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
	SegaBoxText( &r, s, kJustLeft, 0 );

// Points Needed:
	
	GetScreenLayoutPoint( kHistoryViewScreen, kHistViewPointsNeededXCoord, &myPoint );
	s = GetScreenLayoutString( kHistoryViewScreen, kHistViewPointsNeededString );

	GetScreenLayoutPoint( kHistoryViewScreen, kHistViewPointsNeededYAndWidth, &myPoint2 );

	r.left = myPoint.h;
	r.top = myPoint2.v;
	r.right = r.left + myPoint2.h;

	SetupTextGDevice( &(historyGlobals->botRect), historyGlobals->botPats );
	SetFontColors( 0, kDarkPurpleColor, kPurpleColor, 0 );
	SegaBoxText( &r, 