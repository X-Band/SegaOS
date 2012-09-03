/*
	File:		Dates.h

	Contains:	xxx put contents here xxx

	Written by:	Jevans

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	  6/3/94	DJ		first checked in

	To Do:
*/


#ifndef __Dates__
#define __Dates__

/*
*	Some date stuff
*/

#define kSpecialDateEscapeMask	0x7FFFFFFF
#define kYearMask				0x7FFF0000
#define kYearShift				16
#define kMonthMask				0x0000F000
#define kMonthShift				12
#define kDayMask				0x00000F80
#define kDayShift				7

#define DateYear(year)		(((year) << kYearShift) & kYearMask )
#define DateMonth(month)	(((month) << kMonthShift) & kMonthMask)
#define DateDay(day)		(((day) << kDayShift) & kDayMask)
#define Date(year, month, day)	(DateYear(year) | DateMonth(month) | DateDay(day))


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


#endif __Dates__
