/*
	File:		DBConstants.h

	Contains:	Constants for various managers which can be overridden
				by the server if necessary. The constants here are built
				into the ROM in InitRomDatabase().

	Written by:	Ted

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<59>	 8/28/94	SAH		Added kDefaultSyncOTronDelayConst.
		<58>	 8/28/94	HEC		Increased kAnswerTimeout delay.  Added kAnswerDelayConst of 2
									seconds.  Added leased line mode flag.
		<57>	 8/27/94	SAH		Added kDefaultSyncOTronTimeout.
		<56>	 8/27/94	HEC		Added GamePatchFlags const
		<55>	 8/27/94	HEC		Default user accepts challenges.
		<54>	 8/26/94	HEC		Changed kFredModemStatus value.
		<53>	 8/26/94	ATM		Changed HardDefines.h to harddefines.h for UNIX.
		<52>	 8/26/94	HEC		added kFredModemStatusConst
		<51>	 8/25/94	JOE		on second thought...
		<50>	 8/25/94	JOE		foo...update kLastConstantDBType
		<49>	 8/25/94	JOE		add hardware keyboard id const
		<48>	 8/25/94	CMY		Default to the sine wave chortle (sez Joey -- and he d'Man)
		<47>	 8/24/94	HEC		Fixed typo.
		<46>	 8/24/94	HEC		Added kDTMFDurationConst and  kDTMFInterdigitConst.
		<45>	 8/24/94	BET		Add kChatDebugFifoSize.
		<44>	 8/24/94	SAH		Increased kDefaultErrorRecoverTimeout to 3 seconds.
		<43>	 8/24/94	HEC		Made call waiting and keyboard option bits user-based.
		<42>	 8/24/94	HEC		#define'd dbconstants to avoid accidental server/box skews with
									enums.
		<41>	 8/24/94	HEC		Tweaking call waiting and peer timeout consts.
		<40>	 8/21/94	HEC		Added more call waiting consts.
		<39>	 8/21/94	JBH		Moved ROM DB constants into a single table, rather than 50+
									database entries (at 20 bytes per entry!)
		<38>	 8/20/94	CMY		Added constants for AcceptChallengeOption per user.
		<37>	 8/20/94	HEC		Added kAnswerTimeout, kAnswerToneTimeout, and
									kAnswerCarrierTimeout.
		<36>	 8/18/94	JBH		Got rid of kScreensEnteredCount.
		<35>	 8/17/94	CMY		Added constants for text chortling.
		<34>	 8/16/94	BET		Change tickle timeout to one minute.
		<33>	 8/15/94	HEC		added kListenCallWaitingTime.
		<32>	 8/13/94	HEC		Add kGPM896SecretCodeValue.
		<31>	 8/12/94	HEC		Higher noanswer timeout on originating calls.  High lost carrier
									timeout to permit leased-line to reactivate connection.
		<30>	 8/11/94	HEC		Modify.
		<29>	 8/10/94	HEC		Default call waiting enabled.
		<28>	 8/10/94	HEC		Mucked with some values.
		<27>	 8/10/94	HEC		Fine tuning peer connect timeouts, call waiting burst timeout.
		<26>	  8/7/94	KON		Bump server giveup ticks to 90 seconds.
		<25>	  8/6/94	JBH		Added constant to track server-connect messages already read.
		<24>	  8/5/94	HEC		Default to using ABCD keyboard.
		<23>	  8/5/94	JOE		Added kJizzlerOptionsConst
		<22>	  8/4/94	JBH		Added kScreensEnteredCount, which is used to seed the random
									number generator.
		<21>	  8/4/94	HEC		Added kMaxLostPeerConnections. Removed peer tlisten const.
		<20>	  8/3/94	HEC		Added kTriesBeforeDisableCW.
		<19>	  8/3/94	HEC		Added kXB_IMoved user option.  Default is unset.
		<18>	  8/1/94	HEC		Disable call waiting by default.
		<17>	  8/1/94	HEC		Added peer connect constants.
		<16>	 7/28/94	HEC		Whatever Tedward did.
		<15>	 7/28/94	JBH		Added news countdown timer constants
		<14>	 7/23/94	SAH		Added some gametalk constants.
		<13>	 7/23/94	HEC		Added kGameCallWaitingTimeout. Reduced originate timeout to 10
									seconds.
		<12>	 7/18/94	HEC		Added XBAND options flags.
		<11>	 7/17/94	HEC		Increase pause time for disabling call waiting.
		<10>	 7/16/94	HEC		Add constants for lost carrier timing and modem retrain.
		 <9>	 7/12/94	HEC		Add constants for call waitings.
		 <8>	  7/8/94	ADS		Add const's for connect-disable-password
		 <7>	  7/7/94	KON		Add server and peer-to-peer time outs and retry counts.
		 <6>	  7/7/94	HEC		Increased FuckedRBOCDelay from 2 to 3 seconds.
		 <5>	  7/6/94	BET		Add Transport timeout constants
		 <4>	  7/6/94	KON		Add Net Register timout value.
		 <3>	  7/6/94	BET		Fix after Projector fiasco and add thresholds         for
									carrier sense.
		 <2>	  7/6/94	BET		Fix after Projector fiasco and add thresholds for carrier sense.
		 <1>	  7/6/94	HEC		(BET) first checked in
	To Do:
	
*/

#ifndef __DBConstants__
#define __DBConstants__

#ifndef __HardDefines__
#include "harddefines.h"
#endif

#ifndef __GamePatch__
#include "GamePatch.h"
#endif

// XBAND Setup Options
#define kXB_LeasedLineMode			0x2			/* Set to activate leased-line modem mode on peer connects */
#define kXB_FastDSPMode				0x4			/* Set for lower peer-to-peer latency, clear for higher */
#define kXB_IMoved					0x8			/* Set to call the 800# again, clear to use local access */
#define kXB_User0AcceptsChallenges	0x10		/* Set if user 0 accepts challenges */
#define kXB_User1AcceptsChallenges	0x20		/* Set if user 1 accepts challenges */
#define kXB_User2AcceptsChallenges	0x40		/* Set if user 2 accepts challenges */
#define kXB_User3AcceptsChallenges	0x80		/* Set if user 3 accepts challenges */
#define kXB_User0QWERTYKeyboard		0x100		/* Set if user 0 wants QWERTY over ABC... */
#define kXB_User1QWERTYKeyboard		0x200		/* Set if user 1 wants QWERTY over ABC... */
#define kXB_User2QWERTYKeyboard		0x400		/* Set if user 2 wants QWERTY over ABC... */
#define kXB_User3QWERTYKeyboard		0x800		/* Set if user 3 wants QWERTY over ABC... */
#define kXB_User0CallWaitingOn		0x1000		/* Set if user 0 wants call waiting enabled */
#define kXB_User1CallWaitingOn		0x2000		/* Set if user 1 wants call waiting enabled */
#define kXB_User2CallWaitingOn		0x4000		/* Set if user 2 wants call waiting enabled */
#define kXB_User3CallWaitingOn		0x8000		/* Set if user 3 wants call waiting enabled */

#define kDefaultNewsTimerData	0x001E003C		// countdown from 60 (0x3c), 30 ticks per countdown

// CONSTANT VALUES
enum
{
	/* pmodem */
	kFuckedRBOCDelay = 180,					/* Some RBOC's require minimum onhook duration before successful redial */
	kDelayBeforeDial = 60,					/* After picking up hook, delay before dialing. */
	kDialToneTimeout = 120,					/* Max time spent waiting for valid dial tone */
	kDialToneConstTime = 60,				/* Continuous time spent expecting valid dial tone */
	kDelayBeforeDTMF = 30,					/* Time before dialing */
	kOriginateTimeout = 1200,				/* 10 second connection timeout */
	kDialPauseDelay = 180,					/* Delay when "," encountered in dial string */
	kAnswerToneDelay = 200,
	kCloseTimeout = 60,
	kRingOnTimeMin = 60,					/* RBOC-defined thresholds for detecting ring signal */
	kRingOnTimeMax = 180,
	kRingOffTimeMin = 180,
	kRingOffTimeMax = 270,
	kBusyOnTimeMin = 28,					/* RBOC-defined thresholds for detecting busy singal */
	kBusyOnTimeMax = 31,
	kBusyOffTimeMin = 28,
	kBusyOffTimeMax = 31,
	kFEOffListenThreshold = 0x1900,			/* Carrier detection thresholds */
	kFEOffOriginateThreshold = 0x1600,
	kNetRegisterTimeOutInit = 60*60*10L,
	kTransportTimeoutValue = 180,			/* Ticks until packet timeout */
	kTransportTickleCount = 20,				/* Count before we call the session dead */
	kPeerToPeerGiveUpTicks = 60*60L,		/* number of ticks before give up trying peer connection */
	kServerGiveUpTicks = 90*60L,			/* number of ticks before give up server connection */
	kPOpenPeerRetryCount = 3L,				
	kPOpenServerRetryCount = 3L,			/* number of attempts before give up server connection */
	kConnectPasswordHi = 0L,
	kConnectPasswordLo = 0L,
	kAnswerDelay = 2*60,					/* Delay after pickup but before answering */
	kAnswerTimeout = (7L<<16)|(5L),			/* (secs) hiword is overall timeout. loword is timeout on RLSD after S1DET */
	kAnswerToneTimeout = (4L<<16)|(4L),		/* (secs) hiword is atv25 timeout. loword is u1det timeout. */
	kAnswerCarrierTimeout = 5,				/* (secs) wait for carrier timeout. */
	kDTMFDuration = (75*72)/10,				/* duration of each dtmf tone in 7200 samples/sec */
	kDTMFInterdigit = (75*72)/10,			/* interdigit timing in 7200 samples/sec */
	kFredModemStatus = ((kMbit_8|kMonestop|kMenstop|kMenModem)<<8)
						| kMbit_8|kMonestop|kMenstop|kMenModem,
	
	/* CheckLine */
	kEQMThreshold = 2000,					/* Noise level above which we check for call waiting, etc. */
	kEQMPeriod = 240,						/* Minimum time between checking EQM -- need time to stablize */
	kLostRLSDPeriod = 60*5,					/* Have to lose carrier for this time before dropping line */
	kRetrainExpiration = 60*4,				/* Timeout waiting for modem retrain to complete after noisy line */
	kPreCWNoNoisePeriod = 60*4,				/* Can't have been noise on the line for this amount of time
											   before checking for call waiting tone */
	/* Call Waiting */
	kTriesBeforeDisableCW = 10,				/* Don't disable call waiting for this many times after we fail once */
	kListenCallWaitingTime = 10,			/* number of for-loop iterations tone must be active for! */
	kBongListenTimeout = 20,				/* Shouldn't have to listen very long for cw or DTMF */
	kCWDTMFPeriod = kEQMPeriod + 10,		/* Ticks to generate DMTF to remote guy. Must be greater than kEQMPeriod */
	kCWSendDTMFValue = 0xB,					/* Value to poke modem with for DTMF to other modem */
	kCWReceiveDTMFValue = 0xC,				/* Value to poke modem when interpreting DTMF from other modem */
	kCWMasterResponseTimeout = 60*90,		/* Period user has to respond to call waiting dialog */
	kCWMasterCallbackTimeout = ((60L*25)<<16) | (60*100), /* hiword is time to delay before dialing if remaining time < loword/2 */
	kCWMasterCallbackRetries = 1,			/* max reconnection retries before failure */
	kCWSlaveResponseTimeout = 60*90,		/* Period user has to respond to call waiting dialog */
	kCWSlaveCallbackTimeout = 60*120,		/* maximum time of call waiting reconnect session after detection */
	kCWSlaveCallbackRetries = 1,			/* max reconnection retries before failure */
	
	/* User & Box Options */
	kXBANDOptions1 =  kXB_LeasedLineMode
					| kXB_FastDSPMode
					| kXB_User0CallWaitingOn
					| kXB_User1CallWaitingOn
					| kXB_User2CallWaitingOn
					| kXB_User3CallWaitingOn
					| kXB_User0AcceptsChallenges
					| kXB_User1AcceptsChallenges
					| kXB_User2AcceptsChallenges
					| kXB_User3AcceptsChallenges,
					
	
	/* Jizzler Options */
	kJizzlerOptions = 0x0F,					/* Jizzlers on for all 4 users */
	
	/* peer connect */
	kChatTickleSentTimeout = 30,			/* tickle every 0.5 sec */
	kChatMaxTimeBetweenReads = 120,			/* 1.5 seconds of dead air is max acceptable */
	kPeerMasterRedialTime = 60*30,			/* Timeout to redial opponent after line droppage */
	kPeerSlaveWaitTime = 60*40,				/* Timeout to wait for opponent after line droppage */
	kMaxLostPeerConnections = 6,			/* Cap number of lost connections per peer game */
	
	/* gametalk */
	kDefaultSynchTimeout = 60*10,			/* time to wait for other while establishing synch */
	kDefaultSynchStateTimeout = 60*1,		/* time to wait for a state change in establish synch */
	kExchangeCommandsTimeout = 60*1,		/* time to wait for other box's command */
	kDefaultErrorRecoverTimeout = 60 * 3,	/* default timeout for GTSession_ErrorRecover */
	kDefaultSyncOTronTimeout = 60 * 10,		/* default timeout for syncOTron */
	kDefaultSyncOTronDelay = 60 * 3,		/* delay after sync for synco sync sound */
	
	/* smartcards */
	kGPM896SecretCodeValue = 0xAAAA,		/* Current code, may be different */
	
	/* Chortling text */
	kDefaultChortleType = 1,				/* How to animate text under buttons */
	
	/* size of buffer for chat script debug */
	kChatDebugFifoSize = 2048,
	
	/* Hardware Keyboard ID */
	kESKeyboardID = 0x030C0609,
	
	/* Game Patch */
	kGamePatchFlags = kRemoteGame | kColdStart,
	
	/* last one */
	kLastFucker
};


// DB CONSTANT ID's

#define kDBFuckedRBOCDelayConst				0
#define kDBDelayBeforeDialConst				1
#define kDBDialToneTimeoutConst				2
#define kDBDialToneConstTimeConst			3
#define kDBDelayBeforeDTMFConst				4
#define kDBOriginateTimeoutConst			5
#define kDBDialPauseDelayConst				6
#define kDBAnswerToneDelayConst				7
#define kDBCloseTimeoutConst				8
#define kDBRingOnTimeMinConst				9
#define kDBRingOnTimeMaxConst				10
#define kDBRingOffTimeMinConst				11
#define kDBRingOffTimeMaxConst				12
#define kDBBusyOnTimeMinConst				13
#define kDBBusyOnTimeMaxConst				14
#define kDBBusyOffTimeMinConst				15
#define kDBBusyOffTimeMaxConst				16
#define kDBFEOffListenThresholdConst		17
#define kDBFEOffOriginateThresholdConst		18
#define kNetRegisterTimeOutInitConst		19
#define kDBTr