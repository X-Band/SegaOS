/*
	File:		AddressCntl.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 7/27/94	KON		Dodge other used colors with animating color.
		 <5>	 7/27/94	KON		Change animating color since jizzle color changed.
		 <4>	 7/22/94	CMY		Fixed the constant
		 <3>	 7/22/94	CMY		Added a constant for the animating color. Use this to draw with,
									Kon.
		 <2>	  6/2/94	KON		Moved position information to AddressView.h.
		 <1>	  6/1/94	KON		first checked in

	To Do:
*/


#ifndef __AddressCntl__
#define __AddressCntl__

#ifndef __SegaBM__
#include "SegaBM.h"
#endif __SegaBM__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__

#ifndef	__Animation__
#include "Animators.h"
#endif __Animation__

#ifndef __UsrConfg__
#include "UsrConfg.h"
#endif __UsrConfg__

#ifndef __PlayerDB__
#include "PlayerDB.h"
#endif __PlayerDB__

#define kAddressCntlAnimatingColor	12

typedef struct AddressItemDesc
{
	unsigned char	color0;
	unsigned char	color1;
	unsigned char	color2;
	unsigned char	color3;
	unsigned char	palette;
	short			font;
	PlayerInfo		*theAddress;
} AddressItemDesc;

doDITLItemProc GetAddressItemPtr( void );


#endif __AddressCntl__
