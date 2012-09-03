/*

	File:		Main.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<20>	 8/19/94	HEC		Pass param to DoSegaDemo and DoIntroStuff
		<19>	 7/26/94	HEC		PUTearDownServerTalk in MyExitToShell
		<18>	 7/24/94	HEC		StopVBLTask in DoQuit.
		<17>	 7/10/94	SAH		Conditionally compile calls to intro.c out for ROMDB build.
		<16>	 6/20/94	SAH		Updated to new StartupSega interface.
		<15>	 6/16/94	CMY		Set the sleep parameter to WaitNextEvent so we get called as
									much as possible.
		<14>	 6/12/94	CMY		When booting the sega, find out if we rebuilt the OS world due
									to an unstable reset.
		<13>	 6/11/94	CMY		Init RAM database after reset when in simulator.
		<12>	 6/10/94	CMY		Add sega reset code to simulator.
		<11>	  6/5/94	HEC		DoSegaDemo can be called externally now
		<10>	 5/31/94	SAH		Only do InitTestRamDatabase if not ROMDB.
		 <9>	 5/27/94	SAH		Moved database init stuff to CreateDatabases.c
		 <8>	 5/27/94	KON		Add mail cursor. Add bitmaps until we're out of 'SEGA'
									resources.
		 <7>	 5/27/94	SAH		Added some more stuff to the rom database. Cleaned it up a
									little.
		 <6>	 5/27/94	HEC		Fixed some ROM DB problems.
		 <5>	 5/27/94	KON		Added some mail stuff. There's some problem with the result FIFO
									because of the ROM DB. Help Ted!
		 <4>	 5/26/94	HEC		Added ROM heap dump command.
		 <3>	 5/26/94	BET		Change net startup structure
		 <2>	 5/26/94	KON		Update to use userIdentification instead of userName.
		<11>	 5/25/94	SAH		Added Yerga's heap viewer.
		<10>	 5/25/94	HEC		new memory manager
		 <9>	 5/24/94	SAH		Look at text globals through the dispatcher.
		 <8>	 5/23/94	SAH		Added cursor 1. Removed some of the bitmaps.
		 <7>	 5/23/94	KON		Main now sets the current user ID before adding results to the
									database.
		 <6>	 5/20/94	SAH		Added the backdrop to the database. Fixed Roscoe's name.
		 <5>	 5/20/94	SAH		Added cursor install to the database.
		 <4>	 5/19/94	HEC		Name changes!
		 <3>	 5/18/94	KON		Added init call for events and for fonts.
		 <2>	 5/17/94	KON		Update user names to test button controls.

	To Do:
*/

#include "SegaOS.h"		/* Shannon's O/S dispatcher (DJ's PatchDB is already using it) 5/1/94 -dj */
#include "SegaBM.h"
#include <Types.h>

#include <math.h>               /* uses sin(), M_PI */
#include "Main.h"
#include "BigEasy.h"
#include "SegaText.h"
#include "Events.h"
#include "simDebugger.h"
#include "Intro.h"
#include "SegaTextPriv.h"
#include "Exceptions.h"
#include "PhysicalLayer.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Globals
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rect gTileRect = {20, 10, 470, 600 };
short gDrawGrid;
exitToShellProcPtr gExitToShellAddress;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// extern Globals
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern short	gNormal;
extern Rect	gWindowSize;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// prototypes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ToggleSega( void );
void Write1BitSega( void );
void Write4BitSega( void );
void ToggleGrid( void );
void WriteFont( void );

void DoAbout(void);
void InitVars(void);
void Bootstrap(int resetFlag);
void InitTestRamDatabase ( void );
void CleanUp( void );
void MyExitToShell( void );

static void DoMenuSetup( void );
static void KillEds( void );

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// extern functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void OpenWindow( void );
extern void DoCloseWindow( void );
extern void GenerateSegaScreen( short *screen, Ptr patternTable, GWorldPtr dst );
extern void SaveROMHeap( void );

void DoAbout( void )
{	
	PutUpMessage( 0, "\pSega Demo" );
}

void DoSegaDemoStart( void )
{
	DoSegaDemo( false );
}

void DoSegaDemo( Boolean justPlayedGame )
{
Rect		myRect;

	SetRect( &myRect, 0, 0, 320, 224 );
#ifndef ROMDB
	DoIntroStuff( justPlayedGame, FrontWindow(), &myRect );
#endif
}

static void DoMenuSetup( void )
{
	/* BRAIN DAMAGE: this gWindowCount crap is all shannon's fault */
extern short gWindowCount;

//	if( FrontWindow() )
	if( gWindowCount )
	{
		SetMenuItem( 23, mDisable, 0, 0, nil );
		SetMenuItem( 24, mEnable, 0, 0, nil );
	}
	else
	{
		SetMenuItem( 23, mEnable, 0, 0, nil );	
		SetMenuItem( 24, mDisable, 0, 0, nil );
	}
/*** Menu 3: Edit Menu ***/
	if( 0 )
		SetMenuItem( 1, mEnable, 0, 0, nil );
	else
		SetMenuItem( 1, mDisable, 0, 0, nil );

/*** Menu 4: Sound Menu ***/
}

void InitVars(void)
{
	SetAboutProc( &DoAbout );
	SetMenuSetupProc( &DoMenuSetup );
}

void TileVertical( short windowCount, Rect *tileRect )
{
short tileWidth; 
short tileHeight;
WindowPeek	curWindow;
short 		xpos, ypos;
short		iii;

	if( windowCount == 0 )
		return;
		
	tileWidth = tileRect->right - tileRect->left;
	tileHeight = tileRect->bottom - tileRect->top - 22*windowCount;

	curWindow = (WindowPeek)FrontWindow();

	xpos = tileRect->left;
	ypos = tileRect->top + 20;	// title bar height!

	for( iii = windowCount; iii > 0; iii-- )
	{
// For vertical tiling....
		SizeWindow( (GrafPort*)curWindow, tileWidth, tileHeight/windowCount, false );
		MoveWindow( (GrafPort*)curWindow, xpos, ypos, false );
		SetPort( (GrafPort*)curWindow );
		InvalRect( &((WindowPtr)curWindow)->portRect );
		curWindow = curWindow->nextWindow;
		ypos = ypos+tileHeight/windowCount+22;	// take title bar into acount!
	}

/* For checkerboard tiling....
	SizeWindow( curWindow, tileWidth/2, tileHeight/2, false );
	MoveWindow( curWindow, xpos, ypos, false );
	SetPort( curWindow );
	InvalRect( &((WindowPtr)curWindow)->portRect );
	curWindow = curWindow->nextWindow;
	xpos = xpos+tileWidth/2+10;
	if( xpos > tileWidth )
	{
		xpos = tileRect->left;
		ypos += tileHeight/2 + 10;
	}
*/
}

void TileWindows( void )
{
short		windowCount;
WindowPeek	curWindow;
GrafPort	*savePort;

	GetPort( &savePort );

	windowCount = 0;
	if( curWindow = (WindowRecord*)FrontWindow() )
	{
		windowCount++;
		while( curWindow = curWindow->nextWindow )
			windowCount++;
	}
	
	TileVertical( windowCount, &gTileRect );

	SetPort( savePort );
}

void ToggleSega( void )
{
	gNormal++;
	InvalRect( &qd.thePort->portRect );
}

void ToggleGrid( void )
{
	gDrawGrid = (gDrawGrid+1) & 1;
	InvalRect( &qd.thePort->portRect );
}

void Write1BitSega( void )
{
Point		myLoc;
SFReply	reply;
OSErr	myErr;
short	fRefNum;
long		count;
Ptr		baseAddr1;
PixMapHandle	myWPixels;
WindowGlobalsPtr wGlobals;

	SetPt( &myLoc, 0, 0 );
	SFPutFile( myLoc, "\pSave 1-bit Sega Data As:", "\pData.bin", nil, &reply );
	if( !reply.good )
		DebugStr("\pBroke in Write1BitSega" );

	myErr = Create( reply.fName, reply.vRefNum, '????', 'TEXT' );
				if( myErr )
					DebugStr("\pBroke in Write1BitSega" );
	myErr = FSOpen( reply.fName, reply.vRefNum, &fRefNum );
				if( myErr )
					DebugStr("\pBroke in Write1BitSega" );

	count = SEGAVisibleWidth*SEGAHeight*8;

	myErr = SetEOF( fRefNum, count );
	myErr |= SetFPos( fRefNum, fsFromStart, 0 );
	
	wGlobals = GetWindowGlobals( FrontWindow() );
	myWPixels = GetGWorldPixMap( wGlobals->wGWorld );
	LockPixels( myWPixels );
	baseAddr1 = GetPixBaseAddr( myWPixels );
	myErr |= FSWrite( fRefNum, &count, baseAddr1 );
	myErr |= FSClose( fRefNum );
				if( myErr )
					DebugStr("\pBroke in Write1BitSega" );
	UnlockPixels( myWPixels );

}

void WriteFont( void )
{
Point		myLoc;
SFReply	reply;
OSErr	myErr;
short	fRefNum;
long		count;
Ptr		baseAddr1;
PixMapHandle	myWPixels;
WindowGlobalsPtr wGlobals;
TextGlobals	* textGlobals;

	SetPt( &myLoc, 0, 0 );
	SFPutFile( myLoc, "\pWrite Font Data As:", "\pFont.bin", nil, &reply );
	if( !reply.good )
		DebugStr("\pBroke in WriteFont" );

	myErr = Create( reply.fName, reply.vRefNum, '????', 'TEXT' );
				if( myErr )
					DebugStr("\pBroke in WriteFont" );
	myErr = FSOpen( reply.fName, reply.vRefNum, &fRefNum );
				if( myErr )
					DebugStr("\pBroke in WriteFont" );

	count = sizeof( SegaFontTable );
	myErr = SetEOF( fRefNum, count );
	myErr |= SetFPos( fRefNum, fsFromStart, 0 );
	
	/* BRAIN DAMAGE: This is gross but so is accessing the global. If more people need it we should */
	/* add a call */
	if ( GetManagerGlobals ( kTextManager, (Ptr *) &textGlobals, 0L ) == noErr )
		{
		myErr |= FSWrite( fRefNum, &count, (Ptr)textGlobals->theFont );
		}
		
	myErr |= FSClose( fRefNum );
				if( myErr )
					DebugStr("\pBroke in WriteFont" );
}

void Write4BitSega( void )
{
Point		myLoc;
SFReply	reply;
OSErr	myErr;
short	fRefNum;
long		count;
Ptr		baseAddr4;
PixMapHandle	myPixels;
WindowGlobalsPtr wGlobals;
CTabHandle	myCTable;
RGBColor		*rgbPtr;
short		segaColorOut[16];

	SetPt( &myLoc, 0, 0 );
	SFPutFile( myLoc, "\pSave 4-bit Sega Data As:", "\pData4Bt.bin", nil, &reply );
	if( !reply.good )
		DebugStr("\pBroke in Write4BitSega" );

	myErr = Create( reply.fName, reply.vRefNum, '????', 'TEXT' );
				if( myErr )
					DebugStr("\pBroke in Write4BitSega" );
	myErr = FSOpen( reply.fName, reply.vRefNum, &fRefNum );
				if( myErr )
					DebugStr("\pBroke in Write4BitSega" );

	count = SEGAVisibleWidth*SEGAHeight*8*4 + 16*2;

	myErr = SetEOF( fRefNum, count );
	myErr |= SetFPos( fRefNum, fsFromStart, 0 );
	wGlobals = GetWindowGlobals( FrontWindow() );
	myPixels = GetGWorldPixMap( wGlobals->segaGWorld );
	LockPixels( myPixels );
//
// write 16 entry color table in 3-3-3 format	0000 bbb0 ggg0 rrr0
//	
	myCTable = (**myPixels).pmTable;
	{
	short	iii;
	short	red, green, blue;
	
		for( iii = 0; iii < 16; iii++ )
		{
			rgbPtr = &(**myCTable).ctTable[iii].rgb;
			segaColorOut[iii] = ((rgbPtr->blue & 0xE000)>>4) | ((rgbPtr->green & 0xE000)>>8) | ((rgbPtr->red & 0xE000)>>12);
		}
	}
	
	count = 16*2;
	myErr |= FSWrite( fRefNum, &count, segaColorOut );

	baseAddr4 = GetPixBaseAddr( myPixels );
	count = SEGAVisibleWidth*SEGAHeight*8*4 ;
	myErr |= FSWrite( fRefNum, &count, baseAddr4 );
	myErr |= FSClose( fRefNum );
				if( myErr )
					DebugStr("\pBroke in Write4BitSega" );
	UnlockPixels( myPixels );

}

void Bootstrap(int resetFlag)
{
	Boolean rebuiltOS;
	/* boot the sega os. this creates and initializes the ROM database also */
	rebuiltOS = StartupSega( false );

#ifndef	ROMDB
	/* add the test data to the perm database if it's a power-on or the OS is being rebuilt */
	if (!resetFlag || rebuiltOS)
		InitTestRamDatabase ();
#endif
	
	if (resetFlag == false) {
	//
	// Use resource manager to stub out data base for now
	//
		CreateResFile( "\pResDB" );
		OpenResFile( "\pResDB" );
		
		InitVars();
	
		gExitToShellAddress = (exitToShellProcPtr) GetToolTrapAddress( 0xA9F4 );
		SetToolTrapAddress( (long)MyExitToShell, 0xA9F4 );		//exit to shell
	
	/*** Menu 2 ***/
		InstallMenu("\pFile",(IFP)pnull,0);
		InstallItem("\pOpen Picture/O",(IFP)OpenWindow,23);
		InstallItem("\pClose/W",(IFP)DoCloseWindow,-24);
		InstallItem("\p(-",(IFP)pnull,0);
		InstallItem("\pSave ROM Image…",(IFP)SaveROMHeap,0);
		InstallItem("\pQuit/Q",(IFP)DoQuit,0);
	
	/*** Menu 3 ***/
		InstallEditMenu((IFP)pnull,(IFP)pnull,(IFP)pnull,(IFP)pnull,(IFP)pnull);
	
	/*** Menu 4 ***/
		InstallMenu("\pSEGA",(IFP)pnull,0);
		InstallItem("\pView Sega Screen/t",(IFP)ToggleSega,-24);
		InstallItem("\pWrite 1-bit Sega Data",(IFP)Write1BitSega,-24);
		InstallItem("\pWrite 4-bit Sega Data",(IFP)Write4BitSega,-24);
		InstallItem("\pToggle Grid/g",(IFP)ToggleGrid,-24);
	
	/*** Do User Interface ***/
		InstallMenu("\pUser Interface",(IFP)pnull,0);
		InstallItem("\pShow Sega Demo/1",(IFP)DoSegaDemoStart,-24);
		InstallItem("\pSave Font",(IFP)WriteFont,-24);
	
	/*** Install Debugging Menus etc. ***/
	#ifdef DEBUG
		InitDebugger();
	#endif
		
		SetSleepTime(1);	// Give events to the simulator thread as fast as possible
		KillEds();
	}
}


void MyExitToShell( void )
{
//	TDisconnectRequest();
	StopVBLTask();
	PUTearDownServerTalk();
	SetToolTrapAddress( (long)gExitToShellAddress, 0xA9F4 );		//put exit to shell back
	(*gExitToShellAddress)();
}

void KillEds( void )
{
	EnDisEdits(0,0,0,0,0);
}


void CleanUp( void )
{
//	TDisconnectRequest();
}