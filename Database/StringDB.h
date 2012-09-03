/*
	File:		StringDB.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<65>	 8/27/94	JOE		Added writeable string "None"
		<64>	 8/26/94	CMY		Added messages for server connects.
		<63>	 8/25/94	CMY		Added default info text for new Player List players.
		<62>	 8/24/94	HEC		Added findopponent/popen debug strings.
		<61>	 8/24/94	KON		Remove customer service phone number string since it don't work
									that way no more.
		<60>	 8/22/94	KON		Remove a bunch of (hopefully) stale strings.
		<59>	 8/21/94	JBH		Added kTimerBelowZeroString.
		<58>	 8/20/94	JOE		Added kChooseIconCustomIconDescription
		<57>	 8/20/94	HEC		Added kOpponentName
		<56>	 8/20/94	CMY		Added description strings for the XBand setup screen.
		<55>	 8/19/94	CMY		Added a string to indicate when the box is registered.
		<54>	 8/18/94	HEC		Added kXBANDCardChargedCreditString
		<53>	 8/17/94	JOE		Nuke point strings, add game value strings
		<52>	 8/15/94	KON		Remove stale CES strings.
		<51>	 8/15/94	JOE		Add kOn & kOff, move some sound test page things around
		<50>	 8/14/94	KON		Added syncotrong and patent pending to writable strings. As soon
									as we get the patent we can change them!
		<49>	 8/14/94	KON		Added kTheyString.
		<48>	 8/12/94	JOE		Added WSTRs for Opponent taunt & info
		<47>	 8/11/94	KON		Remove some stale strings.
		<46>	  8/9/94	HEC		added kWaitingForPeerCallString.
		<45>	  8/7/94	JOE		added kPoint & kPoints WSTRs
		<44>	  8/6/94	JBH		Added writeable strings for server-connect messages.
		<43>	  8/6/94	HEC		added kSlaveWaitingForMasterCallString
		<42>	  8/3/94	JOE		added constants for taunt/info WSTRs
		<41>	  8/3/94	HEC		Added kBoxPhoneNumber and 	kYouAreCallingFromString.
		<40>	 7/31/94	KON		Add string for new address book entry.
		<39>	 7/31/94	KON		Add customer support number as a writeable string.
		<38>	 7/31/94	KON		Add support and enums for writeable strings.
		<37>	 7/31/94	HEC		Added kConnectedToPeerString
		<36>	 7/29/94	KON		Add kWaiting for Anwer string.
		<35>	 7/26/94	CMY		Added QWERTY keyboard mappings
		<34>	 7/25/94	CMY		Added strings for handle and icon entry.
		<33>	 7/23/94	JOE		Added a couple more strings for the ranking screen
		<32>	 7/22/94	HEC		Added call waiting strings for game or os dialogs during game
									play.
		<31>	 7/22/94	BET		Add prompt strings for 800 and X.25 login.
		<30>	 7/22/94	JOE		added kNoSmartCardPresentString
		<29>	 7/20/94	KON		Add new Joey strings for mail and address book.
		<28>	 7/18/94	DJ		made serverable
		<27>	 7/16/94	JOE		sync w/rsrc file, add account info page strings
		<26>	 7/15/94	HEC		Cleanup. Added new strings.
		<25>	  7/8/94	CMY		Added upper-case keyboard map.
		<24>	  7/7/94	KON		Add special string kNeverDate.
		<23>	  7/7/94	KON		Add net has timed out string which is used to create a deferred
									dialog.
		<22>	  7/6/94	JOE		update string list for user options screen
		<21>	  7/6/94	KON		Add some peer-to-peer status strings.
		<20>	  7/5/94	KON		Make date of 0 the first special date string.
		<19>	  7/1/94	KON		Remove strings used by old results screen.
		<18>	  7/1/94	KON		Added kCATAPULT string. It's used by the Address book to
									identify the root.
		<17>	 6/30/94	CMY		Add strings for keyboard layout
		<16>	 6/20/94	KON		Add CES slave strings.
		<15>	 6/20/94	BET		Added kConnectToCatapultBusy.
		<14>	 6/15/94	HEC		Added kConnectToCatapultFailed.
		<13>	 6/13/94	SAH		Managerized through text manager.
		<12>	  6/5/94	DJ		Added news strings
		<11>	  6/5/94	KON		Added modem connect strings.
		<10>	  6/5/94	KON		Updated a bunch of string names to match new .rsrc.
		 <9>	  6/5/94	CMY		Changed strings to match Joey's new UI
		 <8>	  6/4/94	KON		Add Game, Match, Score.
		 <7>	  6/3/94	CMY		Added more strings for the options screen
		 <6>	  6/3/94	CMY		Added strings for the options screen.
		 <5>	  6/1/94	KON		Add address book strings
		 <4>	 5/30/94	SAH		Added more strings.
		 <3>	 5/29/94	KON		Add more strings.
		 <2>	 5/28/94	KON		Add loads of strings.
		 <1>	 5/28/94	KON		first checked in

	To Do:
*/

#ifndef __StringDB__
#define __StringDB__

//
// Eight "special" dates
//
#define kSpecialDateIndexMask 	0x07

enum
{
	kNullString,
	kDelete,
	kExit,
	kFrom,
	kTitle,
	kDate,
	kFromInMailView = 7,
	kTitleInMailView,
	kDateInMailView,
	kSpecialDateStart,				//10  nil date: Empty string right now
	kNilDate = kSpecialDateStart,	//10
	kNeverDate,
	kAprilFoolsDay,
	kSpecialDatePlaceHolder3,
	kSpecialDatePlaceHolder4,
	kSpecialDatePlaceHolder5,
	kSpecialDatePlaceHolder6,
	kSpecialDatePlaceHolder7,
	kJanuaryString,
	kFebruaryString,
	kMarchString,					//20
	kAprilString,
	kMayString,
	kJuneString,
	kJulyString,
	kAugustString,
	kSeptemberString,
	kOctoberString,
	kNovemberString,
	kDecemberString,
	kRead,							//30
	kNew,
	kFindOpponentString,
	kRankingString,
	kMailString,
	kAddressBookString,
	kEnhancementsString,
	kOptionsString,
	kName,
	kDateLastPlayed,
	kStats,							//40
	kGameOptions = 42,
	kHoursOfPlay = 46,
	kDialingOpponent = 51,
	kPeerNotAnswering,
	kPeerPhoneBusy,
	kCatapultNewsString,
	kGeneralNewsString,
	kPersonalMailString,

	kPlayerProfileString = 58,
	kSelectPlayerString,
	kConnect,						//60
	kAbout,
	kReturn,
	kWaitingForAnswer = 72,
	kDialingCatapult,
	kConnectedToCatapult,
	
	
	kConnectToCatapultBusy = 80,			//80
	kKeyboardLower0,
	kKeyboardLower1,
	kKeyboardLower2,
	kKeyboardLower3,
	kRankingTownPrompt = 87,
	kRankingCurrentLevelPrompt,
	kRankingCurrentPointsPrompt,
	kRankingNextLevelPrompt,		//90
	kRankingNextPointsPrompt,
	
	kChangeHandle = 93,
	kChangeFace,
	kChangeTaunts,
	kChangeInfo,
	kChangePassword,
	kTheNetHasTimedOutString,
	kKeyboardUpper0,
	kKeyboardUpper1,				//100
	kKeyboardUpper2,
	kKeyboardUpper3,
	kCallWaitingString,
	kEnableString,
	KDisableString,
	kKeyboardTypeString,
	kABCDString,
	kQWERTYString,
	kCallWaitingDisable,
	kAboutBox0,						// 110
	kAboutBox1,
	kAboutBox2,
	kAboutBox3,
	kRestrictionsString,
	kPlayingFieldString,
	kHoursOfPlayString,
	kCreditsString,
	kTotalCreditsString,
	kPrepaidCreditsString,
	kAccountCreditsString,			// 120
	kSmartCardCreditsString,
	kNoSmartCardPresentString,
	kConnectedToPeerString,
	kBoxPhoneNumber,				// overridden by the server
	kYouAreCallingFromString,		// 125
	kAnsweringPhoneString,
	kWaitingForPeerCallString,

	kChooseHandleDescription = 131,
	kWins,
	kLosses,
	kStatsColon,
	kDateLastPlayedColon,
	kTitleColon,
	kTownColon,
	
	// prompt strings for 800 and X.25 login
	k800Expect1 = 138,
	k800Return1,
	kX25Expect1,
	kX25Return1,
	kX25Expect2,
	kX25Return2,
	kX25Expect3,
	kX25Return3,
	kX25Expect4,
	kX25Return4,

	kDefaultInfoText = 149,
	kRankingNextLevel2String = 153,
	kRankingNextPoints2String,
	kEnterYourHandle,
	kChooseIcon,
	kChooseIconDescription,
	
	kQWERTYLower0,
	kQWERTYLower1,
	kQWERTYLower2,
	kQWERTYLower3,
	kQWERTYUpper0,
	kQWERTYUpper1,
	kQWERTYUpper2,
	kQWERTYUpper3,
	
	kNewAddressEntryDescription = 168,
	kHitStartWhenFinished = 186,
	kConnectWildcard,
	kTheyString,

	kXBANDCardChargedCreditString = 191,

	kAcceptChallengesString,
	kAcceptChallengesYES,
	kAcceptChallengesNO,
	kCallWaitingON,
	kCallWaitingOFF,
	kCallingFromSAME,
	kCallingFromNEW,
	kKeyboardTypeQWERTY,
	kKeyboardTypeABCD,
	
	kChooseIconCustomIconDescription,

	kTimerBelowZero,
	kMailTimerMessage,
	kConnectedTimerMessage,

	kConnectBusyStr = 210,
	kNoAnswerStr,
	kConnectFailedStr,
	kHandshakeErrStr,
	kScriptTimeoutErrStr,

	/* keep this one as last */
	kLastString
};

/* Don't make this more than # bits in a dbconstant (ie 32), as there is a dbconstant which is a bitfield */
/* recording which messages have already been read! */
#define	kNumConnectMessageIDs	20		/* number of WSTRs stored that can be displayed during connect */

//
// Server string enums begin
//
enum
{
	kUser0Taunt	= 0,
	kUser1Taunt,
	kUser2Taunt,
	kUser3Taunt,
	kUser0Info,
	kUser1Info,
	kUser2Info,
	kUser3Info,
	kSoundTestText = 64,
	kMusicTrackText,
	kMusicText,
	kOnText,
	kOffText,
	kNoneString,			
	kThankYouStringID	= 128,
	kSyncoTronString,
	kPatentPendingString,
	kOpponentTaunt		= 170,
	kOpponentInfo,
	kOpponentName,
	kLocalGameValueString  = 180,
	kOppGameValueString,
	kFirstConnectMessageID = 192	/* ID's 192 - 192+kNumConnectMessageIDs-1 reserved for connect msgs */
};

typedef struct
{
	short	xPos;
	short	yPos;
	char	cString[1];
} xyString;


#ifndef __SERVER__

void DrawDBXYString( DBID stringID ) =
	CallDispatchedFunction( kDrawDBXYString );

xyString *GetDBXYString( DBID stringID ) =
	CallDispatchedFunction( kGetDBXYString );

char *GetSegaString( DBID stringID ) =
	CallDispatchedFunction( kGetSegaString );

char *GetWriteableString( DBID stringID ) =
	CallDispatchedFunction( kGetWriteableString );

char *SetWriteableString( DBID stringID, char *theString ) =
	CallDispatchedFunction( kSetWriteableString );

void DeleteWriteableString( DBID stringID ) =
	CallDispatchedFunction( kDeleteWriteableString );

DBID GetUniqueWriteableStringID( void ) =
	CallDispatchedFunction( kGetUniqueWriteableStringID );


#ifdef SIMULATOR
void AddDBXYString( DBID stringID, xyString *theString, short itemSize );
#endif SIMULATOR

#endif __SERVER__
#endif __StringDB__