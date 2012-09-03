/*
	File:		AboutBox.c

	Contains:	Secret screen

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/29/94	JBH		We're close to ROM freeze. Time to make the clut cycler use the
									HueShift™ technology.
		<14>	 8/16/94	JOE		about box now has its own sound effect
		<13>	  8/5/94	JOE		adjust scale for new sine table
		<12>	  8/4/94	JOE		messed with paddles & names
		<11>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<10>	 7/26/94	ADS		Mostly complete
		 <9>	 7/19/94	ADS		Fucked up sprite dispose
		 <8>	 7/17/94	ADS		Names, ditl, vectors
		 <7>	 7/13/94	ADS		Real players.  Fixed speed problem
		 <6>	 7/13/94	ADS		Use LongMultiply so we don't have A5 problems
		 <5>	 7/13/94	ADS		Remove hardcoded string, see if it works on Sega now
		 <4>	 7/10/94	ADS		More functionality
		 <3>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		 <2>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <1>	  7/5/94	ADS		first checked in
	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "VBLManager.h"
#include "AboutBoxPriv.h"
#include "Screens.h"
#include "Errors.h"
#include "Heaps.h"
#include "SegaScrn.h"
#include "GraphicsDB.h"
#include "BackDrops.h"
#include "vdp.h"
#include "SegaText.h"
#include "StringDB.h"
#include "MungeCode.h"
#include "DBTypes.h"
#include "Time.h"
#include "SegaSound.h"
#include "Math.h"
#include "Controls.h"
#include "SegaIn.h"
#include "SegaVDP.h"
#include "SegaCluts.h"

#include "stdio.h"

//
// Local routines
//

static void _SetupPlayers(AboutBoxGlobals* globals, long dummy);
static void _SetupBall(AboutBoxGlobals* globals, long dummy);
static void _SetupGame(AboutBoxGlobals* globals, long dummy);
static void _DisposeGame(AboutBoxGlobals* globals, long dummy);

static void _RunGame(AboutBoxGlobals* globals, long dummy);
static void _MoveBall(AboutBoxGlobals* globals, long dummy);
static void _MovePlayer(AboutBoxGlobals* globals, long playerNum);
static Boolean _CheckPaddleHit(AboutBoxGlobals* globals, long playerNum);

static void _SetupNames(AboutBoxGlobals* globals, long dummy);
static void _MoveNames( AboutBoxGlobals* globals, long dummy );
static void _DisposeNames( AboutBoxGlobals* globals, long dummy );
static void _FillOneName( AboutBoxGlobals* globals, NameSprite*	name );

static long myCLUTvbl( AboutBoxGlobals *globals );


#define SET_PROC(num, ptr)			globals->procs[num] = (AboutProc) ptr
#define	CALL_PROC(proc, g, p)		(g->procs[proc]) (g, p)


#define SetupPlayers()				CALL_PROC (kSetupPlayers,		globals, 0)
#define SetupBall()					CALL_PROC (kSetupBall,			globals, 0)
#define SetupGame()					CALL_PROC (kSetupGame,			globals, 0)
#define DisposeGame()				CALL_PROC (kDisposeGame,		globals, 0)
#define RunGame()					CALL_PROC (kRunGame,			globals, 0)
#define MoveBall()					CALL_PROC (kMoveBall,			globals, 0)
#define MovePlayer( playerNum )		CALL_PROC (kMovePlayer,			globals, (playerNum))
#define CheckPaddleHit( playerNum )	CALL_PROC (kCheckPaddleHit,		globals, (playerNum))
#define SetupNames()				CALL_PROC (kSetupNames,			globals, 0)
#define MoveNames()					CALL_PROC (kMoveNames,			globals, 0)
#define DisposeNames()				CALL_PROC (kDisposeNames,		globals, 0)
#define FillOneName( name )			CALL_PROC (kFillOneName,		globals, (long) (name))

#ifdef SIMULATOR
	#define halt	Debugger()
#else
	#define halt	asm { dc.w 0xA003 }
#endif



//
// Screen handling routines
//
static long AboutBoxPreflight( long initValue );
static void AboutBoxReset( long refCon );
static long AboutBoxSetup( ScreenParams *params );
static long AboutBoxCommand( long command, long refCon, ScreenParams *params );
static void AboutBoxKill( long refCon );


static long AboutBoxDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = AboutBoxPreflight( params->initValue );
			break;

		case kDoScreenReset:
			AboutBoxReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = AboutBoxSetup( params );
			break;

		case kDoScreenCommand:
			result = AboutBoxCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			AboutBoxKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in Find Opponent Dispatch" );
			break;
			
		}
	return result;
}

static void AboutBoxReset( long refCon )
{
AboutBoxGlobals *findGlobals;

}

static long AboutBoxPreflight( long initValue )
{
	return 0;
}

#define PACK_SEGA_COLOR(r,g,b) ( ((r & 7)<<1) | ((g & 7)<<5) | ((b & 7)<<9) )

static long AboutBoxSetup( ScreenParams *params )
{
AboutBoxGlobals*	globals;
short				clut[16];
short				iii;

	globals = (AboutBoxGlobals *)NewMemory( kTemp, sizeof(AboutBoxGlobals) );
	
	SET_PROC( kSetupPlayers,	_SetupPlayers);
	SET_PROC( kSetupBall,		_SetupBall);
	SET_PROC( kSetupGame,		_SetupGame);
	SET_PROC( kDisposeGame,		_DisposeGame);
	SET_PROC( kRunGame,			_RunGame);
	SET_PROC( kMoveBall,		_MoveBall);
	SET_PROC( kMovePlayer,		_MovePlayer);
	SET_PROC( kCheckPaddleHit,	_CheckPaddleHit);
	SET_PROC( kSetupNames,		_SetupNames);
	SET_PROC( kMoveNames,		_MoveNames);
	SET_PROC( kDisposeNames,	_DisposeNames);
	SET_PROC( kFillOneName,		_FillOneName);
	
	EraseGDevice( kScrollA );
	SetBackdropID ( kBackdrop, true, 0 );

	
	globals->theDitlList = SetupDITLItemList( kAboutBoxLayout );	
	globals->myControlTable = SetupControlTable( kAboutBoxLayout );
	NewDITL( globals->myControlTable, globals->theDitlList );
			
	
	/* minor === BRAIN DAMAGE ===.  We attach this clut to a sprite, which will later get */
	/* disposed. Since the UniqueClut has no "owner count" (it's a magic value -99), when */
	/* the sprite goes, the clut goes as well, so the other name string sprites are using */
	/* a CLUT that is free. Fortunately, nobody needs another clut in here, so we won't */
	/* fix this. */
	
	globals->namesClut = RequestUniqueClut( clut );
	for (iii = 0; iii != 16; iii++ )
		clut[iii] = HueShift( 0x000E, iii<<4);
//		clut[iii] = PACK_SEGA_COLOR( iii, (iii>>1), (iii | 2) );
	SetupClut( globals->namesClut, clut, false );
	
	globals->paddleClut = RequestUniqueClut( clut );
	for (iii = 1; iii != 15; iii++ )
		clut[iii] = HueShift( 0x00E0, iii<<4);
//		clut[iii] = PACK_SEGA_COLOR( (iii>>1), (iii>>2), (iii>>3) );
	SetupClut( globals->paddleClut, clut, false );


	SetupNames();	
	SetupBall();
	SetupPlayers();
	SetupGame();		// sets up timers, etc
	
	
	globals->myvbl = AddVBLRequest( (VBLRequestProc)myCLUTvbl, (long)globals, kPaddleClutRate );

	return (long)globals;
}


static long AboutBoxCommand( long command, long refCon, ScreenParams *params )
{
AboutBoxGlobals *globals;

	globals = (AboutBoxGlobals *)refCon;


	if (globals->lastTime == -1)				// special case first frame
		globals->lastTime = GetCurrentTime() - 30;

	globals->thisTime = GetCurrentTime();
	globals->delta = globals->thisTime - globals->lastTime;
	globals->lastTime = globals->thisTime;
	
#ifdef SIMULATOR			// prevent overruning
	if ((unsigned long) (globals->delta) > 20)
		globals->delta = 20;
#endif
	
	RunGame();
	MoveNames();
	
	if (command & kButtonPressed)
		return 1;	//go back to main screen
	
	return 0;
}

static void AboutBoxKill( long refCon )
{
AboutBoxGlobals *globals;

	globals = (AboutBoxGlobals *)refCon;

	RemoveVBLRequest( globals->myvbl );
	
	ReleaseClut( globals->namesClut );
	ReleaseClut( globals->paddleClut );
	
	DisposeGame();
	
	DisposeNames();

	DisposeDITL( globals->theDitlList );
	DisposeControlTable( globals->myControlTable );
	DisposeDITLItemList( globals->theDitlList );

	DisposeMemory( (Ptr) globals );
}


static long myCLUTvbl( AboutBoxGlobals *globals )
{
short	clut[16];
short	temp;
short	iii;

	if (!VDPIsBusy())
	{
		GetClut( globals->paddleClut, clut );
		temp = clut[15];
		for (iii = 15; iii != 1; iii-- )
			clut[iii] = clut[iii-1];
		clut[1] = temp;
		SetupClut( globals->paddleClut, clut, false );
		
		GetClut( globals->namesClut, clut );
		temp = clut[15];
		for (iii = 15; iii != 0; iii-- )
			clut[iii] = clut[iii-1];
		clut[0] = temp;
		SetupClut( globals->namesClut, clut, false );
		
		return ( kPaddleClutRate );
	}
	else
		return (1);		// defer for a tick
}





#define kSpriteOffset	128

#define	kBallWidth		48
#define kBallHeight		34

#define	kPaddleWidth	8
#define kPaddleHeight	40

#define kBallGraphic	(62)
#define kPaddleGraphic	(63)

#define kPaddleCPURate		(0x00005000)
#define kPaddlePlayerRate	(0x0000E000)

#define kBallIdle		0
#define kBallPlaying	1
#define kBallEscape		2
#define kAttractMode	3

#define kPlayTop		20
#define kPlayLeft		20
#define kPlayBottom		175
#define kPlayRight		300

#define kScreenHeight	224
#define kScreenWidth	320

#define	kBallStartX		((long) (kScreenWidth - kBallWidth) >> 1)
#define kBallStartY		((long) 32)

#define kBallAccelX		(0x00001400)
#define kBallAccelY		(0x00001800)

#define kMaxBallDx		(0x00100000)
#define kMaxBallDy		(0x000C0000)



static void _RunGame(AboutBoxGlobals* globals, long dummy)
{
	switch (globals->gameState)	{
		case kBallIdle:
			DrawSprite ( globals->ballSprite );
			globals->stateTimer -= globals->delta;
			if (globals->stateTimer < 0)
				globals->gameState = kBallPlaying;
			break;
		case kBallPlaying:
		case kBallEscape:
		case kAttractMode:
			MoveBall();
			break;
	}

	MovePlayer(0);
	MovePlayer(1);

	VDPIdle();
}




// if we collide with an edge, there are three possibilities:
//	
//	1.  if gameState != demo and paddle == miss,
//			then continue through and gamestate <- ballescape
//
//	2.  if paddle = hit or gamestate == demo,
//			bounce

// if we go offscreen, bump score, reset ball to middle, and
//	ballstate <- kBallIdle





static void _MoveBall(AboutBoxGlobals* globalsParam, long dummy)
{
register AboutBoxGlobals* globals = globalsParam;		// more efficient

	if (globals->delta)	{
		short	intX, intY;
		Boolean	bounce = 0;
		Boolean	paddleHit = false;
		Boolean	leftEdge = false, rightEdge = false;
		
		globals->ballX += LongMultiply ( globals->ballDx, globals->delta);
		globals->ballY += LongMultiply ( globals->ballDy, globals->delta);
		
		intX = globals->ballX >> 16;
		intY = globals->ballY >> 16;
		
			//  top/bottom bounce always happens
		
		if ( (intY <= globals->ballRect[0] && globals->ballDy < 0)	||
			 ((intY+kBallHeight) >= globals->ballRect[2] && globals->ballDy > 0 ) )	{
			globals->ballDy = -globals->ballDy;
			bounce |= 1;
		}
		
			//  left/right bounce is more special
		
		if ( intX <= globals->ballRect[1] )	{
			leftEdge = true;
			paddleHit = CheckPaddleHit( 0 );
		}
		else
		if ( (intX+kBallWidth) >= globals->ballRect[3] )	{
			rightEdge = true;
			paddleHit = CheckPaddleHit( 1 );
		}

		if (leftEdge || rightEdge)	{
			if ( globals->gameState != kAttractMode && !paddleHit)
				globals->gameState = kBallEscape;
			else	{
				Boolean	flipDir = true;
				
				if (globals->ballDx < 0)	{			// make X absolute for calcs
					globals->ballDx = -globals->ballDx;
					flipDir = false;
				}
								
				globals->ballDx += kBallAccelX;			// X faster now
				if (globals->ballDx > kMaxBallDx)
					globals->ballDx = kMaxBallDx;		// pin X speed
				if (flipDir)
					globals->ballDx = -globals->ballDx;
				
				flipDir = false;
				if (globals->ballDy < 0)	{			// make Y absolute for calcs
					globals->ballDy = -globals->ballDy;
					flipDir = true;
				}
				
				globals->ballDy += kBallAccelY;			// Y faster now
				if (globals->ballDy > kMaxBallDy)
					globals->ballDy = kMaxBallDy;		// pin Y speed
				if (flipDir)
					globals->ballDy = -globals->ballDy;				
				
				bounce |= 2;
			}
		}
		
			// check for fall offscreen
			
		if (globals->gameState == kBallEscape)	{
			if (( intX + kBallWidth < 0 ) || (intX > kScreenWidth) )	{
				// scores
				globals->gameState = kBallIdle;
				globals->stateTimer = 3 * 60;	
				globals->ballX = kBallStartX << 16;
				globals->ballDx = -globals->ballDx;
			}
		}
		
			// finally, reposition the sprite
		
		MoveSprite ( globals->ballSprite, intX + kSpriteOffset, intY + kSpriteOffset );
		DrawSprite ( globals->ballSprite );
		
		if (bounce)
			PlayDBFX ( kAbout1Snd, 0, 0 );
	}
}



static void _MovePlayer(AboutBoxGlobals* globals, long playerLong)
{
short	playerNum = playerLong;					// prevent long-muls later on
Play