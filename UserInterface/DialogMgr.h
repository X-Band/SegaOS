/*
	File:		DialogMgr.h

	Contains:	Modal Dialogs

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<72>	 8/27/94	CMY		Added a dialog for not enough RAM DB memory for new players in
									player list.
		<71>	 8/26/94	KON		Add the kTellEmHowToChangeTheirPhoneNumber dialog.
		<70>	 8/26/94	CMY		Added a dialog for duplicate code name for box players.
		<69>	 8/26/94	KON		Add a take one version of player list connects. Change the write
									mail while registered dialog to a warning rather than a yes/no
									dialog.
		<68>	 8/24/94	HEC		Reversed order of kSlavePeerReconnectTimedOutDialog and
									kMasterPeerReconnectTimedOutDialog.
		<67>	 8/23/94	KON		added kResettingYourPhoneNumber dialog
		<66>	 8/21/94	KON		added user sure number is correct.
		<65>	 8/21/94	KON		added kHavingTroubleWithLocalAccessNum.
		<64>	 8/21/94	HEC		added kCountdownTimerElapsed.
		<63>	 8/17/94	HEC		added kAreYouSureYouWantFindOpponentAskPhone and
									kAreYouSureYouWantConnectToAddressBookPlayerAskPhone
		<62>	 8/17/94	KON		Remove stale dialogs.
		<61>	 8/14/94	HEC		Added kVoiceCallNotModemDialog and
									kPeerConnectProbWaitMoreDialog.
		<60>	 8/13/94	HEC		Added kWipeOSYesNoDialog
		<59>	 8/12/94	HEC		Removed kDialogErr.
		<58>	 8/11/94	KON		Add new user confirmation and information dialogs.
		<57>	  8/6/94	HEC		Added kSlaveWaitsForOpponentToCallDialog.
		<56>	  8/4/94	JOE		added 	kTauntNotWhatYouWanted & kInfoNotWhatYouWanted
		<55>	  8/3/94	JOE		added kHandleNotWhatYouWanted
		<54>	  8/1/94	HEC		Added k[Master][Slave]PeerReconnectTimedOutDialog
		<53>	 7/30/94	HEC		Added kWrongPeerDialog
		<52>	 7/30/94	KON		Added cancel entering mail dialog.
		<51>	 7/29/94	HEC		Reorder call waiting dialogs.
		<50>	 7/29/94	HEC		Added kCallWaitingDuringChat and kRemoteCallWaitingDuringChat.
		<49>	 7/26/94	HEC		Added kCallWaitingDuringGameDialog
		<48>	 7/18/94	dwh		Fix server build collision
		<47>	 7/17/94	JBH		Added secret keys to dialogs.
		<46>	 7/17/94	HEC		Fixed foobar checkin.
		<45>	 7/17/94	HEC		Added smart card dialogs.
		<44>	 7/16/94	CMY		Removed old CES Dialogs
		<43>	 7/15/94	ADS		Added new password dialogs
		<42>	 7/15/94	KON		Add inbox is full, are you sure you want to connect dialog.
		<41>	 7/14/94	KON		Dialog templates are numbered from 0, not 1.
		<40>	 7/14/94	KON		Added error dialog when trying to update mail and already
									registered against an opponent.
		<39>	 7/13/94	CMY		Added Dialogs for Mail preflights.
		<38>	 7/13/94	KON		Add dialogs for you may be interrupted while entering mail and
									entering an address book entry.
		<38>	 7/13/94	KON		Add dialogs for you may be interrupted while entering mail and
									entering an address book entry.
		<37>	 7/12/94	KON		Remove some stale dialogs.
		<36>	 7/12/94	KON		Add dialogs for confirmation on delete.
		<35>	 7/11/94	KON		Add connect for mail only and connect for address book player
									dialogs.
		<34>	  7/9/94	HEC		Added dialogs for modem dubugging and for when the guy has
									changed phone numbers.
		<33>	  7/8/94	CMY		Added dialogs for Equivalent (after case and space stripping)
									names.
		<32>	  7/8/94	ADS		Added semi-universal dialogs
		<31>	  7/8/94	ADS		Add dialogs for connect-disable-password
		<30>	  7/7/94	CMY		More dialogs for AddressBook errors.
		<29>	  7/6/94	KON		Add dialogs to warn about network registration.
		<28>	  7/6/94	KON		Add kAbsMinimumTime.
		<27>	  7/6/94	ADS		Added PW dialogs, new DoDialog format
		<26>	  7/5/94	CMY		Added error dialogs for address book and mail screens.
		<25>	  7/5/94	KON		Added kPeerToPeerConnectFailed dialog.
		<24>	  7/3/94	CMY		Added address book dialogs.
		<23>	  7/1/94	KON		Added minimum and maximum times a dialog can stay up.
		<22>	  7/1/94	KON		Add some network error case dialogs.
		<21>	  7/1/94	DJ		added #ifndef __SERVER__
		<20>	  7/1/94	KON		Add are you sure you want to connect dialog.
		<19>	  7/1/94	KON		Removed some of the CES dialogs. Added can't delete CATAPULT
									from the address book dialog.
		<18>	  7/1/94	KON		Add server number busy.
		<17>	 6/30/94	KON		Added new dialogs for no news.
		<16>	 6/28/94	KON		Add kDiedWaitingForGameDeRegister.
		<15>	 6/21/94	KON		Add opponent has been found dialog.
		<14>	 6/20/94	SAH		Changed to kPlayAgainDialog because i'm stupid.
		<13>	 6/20/94	SAH		Added kPlayAgain dialog.
		<12>	 6/20/94	KON		Add kYouGotNoAddresses dialog.
		<11>	 6/20/94	KON		Moved CPDialog over from private so the DeferredDialogMgr can
									create dialogs directly. Added new call, DoDialogItem.
		<10>	 6/20/94	KON		Add CES dialogs and the youGotNoMail dialog.
		 <9>	 6/17/94	KON		Added lots of game registration dialogs.
		 <8>	 6/17/94	KON		Added game not in NGP list and two stage already registered
									dialogs.
		 <7>	 6/16/94	KON		Add already waiting for game dialog.
		 <6>	 6/15/94	CMY		Dialog parameter text
		 <5>	 6/15/94	KON		Add user has new name dialog.
		 <4>	 6/15/94	KON		Added some box dying disaster screens.
		 <3>	 6/15/94	KON		Add AddToAddressBook and AddWhenDeleted dialogs.
		 <2>	 6/14/94	CMY		Make dialogs database-driven.
		 <1>	 6/14/94	CMY		first checked in

	To Do:
*/

#ifndef __SegaDialogs__
#define __SegaDialogs__

#ifndef __DataBase__
	#include "Database.h"
#endif

#ifndef	__SERVER__
#ifndef	__GT__
	#include "GT.h"
#endif
#endif	/* ! __SERVER__ */

enum
{
	kFuckerDialog = 0,
	kAddToAddressBookDialog,
	kWillBeAddedWhenDeleteDialog,
	kDiedDuringServerConnectionDialog,
	kDiedDuringPeerConnectionDialog,
	kDiedWaitingForGame,					// 5
	kDiedDuringLocalCallWaiting,
	kDiedDuringRemoteCallWaiting,
	kUserHasNewNameDialog,
	kGameNotInNGPList,
	kAlreadyRegistered1,					// 10
	kAddressBookVsNetwork,
	kVersusPlayerListTakeOne,
	kAddressBookVsNewAddressBook,
	kNetworkVsNetwork,
	kNetworkVsAddressBook,					// 15
	kAddressBookVsSameAddressBookTakeTwo,

	kPlayAgainDialog = 18,
	kAnOpponentHasBeenFoundDialog,
	kDiedWaitingForGameDeRegister,			// 20
	kNoOtherNews,
	kNoDailyNews,
	kServerNumberBusy,
	kAreYouSureYouWantFindOpponent = 25,
	kNoDialToneDialog,
	kSomeSortOfNetworkErrorDialog = 28,
	kTheLineHasDroppedDialog,
	kAddressBookFullDialog,					// 30
	kPeerToPeerConnectFailed,		
	kMailNoRecipientDialog,
	kMailNoTitleDialog,
	kMailNoMessageDialog,
	kAddressEmptyDialog,					// 35
	kEnterPasswordDialog,
	kWrongPasswordDialog,
	kCantChangePlayerWhileWaitingMinutes,
	kCantChangePlayerWhileWaitingMinute,
	kCantChangePlayersWhileWaitingSeconds,	// 40
	kDuplicateAddressBookEntryDialog,
	kHandleTooShortDialog,
	kConnectPasswordDialog,
	kConnectPWFailDialog,
	kEquivalentAddressBookEntryDialog,		// 45

	kAreYouSureYouWantConnectMailOnly = 51,
	kAreYouSureYouWantConnectToAddressBookPlayer,
	kAreYouSureDeleteAddressEntry,
	kAreYouSureDeleteMailEntry,
	kYouMayBeInterruptedMail,				// 55
	kYouMayBeInterruptedAddress,
	kOutboxFullDialog,
	kMailAddedToOutboxDialog,
	kAlreadyRegisteredCantUpdateMailDialog,
	kInBoxFullSureYouWantToConnect,			// 60
	kReenterNewPassword,		
	kNewPasswordNoMatch,
	kNotCatapultSmartCardDialog,
	kNotDebitCardDialog,
	kDebitCardPulledDialog,					// 65
	kNoCreditsLeftDialog,
	kProblemDebitingDialog,
	kThanksForPlayingDialog,
	kOpponentDeniedAgainDialog,
	kCallWaitingDuringChatDialog,			// 70
	kRemoteCallWaitingDuringChatDilog,
	kMasterCallWaitingDuringGameDialog,
	kMasterRemoteCallWaitingDuringGameDialog,
	kSlaveCallWaitingDuringGameDialog,
	kSlaveRemoteCallWaitingDuringGameDialog,	// 75
	kWrongPeerDialog,
	kAreYouSureYouWantToCancelMailEntryDialog,
	kSlavePeerReconnectTimedOutDialog,
	kMasterPeerReconnectTimedOutDialog,
	kHandleNotWhatYouWanted,					// 80
	kTauntNotWhatYouWanted,					
	kInfoNotWhatYouWanted,
	kSlaveWaitsForOpponentToCallDialog,
	kAreYouSureYouWantToCreateANewPlayer,
	kTellThemAboutOptionsMenu,
	kWipeOSYesNoDialog,
	kVoiceCallNotModemDialog,
	kPeerConnectProbWaitMoreDialog,
	kAreYouSureYouWantFindOpponentAskPhone,
	kAreYouSureYouWantConnectToAddressBookPlayerAskPhone,	// 90
	kHavingTroubleWithLocalAccessNum,	// 91
	kUserSureNumberIsCorrectTryBackLater,	// 92
	kResettingYourPhoneNumber,
	kCodeNameAlreadyTaken,
	kTellEmHowToChangeTheirPhoneNumber,
	kNotEnoughRoomForNewPlayerList
};

// Return values from DialogMgr routines

#define kDialogOK	0		/* User dismissed an InformDialog */
#define kDialogNo	0		/* User said "no" to a ConfirmDialog */
#define kDialogYes	1		/* User said "yes" to a ConfirmDialog */

#define kCountdownTimerElapsed 0x8000

// Dialog sizes

#define	kSmallDialog	0
#define	kMediumDialog	1
#define kLargeDialog	2

// Return values from Play Again dialog
enum {
	kReturnPlayAgain = 0,			// both said yes
	kReturnDontPlayAgain,			// we said no
	kReturnChat,					// both said chat
	kReturnDingedPlayAgain,			// (s)he said no (is it "dinged" or "dung" ??)
	kReturnDroppedPlayAgain			// network failure
};

//
// Absolute minimum time a dialog can be up (assuming no user intervention)
//
#define kAbsMinimumTime (60*2)

#ifndef __SERVER__

/*
 * Describes a dialog to present.
 */
typedef struct 
{
	short			minimumTime;	/* Refers to minimum time dialog will appear (user can dismiss earlier) */
	short			maximumTime;	/* Refers to maximum time dialog will appear */
	DBID			template;		/* Refers to a CPDialogTemplate above, to get the overall look of a dialog */
	unsigned char	variation;		/* 0=inform, 1=Yes/No */
	unsigned char	defaultItem;	/* Default (0=No, 1=Yes) */
	unsigned char	unique;			/* only used by variation #3 */
	DBID			secretID;		/* ID of secret sequence for dialog (0 for none) */
	char			message[1];		/* The actual message, variable length */
									/* type 3 has further messages	*/
} CPDialog;

typedef struct PlayAgainStruct {		// used to pass data to the PlayAgain dialog
	GTSession	*session;
	Boolean		master;
} PlayAgainStruct;

/* This sets up the colors for the dialog frame and background. The
 * frame consists of 5 rectangular frames, each in a different color.
 */
void SetDialogColors(short background, short frame1, short frame2, short frame3,
	short frame4, short frame5) =
		CallDispatchedFunction( kSetDialogColors );

/* Put up dialog and return kDialogError or kDialogOK */
short DoDialog(DBID which) =
		CallDispatchedFunction( kDoDialog );

/* Some fancier dialogs get a parameter to help them set up */
short DoDialogParam(DBID which, void* param) =
		CallDispatchedFunction( kDoDialogParam );

short DialogParameterText(char *p1, char *p2, char *p3, char *p4) =
		CallDispatchedFunction( kDialogParameterText );

short DoDialogItem( CPDialog *dbDialog, void* param ) =
		CallDispatchedFunction( kDoDialogItem );

#endif __SERVER__


#endif __SegaDialogs__
