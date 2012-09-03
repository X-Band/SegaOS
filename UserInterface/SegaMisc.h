/*
	File:		SegaMisc.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 5/30/94	SAH		Moved OSIdle stuff to time mgr.
		 <3>	 5/29/94	KON		Moved the graphics database type stuff into it's own file.
		 <2>	 5/28/94	KON		Added SegaNumToDate.
		 <3>	 5/18/94	HEC		Changed pattern return types to longs
		 <2>	 5/17/94	KON		Add player icons.

	To Do:
*/


#define kSpecialDateEscapeMask	0x7FFFFFFF
#define kYearMask				0x7FFF0000
#define kYearShift				16
#define kMonthMask				0x0000F000
#define kMonthShift				12
#define kDayMask				0x00000F80
#define kDayShift				7

#define kJanuary			(long)0
#define kFebruary			(long)1
#define	kMarch				(long)2
#define	kApril				(long)3
#define	kMay				(long)4
#define	kJune				(long)5
#define	kJuly				(long)6
#define	kAugust				(long)7
#define	kSeptember			(long)8
#define	kOctober			(long)9
#define	kNovember			(long)10
#define	kDecember			(long)11

void SegaNumToString( long theNum, char *theString );
void SegaNumToDate( long theDate, char *theString );
short CompareDates( long date1, long date2 );
void SetupVDP( void );
void SetupSprite( void );
void ExpandDataToVDP( unsigned char *myBitData, short numBytes );	

