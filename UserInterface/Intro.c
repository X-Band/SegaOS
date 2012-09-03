// #define MESSAGES 1

/*
	File:		Intro.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

	   <172>	 9/15/94	HEC		Re-added CheckWipeCard() to boot code.
	   <171>	 8/29/94	ADS		Eliminate double-dialogs at restart
	   <170>	 8/28/94	JOE		Play the flippin A sound for the intro animation
	   <169>	 8/25/94	CMY		Put up a background before showing box-state and deferred
									dialogs.
	   <168>	 8/23/94	SAH		(For Teddis). Fix crasher on return button after playing game by
									initing the stack to the choose player screen.
	   <167>	 8/21/94	KON		Cosmetic cleanup of bra in dama ge.
	   <166>	 8/21/94	KON		DrawSpriteInFront now takes a hueShift.
	   <165>	 8/20/94	JBH		Make intro animation appear in its finished state for a half
									second if it is not finished when user clicks.
	   <164>	 8/19/94	JBH		Make intro animation sprites truly offscreen, not visible in
									upper-left corner.
	   <163>	 8/19/94	ADS		Fix Teddis' changes to work THREADED
	   <162>	 8/19/94	HEC		Go to main screen if game was just played.
	   <161>	 8/19/94	ADS		Try turning on threading (when we're USEDISK)
	   <160>	 8/17/94	HEC		SetCurUserID to kDBIDErrorValue at intro screen to prevent
									deferred dialogs coming up for the wrong user.  We should not
									base any action on user id before choosing a player.
	   <159>	 8/12/94	KON		Add DebugSelectors.h to support USEDISK define.
	   <158>	  8/9/94	JBH		Make first X draw actually paint across like the rest (call
									DrawSprite to put them in the list offscreen)
	   <157>	  8/9/94	JOE		Start the Intro Animation tune
	   <156>	  8/9/94	JBH		Tweaked intro animation to Joey's liking.
	   <155>	  8/7/94	JBH		Made more ways of drawing the BAND text in the intro animation.
									Added the ™ symbol.
	   <154>	  8/6/94	CMY		Made "Hit..." come in after first animation cycle.
	   <153>	  8/4/94	CMY		Added "Hit Any Key To Continue..." to intro animation
	   <152>	  8/4/94	ADS		Remove unused PlayerInfo from main loop stack
	   <151>	  8/1/94	CMY		Fix a bug where the user exited the intro animation in one of
									the first frames.
	   <150>	  8/1/94	CMY		Ripped out the Intro animation and replaced it with new stuff.
	   <149>	 7/31/94	CMY		Cache the intro animation graphics to make it go really fast
									before we rip it all out.
	   <148>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
	   <147>	 7/28/94	KON		Clear NetRegister if there on reboot after checking for box
									state errors.
	   <146>	 7/26/94	CMY		Updated to pass VBL flag to SetupClut.
	   <145>	 7/25/94	HEC		#ifdef VBL out of ROM build
	   <144>	 7/25/94	HEC		Call StartVBLTask and StopVBLTask.
	   <143>	 7/20/94	JOE		Attach sounds to intro animation
	   <142>	 7/19/94	CMY		Added static to the intro animation
	   <141>	 7/19/94	KON		Turn of threading so everyone quits complaining about the speed.
	   <140>	 7/18/94	SGR		added LED animation to intro screen
	   <139>	 7/17/94	CMY		Updated into animation to use new decompressor and remove delays
	   <138>	 7/16/94	CMY		Added intro animation.
	   <137>	 7/15/94	ADS		Strip out PeerToPeer stuff and a shitload of #includes
	   <136>	 7/14/94	CMY		Don't stomp on screen's state when going to a deeper screen
									whose preflight fails.
	   <135>	 7/13/94	KON		Remove extra call to NetRegisterDone.
	   <134>	 7/13/94	DJ		moved retrycount into if(err) in connection
	   <133>	 7/13/94	KON		Removed reference to kWaitingForGame box state and use
									IsBoxNetRegistered instead.
	   <132>	 7/12/94	JOE		a personification mgr call changed.
	   <131>	 7/12/94	DJ		took out extra GTSession_Shutdown
	   <130>	 7/12/94	DJ		chat
	   <129>	 7/11/94	JBH		Improved chat keyboard handling loop.
	   <128>	 7/11/94	JBH		Added real call to chat keyboard, but it's commented out.
	   <127>	 7/11/94	DJ		tweaked timeouts in chat
	   <126>	 7/11/94	DJ		made chat use reliable gametalk
	   <125>	 7/10/94	JOE		pulled StartDBBGM(); now happens automatically for screens
	   <124>	  7/9/94	DJ		added Chat comm layer
	   <123>	  7/8/94	DJ		timeouts to TListen
	   <122>	  7/8/94	SAH		Shannon is a bigger fucker.
	   <121>	  7/8/94	SAH		Shannon is a fucker.
	   <120>	  7/7/94	KON		Make peer-to-peer time outs and retry constants part of the
									DBConstants database.
	   <119>	  7/7/94	HEC		Removed unnecessary DelayTicks
	   <118>	  7/7/94	KON		Remove extra NetRegisterDone and make slave time out. Add
									deferred dialog handling to main event loop.
	   <117>	  7/6/94	SAH		We really don't need to trash the whole OS when the peer to peer
									connect fails...
	   <116>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
	   <115>	  7/6/94	KON		Allow OS to continue to run after failed peer to peer connect.
	   <114>	  7/6/94	KON		Add some peer-to-peer status strings.
	   <113>	  7/6/94	SAH		Added as many gratuitous PCheckError's as possible.
	   <112>	  7/6/94	HEC		Fixed redialing bug which caused us to fail on good results.
									Removed PInit's which were hosing us.
	   <111>	  7/5/94	HEC		Fixed some peer-to-peer connection bugs.
	   <110>	  7/5/94	KON		Added some error cases to peer-to-peer connect.
	   <109>	  7/5/94	JOE		include DBTypes.h
	   <108>	  7/5/94	JOE		personification mgr update
	   <107>	  7/4/94	ADS		New interface for screen control
	   <106>	  7/4/94	SAH		Include TextUtils.h
	   <105>	  7/3/94	HEC		Added kUsePeerProtocol to PListen
	   <104>	  7/2/94	SAH		Killed the kFindOpponencCount global.
	   <103>	  7/2/94	SAH		Don't update the address book stuff if we died during a peer
									connect.
	   <102>	  7/2/94	KON		Remove CES stuff, use meaningful defines for POpen, fix
									lingering black screen on restart bug.
	   <101>	  7/2/94	HEC		Make this compile -- backed out ADS' <97> #includes reduction.
	   <100>	  7/2/94	HEC		Now tell POpen if we're calling server or peer.
		<99>	  7/1/94	KON		Remove call to dialog time out since that's handled as part of
									the dialog data structure.
		<98>	  7/1/94	SAH		Made compile for USEDISK.
		<97>	  7/1/94	ADS		cut way back on included files
		<96>	 6/30/94	ADS		Fix PEERCONNECT for UseDisk sessions to work
		<95>	 6/28/94	BET		Update for new TOpen TListen return values
		<94>	 6/28/94	SAH		Killed Results.h
		<93>	 6/28/94	KON		Added check for case when resetting while waiting for a game and
									then powering up with different cartridge installed.
		<92>	 6/21/94	BET		Slathering of fixes for Fred GameTalk.
		<91>	 6/21/94	SAH		Yet more dialog fixes.
		<90>	 6/21/94	SAH		(KON) Fixed delay of peer connect dialogs
		<89>	 6/21/94	KON		Add AnOpponentHasBeenFound dialog with a three second dialog
									time out delay.
		<88>	 6/21/94	SAH		Made slave wait for a connection. Added game talk init stuff
									(commented out for CES).
		<87>	 6/20/94	DJ		added peer-to-peer gametalk (#define PLAYLAMEGAME), error
									checking in peer connections, etc.
		<86>	 6/20/94	SGR		Added LED animation to top level screen
		<85>	 6/20/94	DJ		fixed PListent and POpen for new error model where errors are
									returned and noErr indicates success
		<84>	 6/20/94	KON		Added DeferredDialogManager call.
		<83>	 6/20/94	SGR		added LED animations associated with every screen
		<82>	 6/18/94	GOD		Call NetRegisterDone after the server connect and before the
									peer connect. Call the new GamePatch PlayCurrentGame rather than
									the empty PlayGame.
		<81>	 6/18/94	BET		Looks like headers changed since the test build.
		<80>	 6/18/94	BET		Forgot to comment out DOPEERCONNECT and MODEM for the masses.
		<79>	 6/18/94	BET		(Really CMY) Clear kModemTryingToConnect after peer connection.
		<78>	 6/18/94	KON		Update for new NGP list calls.
		<77>	 6/17/94	KON		Add screen reset.
		<76>	 6/17/94	KON		Add support for NetRegister manager.
		<75>	 6/17/94	KON		Add NGP dialog and dialog that we disconnected while waiting for
									an opponent.
		<74>	 6/16/94	CMY		Remove call to AllowUserEvent.
		<73>	 6/16/94	BET		Revert Ted's changes.
		<72>	 6/16/94	SAH		Added some pretty lights.
		<71>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		<70>	 6/15/94	HEC		SegaOSIdle takes a param now
		<69>	 6/15/94	KON		Add dialog screens to describe disasters from last game play.
									Add AddressBook update to peer connect.
		<68>	 6/15/94	DJ		minor jizzling with the jozzle nozzle
		<67>	 6/14/94	DJ		you can turn off peer connect by not defining DOPEERCONNECT
		<66>	 6/13/94	DJ		passing opponent phone number to POpen if #define MODEM
		<65>	 6/13/94	KON		Make selection parameter to screens a long.
		<64>	 6/12/94	DJ		added TNetIdles in DoPeerDataExchange and DisplayPeerInfo
		<63>	 6/12/94	DJ		calling DoPeerConnection for real
		<62>	 6/12/94	DJ		fixing DoPeerConnection
		<61>	 6/12/94	SAH		Use the happy fun backdrop manager.
		<60>	 6/12/94	DJ		trying to checkin over top of chris
		<60>	 6/12/94	DJ		DoPeerConnection now knows who is master and who is slave from
									the opponentPhoneNumber (is '0' if you are waiting, is non-zero
									if you are to dial)
		<59>	 6/11/94	KON		Remember cursor positions across screens.
		<58>	 6/11/94	CMY		When the user has hit ESC in simulator and we're waiting for our
									thread to be killed, we now call YieldToAnyThread instead of
									OSIdle.
		<57>	 6/11/94	KON		Add stack based screen navigation.
		<56>	 6/11/94	CMY		Reenable threading.
		<55>	 6/11/94	CMY		Fix the non-threaded build.
		<54>	 6/10/94	KON		Added support for screen Preflight and Reset calls.
		<53>	 6/10/94	CMY		Added sega reset code to simulator.
		<52>	 6/10/94	BET		Add physical layer control here.
		<51>	 6/10/94	KON		Moved all FindOpponent stuff to a separate file, FindOpponent.c.
		<50>	 6/10/94	KON		Updated to use new screen dispatcher.
		<49>	  6/9/94	BET		Change TOpen TListen interfaces
		<48>	  6/6/94	DJ		DrawPage does WaitForButton, so removed from DoOpponent
		<47>	  6/6/94	DJ		Removed double background draw prior to connect
		<46>	  6/6/94	DJ		Reading simulated news from the string database
		<45>	  6/5/94	SAH		Added hack to make mortal kombat skip the simulated server
									connect.
		<44>	  6/5/94	DJ		made connect screens use strings and new modem icon.  #ifdefed
									news out
		<43>	  6/5/94	HEC		Enhancement button does fighter screen for demo
		<42>	  6/5/94	CMY		Fade the initial screen, not the top level screen.
		<41>	  6/5/94	CMY		Add fading to the first screen.
		<40>	  6/5/94	CMY		Save my ass from the demon hordes!
		<39>	  6/5/94	BET		Add configuration string to TOpen and TListen
		<38>	  6/4/94	DJ		added util to check for button up, etc
		<37>	  6/4/94	KON		Killed dead functions from last user demo. Use generic
									ProcessDitl routine for top level screen.
		<36>	  6/4/94	SAH		Killed a PsyqHalt.
		<34>	  6/4/94	KON		Change name of kAboutText to kPersonificationAboutText.
		<33>	  6/4/94	KON		Adjust personification layout.
		<32>	  6/4/94	KON		Personification getting better. Checkin hell fixed. The night is
									still young!
		<31>	  6/4/94	KON		Added prelim personification stuff and merged because of
									revision/checkout hell.
		<30>	  6/3/94	CMY		Put Options screen strings in the DB
		<29>	  6/3/94	DJ		updating to revised DrawDBGraphic call
		<28>	  6/3/94	DJ		added a fake news screen
		<27>	  6/3/94	CMY		Added a cheap implementation of the options screen. Spec has yet
									to be finalized.
		<26>	  6/3/94	KON		Made top level screen use the database.
		<26>	  6/3/94	KON		Made top level screen use the database.
		<25>	  6/3/94	KON		Add cursorX and Y offset.
		<24>	  6/2/94	KON		Use database for choose your player for four player screen.
		<23>	  6/2/94	KON		Made top level a little more real.
		<22>	  6/2/94	KON		Made HistoryView the Ranking screen. Sortof lame...
		<21>	  6/2/94	KON		Made high level screen a little more real.
		<20>	  6/2/94	SAH		Play some tunes.
		<19>	  6/1/94	SAH		Improved font coloring.
		<18>	  6/1/94	DJ		Added server connect progress screens
		<17>	 5/31/94	KON		Fixed text centering in text buttons.
		<16>	 5/31/94	SAH		Killed SegaMisc.h. PsyqHalt before TOpen for sega.
		<15>	 5/31/94	DJ		added second displayMail() to view incoming mail after server
									connect.
		<14>	 5/31/94	HEC		Changed name of load patch routine
		<13>	 5/30/94	SAH		Made DoInterfaceTopLevelScreen load strings from the database.
		<12>	 5/29/94	KON		Updated to use graphics database.
		<11>	 5/28/94	KON		Changed all NewMemory( false, ...) to NewMemory( kTemp, .... ).
		<10>	 5/28/94	SAH		Removed YieldToAnyThread calls. Killed the Welcome Screen.
		 <9>	 5/28/94	KON		Don't wait for so many keypresses!
		 <8>	 5/28/94	KON		Pass controller reads into DITL proc.
		 <7>	 5/27/94	DJ		removed SegaComm.h
		 <6>	 5/27/94	KON		Made DITL item positions pixel based instead of character based.
		 <5>	 5/27/94	KON		Added a mail screen, flush events before calling any of that
									StandardFile nastieness if SIMULATOR.
		 <4>	 5/26/94	BET		Update for managerized Disk Transport.
		 <3>	 5/26/94	BET		Remove obsolete refs to GameTalk.h
		 <2>	 5/26/94	KON		Update to use userIdentification rather than userName.
		<19>	 5/25/94	KON		Include calls to GetData and to initialize the get proc for
									network access.
		<18>	 5/25/94	HEC		new memory manager
		<17>	 5/24/94	SAH		Killed SendAByte and ReceiveAByte in favour of SegaComm.h.
		<16>	 5/23/94	SAH		Added DoInterfaceTopLevelScreen.
		<15>	 5/21/94	SAH		Updated to latest Text button data format.
		<14>	 5/20/94	SAH		Updated to latest text button stuff. Made names shadowed. Added
									new background graphic.
		<13>	 5/20/94	HEC		Changing WaitVBL to DelayTicks
		<12>	 5/20/94	SAH		Added support for cursors.
		<11>	 5/19/94	BET		Add a comment about threading with serial stuff
		<10>	 5/18/94	KON		Update to use text buttons and multiple fonts.
		 <9>	 5/18/94	HEC		Over to new pattern manager
		 <8>	 5/18/94	BET		Reflect GameTalk API changes
		 <7>	 5/17/94	KON		Update to use SetFontColors.
		 <6>	 5/17/94	KON		Added some calls to the control/DITL manager. Took out all the
									thread yields and made OSIdle call YieldToThread instead.
		 <5>	 5/16/94	SAH		Added some more thread yields.
		 <4>	 5/12/94	SAH		Sprinkled the world with SegaOSIdle so we can do animations.
		 <3>	 5/12/94	KON		Results are now sent through the sendQ.
		 <2>	 5/12/94	KON		Send the send Q on server connect.

	To Do:
*/


#include "Intro.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "SegaIn.h"
#include "Threads.h"
#include "GameID.h"
#include "GameDB.h"
#include "PatchDB.h"
#include "Time.h"
#include "SegaTraps.h"
#include "PlayerDB.h"
#include "DITLItemSetup.h"
#include "Screens.h"
#include "DialogMgr.h"
#include "BoxSer.h"
#include "PrettyLights.h"
#include "NetRegister.h"
#include "ModemMisc.h"
#include "DeferredDialogMgr.h"
#include "Secrets.h"
#include "PeerConnect.h"
#include "Pattern.h"
#include "Sprites.h"
#include "GraphicsDB.h"
#include "Backdrops.h"
#include "Events.h"
#include "SegaSound.h"
#include "Decompress.h"
#include "SegaVDP.h"
#include "Utils.h"
#include "Math.h"
#include "DebugSelectors.h"
#include "SmartCard.h"

#ifdef SIMULATOR
	#include <SetJmp.h>
	#include <Events.h>
	#include "Exceptions.h"
#endif


// Uncomment this to do peer to peer connection.  The following #defines are effective only if
// doing a peer connect.
//
//	(Andy sez:  I think this right, for testing package loading from disk)

#if (defined(SIMULATOR) && defined(USEDISK))
//	#define DOPEERCONNECT
#else
	#define DOPEERCONNECT
#endif



//
// UnComment this baby to build a threaded version!!
// The threaded version won't run with the debugger because ThinkC sucks!!!
// if you have a better explanation for the illegal instruction, let me know!!
//
#ifdef SIMULATOR
#ifdef USEDISK
	#define THREADED
#else
	// use at your own risk!!!
//	#define THREADED
#endif
#endif


#ifdef SIMULATOR
	extern Boolean gSimulatorResetRequest;
	extern jmp_buf	resetJumpBuffer;
#endif


void DoEnterScreen( void );
void DoWaitForOpponentScreen( void );
void DoOpponentHasArrivedScreen( void );
void DoGameOverScreen( void );
void SendReceiveString( char *sendString, char *receiveString );
pascal void *SegaThread( void *threadParameter );

static void	ShowIntroAnimation( void );

void DoServerConnectingScreen(void);
void DoServerConnectedScreen(void);
void DoServerDownloadingMailScreen(void);



//
// Global to this file
//

#ifdef THREADED

short				threadCounter = 0;
ThreadID			theThreadIDs[4];
Boolean				gJustPlayed;		// cheezy!

#endif

#ifdef SIMULATOR

void DoIntroStuff( Boolean justPlayedGame, WindowPtr thisWindow, Rect *windowRect )

#else


void DoIntroStuff( Boolean justPlayedGame )

#endif
{

#ifdef THREADED

	if( threadCounter != 0 )
		return;
		
	gJustPlayed = justPlayedGame;			// this is probably wrong....
		
	threadCounter = 0;
	NewThread( kCooperativeThread, SegaThread, (void *) theThreadIDs[threadCounter], 0x6000, kCreateIfNeeded, 0L,
		&theThreadIDs[threadCounter]);
	threadCounter++;
}


pascal void *SegaThread( void *threadParameter )
{
	Boolean justPlayedGame = gJustPlayed; 	// this is probably wrong....
#endif

short			player1;
short			player2;
Boolean			firstTime = true;
long			gameID;
long			refCon;
DBID			screenID;
DBID			oldScreenID;
short			command;
long			lastSelection;
DBID			screenStack[10];
long			selectionStack[10];
short			stackIndex = 0;
long			boxState;
short 			thisOpponentIndex;
userIdentification	*theUserID;
AnimationRef animation ;
ScreenParams	scrnParams;

#if defined(SIMULATOR) || defined(__SERVER__)
	StartVBLTask();
#endif
	animation = (AnimationRef) 0 ;

	SetupVDP();
	InitPatternManager ( 1504 );
	
	/* who's playing? */
	SetCurFont( kMainFont );
	
	/* Put up a background for the dialogs */
	SetBackdropID(  kBackdrop, true, 0 );
	
	SetRemoteModemTryingToConnectState(false);

	CheckWipeCard();

	if (justPlayedGame)
	{
		selectionStack[0] = 0;
		screenStack[0] = kChoosePlayerScreen;
		screenID = kMainScreen;
		stackIndex = 1;
		ResetBoxState( kAllStates );	//mark box as clean
		goto RESTART;
	}
	
	SetCurUserID(kDBIDErrorValue);
	
//
// Check if we were in a weird state on last shutdown and tell user
//

	boxState = GetBoxState();
	if( boxState != 0 )
	{
		if( boxState & kServerConnect )
			DoDialog( kDiedDuringServerConnectionDialog );

		if( boxState & kPeerConnect )
		{
			DoDialog( kDiedDuringPeerConnectionDialog );
		}

		if( IsBoxNetRegistered() )
		{
			if( GetGameID() == GetCurGameIDSRAM() )
			{
				DoDialog( kDiedWaitingForGame );
			}
			else
			{
			//
			// we died while waiting for a game, and now have a different game cartridge
			// installed.
			//
				if(! (boxState & kPeerConnect) )
					DoDialog( kDiedWaitingForGameDeRegister );
			}
			NetRegisterDone();
		}
		
		if( boxState & kDuringLocalCallWaiting )
			DoDialog( kDiedDuringLocalCallWaiting );

		if( boxState & kDuringRemoteCallWaiting )
			DoDialog( kDiedDuringRemoteCallWaiting );
	}

//
// save the current game ID in SRAM so we know it for the next boot
//
	SetCurGameIDSRAM( GetGameID() );
	SetLastBoxState( boxState );	//only the server clears this baby!
	ResetBoxState( kAllStates );	//mark box as clean

//
// Do any dialogs deferred to boot time
//
	ShowDefDialogs( kDDBoot );
	
//
// Check if the current game is supported
//
	gameID = GetGameID();
	if( GetNGPListGamePatchVersion( gameID ) == kPatchDoesNotExist )
		DoDialog( kGameNotInNGPList );
		
//
// Show the intro animation and spin until the impatient little spud hits a button
//
	screenID = kIntroAnimationScreen ;
	animation = SetLEDScreenAnimation ( screenID, animation ) ;

	ShowIntroAnimation();
	
	screenID = kChoosePlayerScreen;

RESTART:

	oldScreenID = screenID;
	SetCurFont( kMainFont );	

//
// Main event loop
//

// This is all made uglier by the fact that "result" is overloaded to be
// both "command" and "selection state".  We do this by placing command
// in the lower 8 bits and sel-state can be anywhere in all 32. (it's probably
// in the upper 24).  Logic would be much simpler if they were seperated.

	lastSelection = 0;
	scrnParams.selection = 0;
	scrnParams.initValue = 0;
	SetupScreen( screenID, &refCon, &scrnParams );
	animation = SetLEDScreenAnimation ( screenID, animation ) ;

	while( 1 )
	{
	short	dummy;
	long	result = 0;
	unsigned char	newScreenIndex;
	long			forceScreenNum;
	long			preflightReject;
	long			newScreenOrPop;
		
	