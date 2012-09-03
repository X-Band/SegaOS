/*
	File:		Sega.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <3>	 7/15/94	dwh		unix-ise.
		 <2>	 6/30/94	DJ		#ifndef for MWerks
		 <3>	 5/24/94	SAH		Fixed think non-simulator build problem.
		 <2>	 5/19/94	SAH		Moved sega graphics types to SegaGraphics.h

	To Do:
*/

#ifndef __Sega__
#define __Sega__

#ifndef __SegaGraphics__
#include "SegaGraphics.h"
#endif

#define SEGARealWidth		64
#define SEGAVisibleWidth 	40
#define SEGAHeight			28

#define kSpriteBase 0x580	//was 461

#ifndef SIMULATOR

#ifndef	unix
#ifndef __THINK__
#ifndef __MWERKS__

typedef struct Rect
{
	short top;
	short left;
	short bottom;
	short right;
} Rect;

typedef unsigned char *Ptr;

#endif
#endif
#endif	/* ! unix */

#endif

extern Rect	gSEGARect;

#endif	__Sega__
