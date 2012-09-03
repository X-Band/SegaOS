/*
	File:		Errors.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<10>	 8/13/94	BET		Add kScriptTimeoutErr.
		 <9>	 8/12/94	HEC		Errors are now hard coded for look-up-ability and so they don't
									potentially move around as enums.
		 <8>	 8/11/94	HEC		SHOW_SEGA_ERROR_DIALOG on asserts and errors.
		 <7>	 8/10/94	HEC		Added kLineNoiseErr
		 <6>	  8/9/94	HEC		Added smartcard errors.
		 <5>	  8/9/94	HEC		Consolidated errors from entire project.
		 <4>	 6/11/94	DJ		Split out database errors
		 <3>	 5/31/94	DJ		Adding more errors
		 <2>	 5/31/94	HEC		Adding more errors

	To Do:
*/


#ifndef __Errors__
#define __Errors__

/************************************************************************
	Defining DEBUG turns on all assertion checks.
	Defining MESSAGES turns on interesting state messages.
	
	For testing you turn both on.
	For running real systems with debugging turn on only DEBUG
	For real running turn them both off.
	
	These are now defined in the ThinkC project Prefixes.

#define DEBUG
#define MESSAGES

	-dj 4/30/94									

*************************************************************************/


typedef long Err;

#ifndef noErr
#define noErr 0
#endif

#define	kFalse							0
#define kTrue							1
#define kNo								0
#define kYes							1

/* system errors */
#define kNoError						0
#define kFuckedErr						-1
#define kOutOfMemoryErr					-2
#define globalIDRangeErr				-3
#define procIDRangeErr					-4
#define managerIDRangeErr				-5
#define commandSelectorUnknown			-6
#define badDispatchSelector				-7
#define outOfSpritesErr					-8
#define cantCreateCursorErr				-9
#define boxIDGlobalsCorrupted			-10
#define kGameFailedErr					-11
#define kGameHeapTrashedErr				-12
#define kRebootDuringGameErr			-13	

/* database errors */
#define kDBDuplicateTypeError			-100	
#define kDBUnknownTypeError				-101
#define kDBDuplicateIDError				-102
#define kDBUnknownIDError				-103
#define kDBNullData						-104
#define kDBPurgeError					-105
#define kDBROMEntryError				-106

/* dialog errors */
#define kDialogErr						-200

/* message errors */
#define kProcessingStream				0
#define kUnrecognizedOpCodeError		-300
#define kStreamDone						-301
#define kFatalStreamError				-302
#define kSimulatorUserAborted			-303
#define kMessageHandlerOutOfMemory		-304
#define kOSCorrupt						-305
#define kOpponentVerificationFailed		-306

/* network errors */
#define kFifoOverflowErr				-400
#define kFifoUnderflowErr				-401
#define kFifoWrongType					-402
#define kSessionNotClosingErr			-403
#define kSessionClosedErr				-404
#define kLinkClosed						-405
#define kForwardResetAborted			-406
#define kAsyncInProgErr					-407
#define kNotASession					-408
#define kWrongPacketType				-409
#define kNoSessionRecFound				-410
#define kRuntPacketDetected				-411
#define kPortOpenErr					-412
#define kConnectBusy					-413
#define kConnectFailed					-414
#define kNoAnswer						-415
#define kNoDialtone						-416
#define kFwdResetFailed					-417
#define kCantOpenConnection				-418
#define kNoByteReady					-419
#define kTransmitBufferFull				-420
#define kConnectionLost					-421
#define kNoConnection					-422
#define kParityError					-423
#define kFrameError						-424
#define kOverrunError					-425
#define kTimeout						-426
#define kCallWaitingErr					-427
#define kRemoteCallWaitingErr			-428
#define kHandshakeErr					-429
#define kLineNoiseErr					-430
#define kScriptTimeoutErr				-431

/* smartcard errors */
#define kCardNotInstalledErr			-500
#define kNotCatapultCardErr				-501
#define kUnknownCardTypeErr				-502
#define kIncorrectCardTypeErr			-503
#define kSecretCodeFailedErr			-504
#define kNoMoreCreditsErr				-505
#define kDebitErr						-506
#define kIncompleteDebitErr				-507

/* gametalk errors */
#define kBogusControllerSize			-600
#define kGibbledPacket					-601
#define kNoData							-602
#define kBadControlPacket				-603
#define kBadPacketData					-604
#define kFucked							-605


void ReportError(char *file, long line, char *message);
void ReportWarning(char *file, long line, char *message);
void ReportMessage(char *file, long line, char *message);

#ifdef DEBUG

#define ASSERT(a)				{ if(!(a))	ReportError(__FILE__, __LINE__, "Failed Assertion"); }
#define ASSERT_MESG(a, msg) 	{ if(!(a))	ReportError(__FILE__, __LINE__, msg); }
#define ERROR_MESG(msg)			ReportError(__FILE__, __LINE__, msg);
#define WARNING_MESG(msg)		ReportWarning(__FILE__, __LINE__, msg);

#elif defined(SHOW_SEGA_ERROR_DIALOG)

#define ASSERT(a)				{ if(!(a))	ReportError(__FILE__, __LINE__, "Failed Assertion"); }
#define ASSERT_MESG(a, msg) 	{ if(!(a))	ReportError(__FILE__, __LINE__, msg); }
#define ERROR_MESG(msg)			ReportError(__FILE__, __LINE__, msg);
#define WARNING_MESG(msg)

#else

#define ASSERT(a)
#define ASSERT_MESG(a, msg)
#define ERROR_MESG(msg)
#define WARNING_MESG(msg)

#endif	/* DEBUG */



#ifdef MESSAGES
#define MESG(msg)				ReportMessage(__FILE__, __LINE__, msg);
#else
#define MESG(msg)
#endif	/* MESSAGES */



#endif __Errors__