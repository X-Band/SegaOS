#ifdef SIMULATOR
#define MESSAGES 1
//#define DEBUG 1
#endif


/*
	File:		PeerConnect.c

	Contains:	xxx put contents here xxx

	Written by:	Bass Master Classic Programming Team

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<72>	 8/28/94	JOE		Add an LED animation to chat
		<71>	 8/28/94	SAH		Dispose the os GTSession before we play the game so that we
									don't fragment the game heap.
		<70>	 8/27/94	SAH		Let PlayCurrentGame reset the kPeerConnectFailed box state to
									simplify error handling.
		<69>	 8/27/94	JBH		Remove InstallReliableAddress from allocations in game heap.
		<69>	 8/27/94	JBH		Remove InstallReliableAddress from allocations in game heap.
		<68>	 8/27/94	HEC		Use GamePatchFlags const.
		<67>	 8/27/94	SAH		Send opponent verification errors to the server. Handle these
									errors properly for the slave.
		<66>	 8/27/94	HEC		Master should only delay if he gets remote call waiting.
		<65>	 8/26/94	HEC		Master now delays before redialing slave on call waiting.
		<64>	 8/26/94	HEC		Better reconnection reliability.  Slave call waiting voice calls
									now always put up blow off count down dialog.
		<63>	 8/25/94	JBH		Move handles over 2 pixels in chat screen.
		<62>	 8/25/94	HEC		Detect voice calls in slave call waiting cases.  Some smartcard
									recrediting cases.
		<61>	 8/24/94	SAH		Use 19 bit packet format in chat.
		<60>	 8/24/94	HEC		Call waiting option is now user-based.
		<59>	 8/24/94	HEC		More in the saga of game dialogs.
		<58>	 8/24/94	HEC		Removed fucked vbl call in DoPeerRestoreOS. Other tweaks.
		<57>	 8/23/94	HEC		Lose the string param in DoPeerDialog.
		<56>	 8/23/94	BET		Update to new interfaces and structs.
		<55>	 8/22/94	HEC		Better everything!
		<54>	 8/21/94	HEC		Mucking with call waiting to use new timer dialog template.
									ConnectToPeer takes retries and timeout params.
		<53>	 8/19/94	HEC		Removed PeerReconnectFailed(). Added VDPIdle's to various
									graphics routines. Made Connecting text stay on screen.
		<52>	 8/19/94	HEC		Removed PeerConnectTimeout
		<51>	 8/17/94	HEC		Unfortunately, a host of errors.  Was prematurely turning off
									remote modem calling state.  Was making error connect for master
									think it was still registered and a slave!
		<50>	 8/14/94	HEC		Keep listening for master if Plisten errors on return to Main
									screen.
		<49>	 8/14/94	HEC		If we're a slave, return to OS if we think its a voice call or
									there's an error and continue waiting.  Bracket
									PeerConnectionDropped's with GTShutdown/Init's.
		<48>	 8/12/94	JOE		get opponent's name the new way
		<47>	 8/11/94	JBH		Moved sparks and xband logo into sprite manager.
		<46>	 8/10/94	HEC		Reorder resetboxstates.
		<45>	 8/10/94	JBH		Updated chat to new graphics (finally!)
		<44>	 8/10/94	KON		Set peer connect box state when dialing, reset if there is an
									error coming out of the game or at end of game (in endgame.c).
		<43>	 8/10/94	HEC		Fix several Call Waiting bugs.  Don't start game after
									reconnecting from call waiting, just return.  Do resync properly
									in exchange data after reconnect.
		<42>	  8/9/94	CMY		Added a flag to Show/Hide Backdrop.
		<41>	  8/9/94	HEC		Added static function to display text messages.  Rewrote slave
									connection logic.
		<40>	  8/8/94	SAH		Kill some sega debugging code.
		<39>	  8/8/94	HEC		Removed READCONTROLS_ONVBL #ifdef around ControllerVBL().
									Checked for error case from ConnectToPeer.
		<38>	  8/8/94	HEC		hoops. Accidentally blew away SetupPeerGraphics and
									RemotePeerGraphics during a compare.
		<37>	  8/8/94	KON		Remove ClearBackdrop.
		<36>	  8/7/94	HEC		More game patch dialog stuff.  Broke out setup and tear down of
									user interface into routines.
		<35>	  8/6/94	JBH		Converted Chat to use GTLooseSessions.
		<34>	  8/6/94	HEC		Fleshed out call waiting logic. Dropped connection reconnect
									works. Added DoPeerDialog.
		<33>	  8/5/94	SAH		More better (from Teddis). Sega debugging code.
		<32>	  8/4/94	HEC		Tons of happy shit.
		<31>	  8/3/94	SAH		Made _PeerConnectionDropped return an error.
		<30>	  8/3/94	HEC		Happy teddis.
		<29>	  8/3/94	SAH		Pass the gametalk session into exchange peer data.
		<28>	  8/2/94	HEC		New personification transfer stuff.
		<27>	  8/1/94	HEC		Caught ADS bug where we were installing our selectors under the
									wrong manager.
		<26>	  8/1/94	KON		Remove getData dependency.
		<25>	  8/1/94	HEC		DBConstants.  Removed crappy SIMULATOR #ifdefs.  Added new
									routines.
		<24>	  8/1/94	SAH		Fixed pattern memory trashes and other peer to peer fuckups.
		<23>	 7/31/94	HEC		Added TSetTransportHold around send of personification info to
									peer (from 160 down to 94 ticks!).   Added timer for
									send/receive time.
		<22>	 7/31/94	SAH		Changed the establish synch timeout for chat to something more
									reasonable.
		<21>	 7/31/94	HEC		Streamlining. Sparks broken out.  Data exchange broken out.
									Faster data exchange to versus screen.  Other tweaks. Major
									stuff ahead.
		<20>	 7/30/94	HEC		Added call waiting dialogs in chat.  Made SegaOSIdle not to call
									network code (while in chat).
		<19>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<18>	 7/28/94	SAH		Made chat a little more stable.
		<17>	 7/27/94	HEC		Disable call waiting on peer connect if player said so.
		<13>	 7/27/94	HEC		Disable call waiting on peer connect if user sets this option.
		<16>	 7/27/94	KON		Draw the XBAND logo instead of that shitty looking modem. Who
									would ever make a product that looked like that?
		<15>	 7/26/94	SAH		Fixed some error recover/session closing problems with chat.
		<14>	 7/26/94	CMY		Turned "include" into <include> to speed up compile.
		<13>	 7/26/94	SAH		Fixed a bug in chat, added some debugging code. Added a call to
									GT to close the connection.
		<12>	 7/24/94	JBH		Added sparks for master when dialing the slave peer.
		<11>	 7/24/94	CMY		Updated to latest keyboard interfaces.
		<10>	 7/24/94	SAH		MOre lametalk stuff.
		 <9>	 7/23/94	HEC		Made DisplayPeerInfo really display the versus screen after
									peer-to-peer data exchange.
		 <8>	 7/23/94	HEC		Made DisplayPeerInfo really display the Versus screen after peer
									data exchange.
		 <7>	 7/22/94	DJ		added error return from DoPeerDataExchange
		 <6>	 7/22/94	SAH		Updated to new lametalk.
		 <5>	 7/21/94	JOE		Added flags param to SegaBoxText()
		 <4>	 7/18/94	DJ		added peer validation
		 <3>	 7/16/94	DJ		cleaned up chat and peerconnect
		 <2>	 7/15/94	HEC		Use peer protocol now for peer connections.
		 <1>	 7/15/94	ADS		Stripped from Intro.c

	To Do:
*/

#include "SegaScrn.h"
#include "GraphicsDB.h"
#include "Backdrops.h"
#include "SegaText.h"
#include "VDP.h"
#include "BoxSer.h"
#include "Opponent.h"
#include "NetRegister.h"
#include "ModemMisc.h"
#include "DialogMgr.h"
#include "Heaps.h"
#include "PhysicalLayer.h"
#include "Reliability.h"
#include "GamePatch.h"
#include "DBConstants.h"
#include "StringDB.h"
#include "Time.h"
#include "NetErrors.h"
#include "TransportLayer.h"
#include "Messages.h"
#include "Personification.h"
#include "DBTypes.h"
#include "MailView.h"
#include "TextUtls.h"
#include "Events.h"
#include "Keyboard.h"
#include "SegaIn.h"
#include "Screens.h"
#include "PathProcs.h"
#include "PeerConnectPriv.h"
#include "PatchDB.h"
#include "EndGame.h"
#include "GTSendData.h"
#include "VBLManager.h"
#include "Controls.h"
#include "SegaSound.h"
#include "News.h"
#include "PrettyLights.h"

#ifdef SIMULATOR
#include <StdLib.h>
#include <StdIO.h>
#endif

#ifndef SIMULATOR
PeerConnGlobals peer;
#endif

//
//  DISPATCHED ROUTINES
//
void			_DoPeerConnection( void );
OSErr			_ConnectToPeer( Boolean master, Boolean setupListen, long retries, long timeout );
OSErr			_DoMasterPeerConnect( long pmodemFlags, long retries, long timeout );
OSErr			_DoSlavePeerConnect( long pmodemFlags, long retries, long timeout );
void			_DisplayPeerInfo( GTSession * session );
void			_PeerConnectionDropped( OSErr err );
void			_DoPeerRestoreOS( void );
OSErr			_DoExchangePeerData( GTSession *gtSession, long * opponentTag );
OSErr			_DoPeerDialog( DBID dialogID, long timeout );
void			_Chat( GTSession *session, Boolean master );

static void SetupPeerGraphics( void );
static void DisposePeerGraphics( void );
static void DisplayPeerText( short stringID );
static void EraseText( void );
static void _PeerVBLProc( void );

long
_PeerConnectControl ( short command, long data )
{
long				offset;
short				error;
PeerConnGlobals*	globals;
short				ii;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(peer,offset);
			
			error = AllocateGlobalSpace ( kPeerConnectManager, offset, sizeof(PeerConnGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create peer connect globals" );
			if ( error != noErr )
				{
				return error;
				}
				
			/* install our selectors */
			SetDispatchedFunction ( kDoPeerConnection,		kPeerConnectManager,	_DoPeerConnection );
			SetDispatchedFunction ( kConnectToPeer,			kPeerConnectManager,	_ConnectToPeer );
			SetDispatchedFunction ( kDisplayPeerInfo,		kPeerConnectManager,	_DisplayPeerInfo );
			SetDispatchedFunction ( kDoSlavePeerConnect,	kPeerConnectManager,	_DoSlavePeerConnect );
			SetDispatchedFunction ( kDoMasterPeerConnect,	kPeerConnectManager,	_DoMasterPeerConnect );
			SetDispatchedFunction ( kPeerConnectionDropped,	kPeerConnectManager,	_PeerConnectionDropped );
			SetDispatchedFunction ( kDoPeerRestoreOS,		kPeerConnectManager,	_DoPeerRestoreOS );
			SetDispatchedFunction ( kDoExchangePeerData,	kPeerConnectManager,	_DoExchangePeerData );
			SetDispatchedFunction ( kDoPeerDialog,			kPeerConnectManager,	_DoPeerDialog );
			SetDispatchedFunction ( kChat,					kPeerConnectManager,	_Chat );
			SetDispatchedFunction ( kPeerStartVBL,			kPeerConnectManager,	_PeerStartVBL );
			SetDispatchedFunction ( kPeerStopVBL,			kPeerConnectManager,	_PeerStopVBL );
			SetDispatchedFunction ( kPeerVBLHandler,		kPeerConnectManager,	_PeerVBLHandler );
			break;
		
		case kSoftInialize:
			/* init our globals here */
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}

void _DoPeerConnection( void )
{
GTSession *			gtSession;
Boolean				master;
OSErr				err;
long				temp;
short				dialog;
long				flags;
long				opponentTag;

	SetBoxState(kPeerConnect);	//We're connecting to our buddy!
	ResetBoxState(kListeningForPeerConnection);	// Disable SegaOSIdle from checking for ring

	gtSession = (GTSession *)NewMemory ( kPerm, sizeof(GTSession) );

	dialog = kPeerToPeerConnectFailed;
	DBGetConstant( kMaxLostPeerConnectionsConst, &temp );
	REFGLOBAL(peer,maxlostconnections) = temp;
	REFGLOBAL(peer,lostconnections) = 0;
	REFGLOBAL(peer,displayUserInterface) = 1;
	REFGLOBAL(peer,vblInstalled) = 0;

	master = IsBoxMaster();
	SetupPeerGraphics();
	
	err = ConnectToPeer( master, false, master?-1:0, -1 );

	SetRemoteModemTryingToConnectState( false );
	DisposeXBandLogoSparks( REFGLOBAL(peer, xbandLogo) );

	// Deal with physical connection error
	if ( err )
	{
		if ( master )
			goto credittoken;

		if ( err == kNoAnswer )
		{
			dialog = kVoiceCallNotModemDialog;
		}
		else
		{
			dialog = kPeerConnectProbWaitMoreDialog;
			SetBoxState(kListeningForPeerConnection);	// wait for our bud!
			ConnectToPeer( false, true, -1, -1 );			// wait for opponent's call
		}
		goto bail;
	}

	DisplayPeerText( kConnectedToPeerString );

	GTSession_Init ( gtSession, master );
	err = DoExchangePeerData( gtSession, &opponentTag );
	GTSession_Shutdown(gtSession);
	
	// Clear our graphics shit and put up the versus screen.
	EraseText();
	DisposePeerGraphics();

	// Deal with communication error
	if ( err )
	{
		if (err == kOpponentVerificationFailed)
			{
			GameResult *	results;
			
			dialog = kWrongPeerDialog;
			
			/* post error game results to the server */
			results = NewMemoryClear ( kTemp, sizeof(