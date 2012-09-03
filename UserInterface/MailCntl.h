/*
	File:		MailCntl.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	  8/7/94	CMY		Updated rectangle coordinates for mail area.
		 <7>	 7/22/94	KON		UI poop.
		 <6>	 7/22/94	KON		Add color cycling.
		 <5>	  6/2/94	KON		Moved position info to MailView.h.
		 <4>	 5/29/94	KON		More monkeying.
		 <3>	 5/28/94	KON		Monkeyed around with mail positions.
		 <2>	 5/28/94	KON		Changed some layout stuff.
		 <1>	 5/27/94	KON		first checked in
	To Do:
*/

#ifndef __MailCntl__
#define __MailCntl__

#ifndef __SegaBM__
#include "SegaBM.h"
#endif __SegaBM__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__

#ifndef	__Animation__
#include "Animators.h"
#endif __Animation__

#ifndef	__Mail__
#include "Mail.h"
#endif __Mail__

#define kMailCntlAnimatingColor 12

typedef struct MailItemDesc
{
	unsigned char	color0;
	unsigned char	color1;
	unsigned char	color2;
	unsigned char	color3;
	unsigned char	palette;
	short			font;
	Mail			*theMail;
} MailItemDesc;

doDITLItemProc GetMailItemPtr( void );


#endif __MailCntl__
