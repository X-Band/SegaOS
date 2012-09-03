/*
	File:		BoxSerPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<16>	 8/23/94	BET		Add netErrorsValid field
		<15>	 8/21/94	KON		Add second local access number.
		<14>	 8/21/94	JBH		Added pointer to ROM DB constants table.
		<13>	 8/20/94	JOE		added userCustomROMClutID[kMaxLocalUsers]
		<12>	 8/18/94	ADS		Changed 4's to kMaxLocalUsers
		<11>	 8/18/94	ADS		Hide gBoxID globals
		<10>	 8/17/94	JOE		gameValue global no longer needed
		 <9>	  8/7/94	JOE		added gameValue global
		 <8>	  8/4/94	SAH		Keep track of game errors.
		 <7>	 7/27/94	KON		Add dialNetwork again flag.
		 <6>	 7/20/94	DJ		added resultsAreValid for GetCompetitionResults
		 <5>	 7/15/94	SAH		Rearranged globals.
		 <4>	 7/15/94	dwh		unix-ise.
		 <3>	  7/2/94	SAH		Added globals.
		 <2>	  7/1/94	HEC		Added masterFlag global.
		 <1>	 6/27/94	KON		first checked in

	To Do:
*/

#ifndef __SegaTypes__
#include "SegaTypes.h"
#endif


#ifdef	unix
typedef	char	Boolean;
#endif

typedef
struct BoxServicesGlobals
{
	long			curGameID;
	long			boxState;
	long			lastBoxState;
	long			wins;
	long			losses;
	short			dialNetworkAgain;	//used by 800 num and server to decide whether
										//or not to dial back.
	GameResult		results;
	GameResult		errorResults;
	Boolean			resultsAreValid;
	Boolean			errorResultsValid;
	Boolean			masterFlag;
	Boolean			netErrorsValid;
	long			*ROMDBConstants;
} BoxServicesGlobals;


/*
*	Box serial/server phone number global. These are placed in the low level os heap so they
*	don't get trashed if the os gets wasted. They are checksummed. if the checksum goes bad,
*	they are wiped...
*/

typedef
struct BoxIdentification
{
	BoxSerialNumber	boxID;
	phoneNumber		localAccessNumber1;
	phoneNumber		localAccessNumber2;
	phoneNumber		boxPhoneNumber;
	unsigned char	userValidationFlags[kMaxLocalUsers];	// indicates which parts of personif have been changed; cleared by server download
	UserName		userName[kMaxLocalUsers];
	DBID			userROMIconID[kMaxLocalUsers];			// RAM icon, if there is is one, comes from boxID & userID
	DBID			userCustomROMClutID[kMaxLocalUsers];	// if 0xFF, use clut bitmap's clut
	Password		userPassword[kMaxLocalUsers];
	Hometown		boxHometown;
	long			problemToken;
	long			validationToken;
	XBANDCard		lastCard;
	unsigned short	checksum;
} BoxIdentification;


/*
*	This is the accessor macro for the Box id globals in high mem
*/
#define	gBoxIDAddress		( gBoxRestartAddress - sizeof(BoxIDReserved) )
#define	gBoxID				((BoxIdentification *) gBoxIDAddress)





#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	BoxServicesGlobals
#else
extern BoxServicesGlobals services;
#endif
