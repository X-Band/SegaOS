/*
	File:		BoxSer.h

	Contains:	Protected access to box identification globals

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<39>	 8/25/94	BET		Add multiple NetErrorRecords for X25 and 800.
		<38>	 8/25/94	KON		Added glue for ClearNetErrors.
		<37>	 8/25/94	HEC		Added kSlaveTimedOut and kPeerConnectFailed box states.
		<36>	 8/24/94	HEC		Call waiting and keyboard options are now user-based.
		<35>	 8/22/94	ADS		Add hidden box serial # calls
		<34>	 8/21/94	KON		Add second local access number.
		<33>	 8/20/94	JOE		add Get/SetLocalUserCustomROMClutID
		<32>	 8/20/94	CMY		Added Get/Set AcceptChallengeOption.
		<31>	 8/18/94	HEC		Update UseBoxProblemToken prototype.
		<30>	 8/18/94	ADS		Hide gBoxID globals
		<29>	 8/17/94	JOE		now the game value funcs deal with strings
		<28>	 8/14/94	HEC		Add description of box states.
		<27>	 8/13/94	BET		Add GetNetErrors
		<26>	 8/12/94	HEC		added kUsedSmartCardForConnectState box state.
		<25>	  8/7/94	JOE		added SetGameValue()
		<24>	  8/7/94	JOE		added getgamevalue() for the Versus screen
		<23>	  8/4/94	SAH		Added calls to get and set the game error resuts.
		<22>	  8/3/94	HEC		Added new options accessor routines.
		<21>	  8/1/94	HEC		New routines.  Added kGameEnded box state.
		<20>	 7/27/94	KON		Add set and check network dial again flag to support 800 number.
		<19>	 7/20/94	DJ		added GetCompetitionResults
		<18>	 7/19/94	DJ		set and get of box hometown
		<17>	 7/16/94	dwh		(BET) Fix an include capitalization.
		<16>	 7/15/94	SAH		New calls for GameResults.
		<15>	 7/13/94	DJ		added kListeningForPeerConnection to fix peer listening for conn
		<14>	 7/13/94	KON		Removed kWaitingForGame box flag.
		<13>	 7/10/94	HEC		Added DBSetConstants
		<12>	  7/7/94	HEC		Added DBGetConstant and DBAddConstant.
		<11>	  7/2/94	SAH		Added BoxID calls.
		<10>	  7/2/94	DJ		added gameresultflags to _SetGameScores and added GameResult
									struct
		 <9>	  7/1/94	HEC		Added SetBoxMaster function.
		 <8>	 6/28/94	SAH		Made new box serial number structure.
		 <7>	 6/27/94	KON		Added get and set curGameIDSRAM.
		 <6>	 6/20/94	KON		Add IsBoxMaster call.
		 <5>	 6/17/94	SAH		Added the kPlayingGame state.
		 <4>	 6/17/94	KON		Add game score stuff.
		 <3>	 6/14/94	KON		Add box state stuff.
		 <2>	 6/13/94	KON		Managerize.

	To Do:
*/


#ifndef __BoxSer__
#define __BoxSer__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifndef __Database__
#include "DataBase.h"
#endif

//
// Box states
//
#define kAllStates						0xFFFFFFFF
#define kServerConnect					0x00000001
#define kPeerConnect					0x00000002
#define kListeningForPeerConnection		0x00000004
#define kInitiatingGame					0x00000008
#define kPlayingGame					0x00000010
#define kDuringLocalCallWaiting			0x00000020
#define kDuringRemoteCallWaiting		0x00000040
//#define kGameEnded					0x00000080	// unused
#define kUsedSmartCardForConnectState	0x00000100
#define kSlaveTimedOut					0x00000200
#define kPeerConnectFailed				0x00000400

/*	BOX STATE DESCRIPTIONS

	kServerConnect
	
	kPeerConnect
	
	kListeningForPeerConnection
	
	  SET:		when waiting for a master peer to call us.
	  CLEARED:	when successfull connect has been established.
	  USE:		Do different stuff if we're waiting for peer to call.
	  			Many actions are restricted during this time.

	kInitiatingGame
	
	kPlayingGame
	
	kDuringLocalCallWaiting
	kDuringRemoteCallWaiting
	
	  SET:		whenever the appropriate call-waiting tone is detected.
	  CLEARED:	when call waiting is resolved or at boot.
	  USE:		Puts box in mode of trying to recover from call waiting.
	  			Alert the user of this fact at next boot.

	kGameEnded
	
	kUsedSmartCardForConnectState
	
	  SET:		when a valid smartcard has been used to connect to the network in FindOpponent.c.
	  CLEARED:	at the completion (?) of the first game if played, or mail-only connect.
	  USE:		Tells box whether or not we need to credit a card when problem occur. We credit
				a card by setting it's token (in gBoxID) to the problem token.
*/

#define kRedialTheNetwork		1
#define kDontRedialTheNetwork	0

/*
 * type of net error recovery for GetNetErrors
 */
 
enum {
	kCurrentErrorType,
	k800Errors,
	kX25Errors
	};
	
#ifndef __SERVER__

BoxSerialNumber * GetBoxSerialNumber( void ) =
	CallDispatchedFunction( kGetBoxSerialNumber );

void SetBoxSerialNumber( BoxSerialNumber * theSerialNumber ) =
	CallDispatchedFunction( kSetBoxSerialNumber );

void GetHiddenBoxSerialNumbers( BoxSerialNumber * s1, BoxSerialNumber * s2 ) =
	CallDispatchedFunction( kGetHiddenBoxSerialNumbers );

Hometown *GetBoxHometown( void ) =
	CallDispatchedFunction( kGetBoxHometown );

void	SetBoxHometown( Hometown hometown ) =
	CallDispatchedFunction( kSetBoxHometown );

void SetBoxState( long bitsToSet ) =
	CallDispatchedFunction( kSetBoxState );

void ResetBoxState( long bitsToSet ) =
	CallDispatchedFunction( kResetBoxState );

long GetBoxState( void ) =
	CallDispatchedFunction( kGetBoxState );

void SetLastBoxState( long bitsToSet ) =
	CallDispatchedFunction( kSetLastBoxState );

void ResetLastBoxState( long bitsToSet ) =
	CallDispatchedFunction( kResetLastBoxState );

long GetLastBoxState( void ) =
	CallDispatchedFunction( kGetLastBoxState );

void SetCompetitionResults( GameResult * results ) =
	CallDispatchedFunction( kSetCompetitionResults );

GameResult *GetCompetitionResults( void ) = 
	CallDispatchedFunction( kGetCompetitionResults );

void SetGameErrorResults( GameResult * results ) =
	CallDispatchedFunction( kSetGameErrorResults );

GameResult *GetGameErrorResults( void ) = 
	CallDispatchedFunction( kGetGameErrorResults );

void ClearGameResults( void ) =
	CallDispatchedFunction( kClearGameResults );

char * GetLocalGameValue( void ) =
	CallDispatchedFunction( kGetLocalGameValue );

void SetLocalGameValue( char * valstring ) =
	CallDispatchedFunction( kSetLocalGameValue );

char * GetOppGameValue( void ) =
	CallDispatchedFunction( kGetOppGameValue );

void SetOppGameValue( char * valstring ) =
	CallDispatchedFunction( kSetOppGameValue );

void UpdateGameResults( GameResult * results ) =
	CallDispatchedFunction( kUpdateGameResults );

void GetGameWinsLosses( long * wins, long * losses ) =
	CallDispatchedFunction( kGetGameWinsLosses );

Boolean IsBoxMaster( void ) =
	CallDispatchedFunction( kIsBoxMaster );

void SetBoxMaster( Boolean master ) =
	CallDispatchedFunction( kSetBoxMaster );

void SetCurGameIDSRAM( long gameID ) =
	CallDispatchedFunction( kSetCurGameID );

long GetCurGameIDSRAM( void ) =
	CallDispatchedFunction( kGetCurGameID );

OSErr CheckBoxIDGlobals( void ) =
	CallDispatchedFunction( kCheckBoxIDGlobals );

void InitBoxIDGlobals( void ) =
	CallDispatchedFunction( kInitBoxIDGlobals );

void ChangedBoxIDGlobals( void ) =
	CallDispatchedFunction( kChangedBoxIDGlobals );

DBErr DBAddConstant( DBID theID, long data ) =
	CallDispatchedFunction( kDBAddConstant );

DBErr DBGetConstant( DBID theID, long *data ) =
	CallDispatchedFunction( kDBGetConstant );

void DBSetConstants( short const, DBID *idList, long *constList ) =
	CallDispatchedFunction( kDBSetConstants );

void SetDialNetworkAgainFlag( short flagValue ) =
	CallDispatchedFunction( kSetDialNetworkAgainFlag );

short CheckDialNetworkAgainFlag( void ) =
	CallDispatchedFunction( kCheckDialNetworkAgainFlag );

void SetBoxXBandCard( XBANDCard *card ) =
	CallDispatchedFunction( kSetBoxXBandCard );

XBANDCard * GetBoxXBandCard( void ) =
	CallDispatchedFunction( kGetBoxXBandCard );

XBANDCard * GetBoxLastCard( void ) =
	CallDispatchedFunction( kGetBoxLastCard );

void SetBoxMagicToken( long token ) =
	CallDispatchedFunction( kSetBoxMagicToken );

void SetBoxProblemToken( long token ) =
	CallDispatchedFunction( kSetBoxProblemToken );

long GetBoxProblemToken( void ) =
	CallDispatchedFunction( kGetBoxProblemToken );

void UseBoxProblemToken( unsigned char problem ) =
	CallDispatchedFunction( kUseBoxProblemToken );

void SetBoxValidationToken( long token ) =
	CallDispatchedFunction( kSetBoxValidationToken );

long GetBoxValidationToken( void ) =
	CallDispatchedFunction( kGetBoxValidationToken );

void SetIMovedOption( Boolean moved ) =
	CallDispatchedFunction( kSetIMovedOption );

void SetQwertyKeyboardOption( short userID, Boolean qwerty ) =
	CallDispatchedFunction( kSetQwertyKeyboardOption );

void SetCallWaitingOption( short userID, Boolean enable ) =
	CallDispatchedFunction( kSetCallWaitingOption );

void SetAcceptChallengesOption( short userID, Boolean accept ) =
	CallDispatchedFunction( kSetAcceptChallengesOption );

Boolean GetIMovedOption( void ) =
	CallDispatchedFunction( kGetIMovedOption );

Boolean GetQwertyKeyboardOption( short userID ) =
	CallDispatchedFunction( kGetQwertyKeyboardOption );

Boolean GetCallWaitingOption( short userID ) =
	CallDispatchedFunction( kGetCallWaitingOption );

Boolean GetAcceptChallengesOption( short userID ) =
	CallDispatchedFunction( kGetAcceptChallengesOption );

NetErrorRecord * GetNetErrors ( short errorType ) =
	CallDispatchedFunction( kGetNetErrors );
	
phoneNumber *GetBoxPhoneNumber( void ) = 
	CallDispatchedFunction( kGetBoxPhoneNumber );

void SetBoxPhoneNumber( phoneNumber *thePhoneNumber ) = 
	CallDispatchedFunction( kSetBoxPhoneNumber );

void GetLocalAccessPhoneNumber( phoneNumber **local1, phoneNumber **local2 ) =
	CallDispatchedFunction( kGetLocalAccessPhoneNumber );

void SetLocalAccessPhoneNumber( phoneNumber *localPhone1, phoneNumber *localPhone2 ) = 
	CallDispatchedFunction( kSetLocalAccessPhoneNumber );

phoneNumber *Get800PhoneNumber( void ) = 
	CallDispatchedFunction( kGet800PhoneNumber );

UserName * GetLocalUserName ( unsigned char userID ) = 
	CallDispatchedFunction( kGetLocalUserName );

void SetLocalUserName ( unsigned char userID, UserName name ) = 
	CallDispatchedFunction( kSetLocalUserName );

DBID GetLocalUserROMIconID ( unsigned char userID ) = 
	CallDispatchedFunction( kGetLocalUserROMIconID );

void SetLocalUserROMIconID ( unsigned char userID, DBID ROMIconID ) = 
	CallDispatchedFunction( kSetLocalUserROMIconID );

DBID GetLocalUserCustomROMClutID ( unsigned char userID ) =
	CallDispatchedFunction( kGetLocalUserCustomROMClutID );

void SetLocalUserCustomROMClutID ( unsigned char userID, DBID CustomROMClutID ) =
	CallDispatchedFunction( kSetLocalUserCustomROMClutID );

Password *GetLocalUserPassword ( unsigned char userID ) =
	CallDispatchedFunction( kGetLocalUserPassword );

void SetLocalUserPassword ( unsigned char userID, Password *pw ) =
	CallDispatchedFunction( kSetLocalUserPassword );

void ValidateUserPersonificationParts( unsigned char userID, unsigned char partFlags ) =
	CallDispatchedFunction( kValidateUserPersonification );

void InvalidateUserPersonificationParts( unsigned char userID, unsigned char partFlags ) =
	CallDispatchedFunction( kInvalidateUserPersonification );

unsigned char GetValidationFlags( unsigned char userID ) =
	CallDispatchedFunction( kIsUserValidated );

void ClearNetErrors( short errorType ) =
	CallDispatchedFunction( kClearNetErrors );


#endif __SERVER__


#endif __BoxSer__
