/*
	File:		GamePatch.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<35>	 8/29/94	ATM		Added #ifndef __SERVER__ stuff to make it build under UNIX.
		<34>	 8/27/94	SAH		Real sinkotron interface/selectors. Bumped size of game dispatch
									table.
		<33>	 8/26/94	HEC		BIG ONE: Added lots of PModem routines to the game dispatcher to
									make OSCheckLine callable during game play.
		<32>	 8/24/94	HEC		Added game dialog idle and countdown timer.
		<31>	 8/23/94	HEC		Changed GPDialogRec
		<30>	 8/22/94	SAH		More general low mems.
		<29>	 8/22/94	SAH		Added Stinkotron.
		<28>	 8/22/94	HEC		Fucker.
		<27>	 8/22/94	HEC		Update game dispatcher to new vectors.
		<26>	 8/21/94	ADS		New lomem layout
		<25>	 8/21/94	JOE		Christen the Game Patch Flags section with kMasterOnLeftFlag
		<24>	  8/7/94	HEC		More to GPDialogRec.  Added dialog commands.
		<23>	  8/6/94	ADS		New interface for controller types & reading them
		<22>	  8/6/94	HEC		Filled out GPDialogRec.
		<21>	  8/6/94	ADS		Added controller init call
		<20>	  8/4/94	SAH		Changed minFreeGames to freeTimeOver. Made OSNewMemory and
									OSDisposeMemory game dispatcher calls to stop assembly selector
									hell. Killed ResumeGame. Added GPReportErrors.
		<19>	  8/1/94	HEC		Removed gameOwnsDram.
		<18>	 7/31/94	HEC		Added gameOwnsDram game param.  Added kOSCheckLine.
		<17>	 7/29/94	SAH		Made the game dispatcher macro smaller.
		<16>	 7/26/94	HEC		Added HandleCallWaiting support.
		<15>	 7/26/94	SAH		Added OSGTCloseSessionSynch.
		<14>	 7/25/94	SAH		Added minChargedGames to the GamePatchParamBlock.
		<13>	 7/25/94	ADS		Added ReadControllers
		<12>	 7/24/94	SAH		Happy happy gametalk fun.
		<11>	 7/15/94	SAH		New GamePatchParamBlock.
		<10>	 7/12/94	SAH		Added CreateGameDispatcher.
		 <9>	 7/12/94	HEC		Defined PModem selectors/protos for GameTalk via the game os.
		 <8>	  7/5/94	SAH		Updated game dispatch macro to use latest low mems.
		 <7>	  7/2/94	SAH		Added minMappedRom.
		 <6>	 6/28/94	SAH		Moved the game patch globals to globals.h.
		 <5>	 6/21/94	SAH		GameTalk is here.
		 <4>	 6/20/94	SAH		Added affectsRanking to the game param block.
		 <3>	 6/18/94	GOD		Initial working version.
		 <2>	 6/17/94	SAH		Latest game patch stuff.
		 <1>	 6/16/94	SAH		first checked in

	To Do:
*/


#ifndef __GamePatch__
#define __GamePatch__


#ifndef __GT__
#include "GT.h"
#endif

#ifndef __SERVER__
#ifndef __GTSYNCOTRON__
#include "GTSyncOTron.h"
#endif

#include "Controls.h"
#endif	/*__SERVER__*/

/*
 *  These are the flags defined for the PatchDescriptor's flags field
 */

#define	kMasterOnLeftFlag	0x00000001


/*
*	Here are some game patch structures
*/


#ifndef __SERVER__
typedef
struct GamePatchParamBlock
{
	// this stuff is passed into the game patch by the os
	Boolean			isMaster;
	Boolean			affectsRanking;
	short			freeTimeOver;
	short			frameDelay;
	long			minMappedRam;		// size of sram from beginning that must be mapped in
	long			minMappedRom;		// size of rom from beginning that must be mapped in
	long			maxTimePlay;
	short			maxGamesPlay;
	long			gameFlags;
	GTSession *		commSession;
	long			osReserved[ 10 ];
	
	// this stuff is set by the game patch for the os to look at
	GameResult		results;
} GamePatchParamBlock;
#endif /*__SERVER__*/


typedef
struct GameDispatchTable
{
	unsigned long	function;
	unsigned long	globalsPtr;
} GameDispatchTable;


/* game play flags */
#define	kRemoteGame		0x1			// set if a local/remote game, clear if lcoal only
#define	kColdStart		0x2			// set if cold boot (show copyright screens)


/*
*	Game Patch interface
*/

typedef long ( *GameProc ) ( long command, long data1, long data2 );

enum
{
	kGPInitGamePatch,
	kGPStartGamePatch,
	kGPDisplayMessage,
	kGPStopGame,
	kGPKillGame,
	kGPReportError,
	
	/* last game patch command */
	kLastGameCommand
};

// Game Patch Errors to OS
enum
{
	kGPNoErr = 0,
	kGPCannotDrawDialogErr = -1
};

// Game Patch Dialogs

#define kGPDShowTimer	0x00000001L

enum {
	kGPDDisplayCmd,			// put up dialog
	kGPDIdleCmd,			// dialog idle (does not have to be supported)
	kGPDRemoveCmd			// tear down dialog
};

typedef struct {
	unsigned short command;	// What should dialog do?
	unsigned short flags;	// flags
	char 		*message;	// text to display
	char		*respA;		// Button A -- 1st response text (nil if not used)
	char		*respB;		// Button B -- 2nd response text (nil if not used)
	char		*respC;		// Button C -- 3nd response text (nil if not used)
	long		countdown;	// Number of remaining ticks until timeout (0 if no timer)
	long		refCon;		// for game patch use
} GPDialogRec;

// OS Game call selectors

/* total number of game selectors available */
#define	kNumGameSelectors	64

enum
{
	/* general game stuff */
	kOSHandleGameError,
	kOSGameOver,
	
	/* basic os stuff */
	kOSNewMemory,
	kOSDisposeMemory,
	kOSDelayTicks,
	
	/* hardware stuff */
	kOSSetSafeRomSrc,
	kOSSetSafeRamSrc,
	kOSSetVectorTableAddr,
	kOSSetVector,
	kOSSetLEDs,
	
	/* PModem */
	kOSReadSerialByte,
	kOSWriteSerialByte,
	kOSReceiveBufferAvail,
	kOSTransmitBufferFree,
	kOSCheckLine,
	kOSDetectLineNoise,
	kOSCheckCarrier,
	kOSListenToLine,
	kOSSetTimerTicks,
	kOSTimerExpired,
	kOSToneMode,
	kOSReadXRAM,
	kOSReadYRAM,
	kOSWriteXRAM,
	kOSWriteYRAM,
	
	/* gametalk */
	kOSGTSSetPacketFormat,
	kOSGTSSetRamRomOffset,
	kOSGTSessionSetLatency,
	kOSGTSessionPrefillFifo,
	kOSGTSessionEstablishSynch,
	kOSGTSErrorRecover,
	kOSGTSCloseSessionSynch,
	kOSGTSFlushInput,
	kOSGTSessionValidateControl,
	kOSGTSessionExchangeCommands,
	kOSGTSDoCommand,
	kOSGTSDoResend,
	kOSGTSResendFromFrame,
	kOSGTModemInit,
	kOSGTModemGetModemError,
	kOSGTModemClearFifo,
	kOSGTModemClockInByte,
	kOSGTModemClockOutByte,
	kOSGTModemAbleToSend,
	kOSGTModemSendBytes,
	kOSGTModemCheckLine,
	
	/* controller, should probably be in "hardware stuff"	*/
	kOSInitControllers,
	kOSReadControllers,
	
	/* stinkotron */
	kOSGTSyncotron,
	kOSGTMasterCalculateLatency,
	kOSGTSlaveCalculateLatency,
	kOSGTSyncoReadModemVBL,
	kOSGTSyncronizeVBLs,
	kOSGTSyncronizeMasterLeave,
	kOSGTSyncronizeSlaveLeave,
	kOSGTSyncoTronVBLHandler,
	
	/* keep this one */
	kOSLastFunction
};


/* macro to dispatch through the game dispatcher */
/* the cool thing about this one is it automatically does the rom address translation! */
/* ==BRAIN DAMAGE== This will eventually need to be ROM/RAM savvy! */
#ifdef SIMULATOR
#define	CallGameOSFunction( sel )		{ 0x7000 + sel, 0x2078, GameGlobal(gameDispatcher0), 0x4e90 }
#else
#define	CallGameOSFunction( sel )		{ 0x7000 + sel, 0x2078, GameGlobal(gameDispatcher0), 0xD1F8, GameGlobal(romOffset), 0x4e90 }
#endif

#define	kGameGlobalsStackSize	( 32 * 8 )

#ifndef __SERVER__
/* 
*	os calls for the game patch
*/

	/* general game calls */
OSErr	OSHandleGameError ( OSErr err ) = 
	CallGameOSFunction ( kOSHandleGameError );

OSErr	OSGameOver ( void ) = 
	CallGameOSFunction ( kOSGameOver );

	/* some basic os calls */
void	OSDisposeMemory ( void *p ) =
	CallGameOSFunction( kOSDisposeMemory );

void *	OSNewMemory ( short perm, long size ) =
	CallGameOSFunction( kOSNewMemory );

void	OSDelayTicks( long ticks, short caller) =
	CallGameOSFunction( kOSDelayTicks );


	/* hardware calls */
void OSSetSafeRomSrc ( Ptr start, Ptr end ) = 
	CallGameOSFunction( kOSSetSafeRomSrc );

void OSSetSafeRamSrc ( Ptr start, Ptr end ) = 
	CallGameOSFunction( kOSSetSafeRamSrc );

void OSSetVectorTblAddr ( Ptr address ) = 
	CallGameOSFunction( kOSSetVectorTableAddr );

void OSSetVector ( long vector, Ptr address ) = 
	CallGameOSFunction( kOSSetVector );

void OSSetLEDs ( unsigned char data ) = 
	CallGameOSFunction( kOSSetLEDs );

	/* PModem */
OSErr 	OSReadSerialByte (unsigned char *byte) =
	CallGameOSFunction(kOSReadSerialByte);
	
OSErr 	OSWriteSerialByte (unsigned char byte) =
	CallGameOSFunction(kOSWriteSerialByte);
	
OSErr 	OSTransmitBufferFree (void) =
	CallGameOSFunction(kOSTransmitBufferFree);

short	OSReceiveBufferAvail(void) =
	CallGameOSFunction(kOSReceiveBufferAvail);

OSErr	OSCheckLine(void) =
	CallGameOSFunction(kOSCheckLine);

OSErr	OSDetectLineNoise(void) =
	CallGameOSFunction(kOSDetectLineNoise);

OSErr	OSCheckCarrier(void) =
	CallGameOSFunction(kOSCheckCarrier);

short	OSListenToLine(void) =
	CallGameOSFunction(kOSListenToLine);

void	OSWriteXRAM(long addr, long xcr, long val) =
	CallGameOSFunction(kOSWriteXRAM);

void	OSWriteYRAM(long addr, long ycr, long val) =
	CallGameOSFunction(kOSWriteYRAM);

long	OSReadXRAM(long addr, long xcr) =
	CallGameOSFunction(kOSReadXRAM);

long	OSReadYRAM(long addr, long ycr) =
	CallGameOSFunction(kOSReadYRAM);

void	OSSetTimerTicks(short t, short ticks) =
	CallGameOSFunction(kOSSetTimerTicks);

short	OSTimerExpired(short t) =
	CallGameOSFunction(kOSTimerExpired);

void	OSToneMode(void) =
	CallGameOSFunction(kOSToneMode);

	/* gametalk */
Err		OSGTSession_SetGTPacketFormat ( GTSession * session, short packetFormat ) = 
	CallGameOSFunction( kOSGTSSetPacketFormat );

Err		OSGTSession_SetRamRomOffsets ( GTSession * session, long ramOffset, long romOffset ) = 
	CallGameOSFunction( kOSGTSSetRamRomOffset );

Err 	OSGTSession_SetLatency ( GTSession * session, short frameLatency ) = 
	CallGameOSFunction( kOSGTSessionSetLatency );

Err		OSGTSession_PrefillFifo ( GTSession * session, unsigned long fillData ) = 
	CallGameOSFunction( kOSGTSessionPrefillFifo );

Err		OSGTSession_EstablishSynch ( GTSession * session, long timeout ) = 
	CallGameOSFunction( kOSGTSessionEstablishSynch );

Err		OSGTSession_ErrorRecover ( GTSession * session, Err error, long timeout ) = 
	CallGameOSFunction( kOSGTSErrorRecover );

Err		OSGTSession_CloseSessionSynch ( GTSession * session, long timeout ) = 
	CallGameOSFunction( kOSGTSCloseSessionSynch );

Err 	OSGTSession_FlushInput(GTSession *session) = 
	CallGameOSFunction( kOSGTSFlushInput );

long 	OSGTSession_ReadController ( GTSession * session, long *localData, long * remoteData ) = 
	CallThroughVector ( GameGlobal(gtReadController) );

Err 	OSGTSession_SendController ( GTSession * session, long data ) = 
	CallThroughVector ( GameGlobal(gtSendController) );

Err 	OSGTSyncOTron_Synch ( SyncState * state ) = 
	CallGameOSFunction( kOSGTSyncotron );

	/* controller, should probably be in "hardware stuff"	*/
	
ControllerType 	OSInitControllers( void ) = 
	CallGameOSFunction( kOSInitControllers );

void 	OSReadAllControllers( ControllerType type, short* results, short flags ) = 
	CallGameOSFunction( kOSReadControllers );


#ifdef SIMULATOR
#define	OSGTSession_ReadHardwareModem(s)	(noErr)
#else
Err 	OSGTSession_ReadHardwareModem ( GTSession * session ) = 
	CallThroughVector ( GameGlobal(gtReadModem) );
#endif

#endif /*__SERVER__*/

#endif
