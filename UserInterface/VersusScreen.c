/*
	File:		VersusScreen.c

	Contains:	xxx put contents here xxx

	Written by:	britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<48>	 8/29/94	SAH		Call PeerConnectionDropped before disposing of all the graphics
									so the luser has something pretty to look at while tedward
									redials.
		<47>	 8/28/94	JOE		Add sanity checks for Taunt string len, Use NewMemoryClear() so
									I won't suck, cleaned up an edge case.
		<46>	 8/26/94	CMY		Fade out our screen before entering the game, so we don't get
									poo.
		<45>	 8/24/94	SAH		USe 19 bit packet format.
		<43>	 8/22/94	JOE		Ted sed: "Why doncha range check that Font ID"...fine idea!
		<42>	 8/22/94	JOE		Kut 'n Paist error for the standalone build
		<41>	 8/21/94	JOE		turn off the STANDALONE flag
		<40>	 8/21/94	JOE		OK, Kon!
		<39>	 8/21/94	KON		Pass hueshift into DrawSpriteInFront. Joe...get the hue shift
									from the userIdentification!!!
		<38>	 8/21/94	JOE		Revived the Who's-On-Left feature
		<37>	 8/20/94	JOE		Adjust X logo for Joweee
		<36>	 8/18/94	JOE		Embedded font is now fontnum + 1, so 0 doesn't screw up the
									string
		<35>	 8/18/94	ADS		Hide gBoxID globals
		<34>	 8/17/94	JOE		Use new game value func
		<33>	 8/14/94	HEC		Commented out STANDALONE.  Bracketed close/init GT session
									around PeerConnectionDropped.
		<32>	 8/14/94	JOE		I'm doubly lymp
		<31>	 8/13/94	HEC		Fixed versusGlobals->peerConnectErr when equal to kLinkError.
		<30>	 8/12/94	JOE		fix limpness
		<29>	 8/12/94	JOE		tweak for standalone testing
		<28>	 8/12/94	JOE		new custom icon stuff
		<27>	 8/11/94	JOE		Fix scroll turds, made it scroll out the taunts nicer
		<26>	 8/10/94	JOE		Draw opponent's taunt for the right player, not the left.
		<25>	  8/9/94	CMY		Added a flag to SHow/Hide Backdrop.
		<24>	  8/9/94	SAH		Get the opponent's personification dbid from
									GetOpponentPersonificationDBID rather than it's
									userIdentification (which contains the real userID).
		<23>	  8/8/94	HEC		Handle bad connections in VersusSetup (GTCreateLooseSession).
									Fade backdrop at end of screen.
		<22>	  8/8/94	SAH		Handle link closing better.
		<21>	  8/8/94	HEC		Removed READCONTROLS_ONVBL #ifdef around ControllerVBL().
		<20>	  8/8/94	KON		Remove ClearBackdrop.
		<19>	  8/7/94	JOE		put in amulet graphics + horz & vert lines, points stuff
		<18>	  8/7/94	HEC		Deal with network errors gracefully. Close versus screen at the
									min time if either player hits button before then.
		<17>	  8/5/94	SAH		VBL task for gametalk.
		<16>	  8/3/94	HEC		Leave versus screen up a minimum amount of time before bailing.
		<15>	  8/3/94	SAH		Added remote impatience.
		<14>	  8/3/94	CMY		Fixed the background.
		<13>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<12>	 7/26/94	SAH		Draw the user icon through the icon ref rather than the user id.
		<11>	 7/24/94	HEC		DrawTaunts now calls GetCurOpponentIdentification to get info on
									the remote player.
		<10>	 7/23/94	HEC		Fixed britt's spooge. Was trashing memory and indexing a
									pascal-style string instead of c.
		 <9>	 7/23/94	JOE		fuck
		 <8>	 7/22/94	CMY		Fixed jizzle rect
		 <7>	 7/21/94	JOE		Add flags param to SegaBoxText()
		 <6>	 7/21/94	JOE		trigger taunt type FX
		 <5>	 7/20/94	JOE		Now each user has a jizzle.
		 <4>	 7/20/94	JOE		just jizzlers to do now
		 <3>	 7/20/94	JOE		work in progress
		 <2>	 7/19/94	JOE		first checked in

	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "vdp.h"
#include "DITLItemSetup.h"
#include "SegaScrn.h"
#include "BackDrops.h"
#include "VersusScreen.h"
#include "StatText.h"
#include "UsrConfg.h"
#include "Opponent.h"
#include "Personification.h"
#include "SegaBM.h"
#include "SegaGraphics.h"
#include "SegaText.h"
#include "TextUtls.h"
#include "DBTypes.h"
#include "SegaIn.h"
#include "time.h"
#include "GTSendData.h"
#include "VersusScreenPriv.h"
#include "Screens.h"
#include "ScreenLists.h"
#include "Errors.h"
#include "heaps.h"
#include "MungeCode.h"
#include "StatText.h"
#include "Personification.h"
#include "GameID.h"
#include "GameDB.h"
#include "Events.h"
#include "Sprites.h"
#include "SegaSound.h"
#include "GraphicsDB.h"
#include "Controls.h"
#include "VBLManager.h"
#include "PeerConnect.h"
#include "SegaCluts.h"
#include "BoxSer.h"
#include "PatchDB.h"


static long VersusViewDispatch( short action, long command, long *refCon, ScreenParams *params );
static long VersusViewPreflight( long initValue );
static void VersusViewReset( long refCon );
static long VersusViewSetup( ScreenParams *params );
static long VersusViewCommand( long command, long refCon, ScreenParams *params );
static void VersusViewKill( long refCon );


static void DickWithVersusDITL( VersusViewRefCon *versusGlobals, Boolean localUserOnLeft );
static void DrawTaunts( VersusViewRefCon *versusGlobals, Boolean localUserOnLeft );
static void DrawATaunt( VersusViewRefCon *versusGlobals, short whichPlayerTaunt, 
													long *textPats, char *s);
static void DrawValue( VersusViewRefCon *versusGlobals );
void BreakTauntUp( char *s, long pixwidth, VersusViewRefCon *versusGlobals );
Boolean DrawTauntChar( VersusViewRefCon *versusGlobals );
void DumpTaunt( VersusViewRefCon *versusGlobals );

static void VersusVBL ( void );


//#define		STANDALONE				/* comment out for real build */

/*
	There are 9 items on the Versus Screen:  
	
								Game Name
	
				Player Icon						Opponent Icon
				
				Player Handle					Opponent Handle
				
				Player Town						Opponent Town
				
				Player Taunt					Opponent Taunt
				
	Layout is controlled by a Laid resource.  
*/



static long VersusViewDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = VersusViewPreflight( params->initValue );
			break;

		case kDoScreenReset:
			VersusViewReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = VersusViewSetup( params );
			break;

		case kDoScreenCommand:
			result = VersusViewCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			VersusViewKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Versus View Dispatch" );
			break;
			
		}
	return result;
}



static void VersusViewReset( long refCon )
{
VersusViewRefCon	*versusGlobals;

	versusGlobals = (VersusViewRefCon *) refCon;
	ClearDITLDone( versusGlobals->theDitlList );
}



static long VersusViewPreflight( long initValue )
{
	return 0;
}



static long VersusViewSetup( ScreenParams *params )
{
StatTextDesc		*STDPtr;
VersusViewRefCon	*versusGlobals;
segaCharRect 		myRect;
GTSession *			session;
OSErr 				err;
long				patchFlags;

	// Use NewMemoryClear, or risk sucking!
	versusGlobals = (VersusViewRefCon *) NewMemoryClear( kTemp, sizeof(VersusViewRefCon) );
	
	EraseGDevice ( kScrollA );
	SetBackdropID( kBlackBackground, true, 0 );
			
	versusGlobals->theDitlList = SetupDITLItemList( kVersusScreenLayout );	
	versusGlobals->myControlTable = SetupControlTable( kVersusScreenLayout );
	
	
	patchFlags = GetGamePatchFlags( GetGameID() );
	if ( (patchFlags & kMasterOnLeftFlag) && IsBoxMaster() )
		versusGlobals->localPlayerOnLeft = true;		
	else
		versusGlobals->localPlayerOnLeft = false;		


	DickWithVersusDITL( versusGlobals, params->selection );
	
	NewDITL( versusGlobals->myControlTable, versusGlobals->theDitlList );

	myRect.left = 3;
	myRect.top = 2;
	myRect.right = 36;
	myRect.bottom = 25;
		
	versusGlobals->myTextBoxMems = DrawTextBox( &myRect, 14, kScrollA );			
	StartTextBoxAnimation( versusGlobals->myTextBoxMems, 20 );

	myRect.left = 13;
	myRect.top = 10;
	myRect.right = 17;
	myRect.bottom = 10;
		
	versusGlobals->leftUserJizzle = DrawTextBox( &myRect, 12, kScrollA );		
	StartTextBoxAnimation( versusGlobals->leftUserJizzle, 20 );

	myRect.left = 24;
	myRect.top = 10;
	myRect.right = 28;
	myRect.bottom = 10;
		
	versusGlobals->rightUserJizzle = DrawTextBox( &myRect, 12, kScrollA );		
	StartTextBoxAnimation( versusGlobals->rightUserJizzle, 20 );

#ifdef STANDALONE
	versusGlobals->lsession = 0;
#else
	/* create the loose fucker - if we get an error here, we really want to bail on the */
	/* whole screen - how do we do that? */
	session = (GTSession *) params->initValue;
	GTSession_Init ( session, false );
	GTSession_SetGTPacketFormat ( session, k19BitData );
	while ( err = GTCreateLooseSession ( session, &versusGlobals->lsession ) )
	{
		WARNING_MESG( "Error creating loose session" );
		GTSession_Shutdown ( session );
		PeerConnectionDropped( err );
		GTSession_Init ( session, false );
	}
	/* install the gametalk vbl */
	versusGlobals->saveVBL = gVBLHandler;
	versusGlobals->saveGTSession = gVBLGTSession;
	
	gVBLGTSession = (long) session;
	gVBLHandler = (long) VersusVBL;
#endif

	DrawTaunts( versusGlobals, params->selection );
	
	DrawValue( versusGlobals );
	
	/* figure out when we may first go away */
	versusGlobals->minDisplayTime = GetCurrentTime() + kScreenMinDisplayTime;
			
	VDPIdle();											// let the shit update
	return (long)versusGlobals;
}



static void DickWithVersusDITL( VersusViewRefCon *versusGlobals, Boolean localUserOnLeft )
{
StatTextDesc		*STDPtr;
userIdentification	*left;
userIdentification	*right;
userIdentification	*temp;
char				*s;
DITLItemList 		*theDitlList;
SegaBitMapPtr		theLeftIcon;
SegaBitMapPtr		theRightIcon;
unsigned char		theLeftHue;
unsigned char 		theRightHue;
OSErr				err;
char				*leftPlayerHandle;
char				*leftPlayerTown;
char				*rightPlayerHandle;
char				*rightPlayerTown;


	if ( versusGlobals->localPlayerOnLeft )				/* who's on the left? */
	{
		left = GetCurrentLocalUser();
		leftPlayerHandle = GetPersonificationPart( left->userID, kPersonificationHandle );
		leftPlayerTown = GetPersonificationPart( left->userID, kPersonificationTown );	
		theLeftHue = (unsigned char)GetPersonificationPart( left->userID, kPersonificationROMClutID );	
#ifdef STANDALONE
		right = left;
		rightPlayerHandle = leftPlayerHandle;
		rightPlayerTown = leftPlayerTown;
		theRightHue = theLeftHue;
#else	
		right = GetCurOpponentIdentification();
		rightPlayerHandle = right->userName;
		rightPlayerTown = right->userTown;
		theRightHue = right->colorTableID;
#endif
	}
	else
	{
		right = GetCurrentLocalUser();		
		rightPlayerHandle = GetPersonificationPart( right->userID, kPersonificationHandle );
		rightPlayerTown = GetPersonificationPart( right->userID, kPersonificationTown );	
		theRightHue = (unsigned char)GetPersonificationPart( right->userID, kPersonificationROMClutID );	
#ifdef STANDALONE
		left = right;
		leftPlayerHandle = rightPlayerHandle;
		leftPlayerTown = rightPlayerTown;
		theLeftHue = theRightHue;
#else	
		left = GetCurOpponentIdentification();
		leftPlayerHandle = left->userName;
		leftPlayerTown = left->userTown;
		theLeftHue = left->colorTableID;
#endif
	}
	
	
	theLeftIcon = GetPlayerIconBitMap( left );				/* get icons */
	theRightIcon = GetPlayerIconBitMap( right );
		
	theDitlList = versusGlobals->theDitlList;
	

// Jizz Circle
	versusGlobals->jizzCircle = CreateSprite( (SegaBitMap *)DBGetItem( kBitMapType, kPreGameCircle ) );
	MoveSprite( versusGlobals->jizzCircle, 128+160-32, 128+48 );
	DrawSprite( versusGlobals->jizzCircle );

// XBand Logo
	versusGlobals->XGraphic = CreateSpriteInFront( (SegaBitMap *)DBGetItem( kBitMapType, kYellowXBandLogo ), 0 );
	MoveSprite( versusGlobals->XGraphic, 128+160-22, 128+63 );
	DrawSprite( versusGlobals->XGraphic );

// Horizontal Line
	versusGlobals->horzLine = DrawDBGraphicAt( 0, kPreGameHorzLine, 5, 6, kScrollB );

// Vertical Line
	versusGlobals->vertLine = DrawDBGraphicAt( 0, kPreGameVertLine, 20, 15, kScrollB );

	VDPIdle();

	
// Game Name
	STDPtr = (StatTextDesc *)(theDitlList->items[kGameNameText].objectData);
	s = GetGameName( GetGameID() );
	if ( s )
	  	STDPtr->myCString = s;


// Player Icons
	versusGlobals->leftIcon = CreateSpriteInFront( theLeftIcon, theLeftHue );
	MoveSprite( versusGlobals->leftIcon, 
					(versusGlobals->theDitlList)->items[kLeftPlayerIcon].xPos, 
					(versusGlobals->theDitlList)->items[kLeftPlayerIcon].yPos );
	DrawSprite( versusGlobals->leftIcon );
	
	versusGlobals->rightIcon = CreateSpriteInFront( theRightIcon, theRightHue );
	MoveSprite( versusGlobals->rightIcon, 
					(versusGlobals->theDitlList)->items[kRightPlayerIcon].xPos, 
					(versusGlobals->theDitlList)->items[kRightPlayerIcon].yPos );
	DrawSprite( versusGlobals->rightIcon );
	

// Left Player Handle
	STDPtr = (StatTextDesc *)(theDitlList->items[kLeftPlayerHandle].objectData);
	if ( leftPlayerHandle )
	  	STDPtr->myCString = leftPlayerHandle;

// Left Player Town
	STDPtr = (StatTextDesc *)(theDitlList->items[kLeftPlayerTown].objectData);
	if ( leftPlayerTown )
	  	STDPtr->myCString = leftPlayerTown;
	
// Right Player Handle
	STDPtr = (StatTextDesc *)(theDitlList->items[kRightPlayerHandle].objectData);
	if ( rightPlayerHandle )
	  	STDPtr->myCString = rightPlayerHandle;

// Right Player Town
	STDPtr = (StatTextDesc *)(theDitlList->items[kRightPlayerTown].objectData);
	if ( rightPlayerTown )
	  	STDPtr->myCString = rightPlayerTown;
}


/*
	Taunts are drawn center-justified to the DB specified XPos into a box at the 
	DB specified YPos.  The amount the box extends to the left & right of center
	is set by the constant kTauntBoxHalfWidth (yuk).
	The box height is set by the kTauntPatHeight constant.
*/
	
static void DrawTaunts( VersusViewRefCon *versusGlobals, Boolean localUserOnLeft )
{
short				oldFont;
char				*leftTaunt;
char				*rightTaunt;

	if ( versusGlobals->localPlayerOnLeft )				/* who's on the left? */
	{
		leftTaunt = GetPersonificationPart( GetCurUserID(), kPersonificationTauntText );	
#ifdef STANDALONE
		rightTaunt = leftTaunt;
#else	
		rightTaunt = GetCurOpponentTaunt();
#endif
	}
	else
	{
		rightTaunt = GetPersonificationPart( GetCurUserID(), kPersonificationTauntText );	
#ifdef STANDALONE
		leftTaunt = rightTaunt;
#else	
		leftTaunt = GetCurOpponentTaunt();
#endif
	}


	SetCurrentDevice( kScrollA );
	oldFont = GetCurFont ();

	DrawATaunt( versusGlobals, kLeftPlayerTaunt, &(versusGlobals->leftTauntTextPat), leftTaunt );
		
	DrawATaunt( versusGlobals, kRightPlayerTaunt, &(versusGlobals->rightTauntTextPat), rightTaunt );

	SetCurFont( oldFont );
}



static void DrawATaunt( VersusViewRefCon *versusGlobals, short whichPlayerTaunt, 
															long *textPats, char *s)
{
StatTextDesc		*STDPtr;
segaCharRect		textRect;
Rect				pixRect;
short				XPos;
short				YPos;
char				*d;
long				impatient;
long *				outData;
long				remoteImpatient;
Err					err;
short				slen;

	XPos = (versusGlobals->theDitlList)->items[whichPlayerTaunt].xPos;
	YPos = (versusGlobals->theDitlList)->items[whichPlayerTaunt].yPos;

	textRect.top = YPos >> 3;
	textRect.bottom = textRect.top + kTauntPatHeight;
	textRect.left = (XPos >> 3) - kTauntPatHalfWidth;
	textRect.right = (XPos >> 3) + kTauntPatHalfWidth;
			
	STDPtr = (StatTextDesc *)((versusGlobals->theDitlList)->items[whichPlayerTaunt].objectData);

	pixRect.top = (STDPtr->animationData).yPos;
	pixRect.left = 0;
	pixRect.bottom = kTauntPatHeight * 8;
	pixRect.right = (kTauntPatHalfWidth * 2 * 8);
			
	SetCurFont ( STDPtr->font );

	*textPats = LinearizeScreenArea( &textRect, STDPtr->palette );
	if ( *textPats )	
	{
		SetupTextGDevice( &textRect, *textPats );
				
		SetFontColors( STDPtr->color0, STDPtr->color1, STDPtr->color2, STDPtr->color3 );
		
		if ( !s )
			return;

		slen = GetStringLength( s );
		
		if ( (slen == 0) || (slen > 128) )		/* sanity check */
			return;
		
		d = NewMemoryClear ( kTemp, slen+1 );
		CopyCString( s, d );
		BreakTauntUp( d, pixRect.right, versusGlobals );		/* break the taunt up into 3 lines */		
		
		if ( versusGlobals->impatient )
			DumpTaunt( versusGlobals );
		else
		{
			while ( DrawTauntChar( versusGlobals ) )
			{
				PlayDBFX( kTauntTextAppearSnd, 0, 0 );
				
				/* give the modem a chance */
				if ( versusGlobals->lsession )
				{
					GTSession_ReadHardwareModem ( versusGlobals->lsession->session );
				}
					
				DelayTicks( 2, kNonSpecificCode );
					
				/* see if either one is impatient - see if we need to send something or not */
				impatient = CheckUserButtonPress();
				outData = 0L;
				if ( impatient )
				{
					outData = &impatient;
				}
					
				/* we structure the loop this way as we always 