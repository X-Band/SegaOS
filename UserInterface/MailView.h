/*
	File:		MailView.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	  8/9/94	CMY		Moved some KONstants.
		<14>	  8/7/94	CMY		Updated rectangle coordinates
		<13>	 7/30/94	JOE		add color cycling rate const
		<12>	 7/16/94	KON		Shrunk date pixel length because new 10 point fonts were
									sneaking over the right edge.
		<11>	  7/3/94	KON		Make the date clip properly.
		<10>	 6/21/94	KON		Fix string lengths to prevent text overwriting for CES.
		 <9>	 6/20/94	KON		Update for CES look.
		 <8>	 6/10/94	KON		Update to use new screen dispatcher. Still needs work on
									DisplayMail prototype.
		 <7>	  6/5/94	KON		Tweak coordinates.
		 <6>	  6/4/94	KON		Add kFourWayMail screen.
		 <5>	  6/3/94	KON		Monkey with position info...should probably move this to the
									resource DB......
		 <4>	  6/2/94	KON		Moved position information to here from MailCntl.h.
		 <3>	 5/28/94	KON		Monkeyed around with mail rectangle.
		 <2>	 5/28/94	KON		Added mail rectangle.
		 <1>	 5/27/94	KON		first checked in

	To Do:
*/

#ifndef __MailView__
#define __MailView__

#ifndef __Mail__
#include "Mail.h"
#endif __Mail__

#define	kMailViewClutCycleRate	3

//
// ==BRAIN DAMAGE==
//Lame as shit: grab these guys from the DB!
//
#define kMailRectLeft	40
#define kMailRectRight	290
#define kMailRectTop	24
#define kMailRectBottom	152

//
// Positions in main mail dialog
//
// ==BRAIN DAMAGE==
//Lame as shit: grab these guys from the DB!
//
#define kMailFromPos			3
#define kMailFromLength			110
#define kMailTitlePos			121
#define kMailTitlePixelLength	118
#define kMailDatePos			198
#define kMailDatePixelLength	74


//
// Positions in read one piece of mail dialog
//
// ==BRAIN DAMAGE==
//Lame as shit: grab these guys from the DB!
//
#define kMailReadFromPos			20
#define kMailReadFromLength			70
#define kMailReadTitlePos			100
#define kMailReadTitlePixelLength	104
#define kMailReadDatePos			218
#define kMailReadDatePixelLength	74

enum
{
	kGeneralNews,
	kCatapultNews,
	kPlayerMail,
	kFourWayMailExit
};

// ==BRAIN DAMAGE==
// Fix me and make me real!!!
//
void DisplayMail( void );


#endif __MailView__



