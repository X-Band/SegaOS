/*
	File:		Options.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 7/17/94	JOE		update CreditRecord
		 <3>	 7/17/94	JOE		update
		 <2>	 6/10/94	KON		Moved DoOptionScreen prototype to equivalent in Screens.h.
		 <1>	  6/5/94	CMY		first checked in

	To Do:
*/



#ifndef __Options__
#define __Options__

typedef long HourCount;			/* Fixed point 16.16 Hours */
#define kOneMinue	0x444
#define kOneSecond	0x12

typedef char HourOfDay;			/* 0..23 = 12:00 AM .. 11:00 PM */

/* Flags for the Restrictions record */
#define kLocalCallsOnly 1

typedef struct {
	char			playField[32];
	char			hours1[32];
	char			hours2[32];
} RestrictionsRecord;


typedef short CreditCount;

typedef struct {
	CreditCount		accountCredits;
	CreditCount		usedCredits;
} CreditRecord;


#endif __Options__