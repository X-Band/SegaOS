/*
	File:		SegaIcons.h

	Contains:	xxx put contents here xxx

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):
	
		 <7>	 8/16/94	CMY		Added a range of reserved icons for use only by Catapult, not as
									player icons.
		 <6>	 8/12/94	JOE		Mods for better Custom Icon scheme
		 <5>	  8/3/94	JOE		changed IconSetup to be just a frameDelay value (short)
		 <4>	 7/18/94	KON		not sure what I did.
		 <3>	  7/3/94	JOE		revamp for IDSC's and ISEG's
		 <2>	  7/2/94	JOE		whoopsy
		 <1>	  7/2/94	JOE		first checked in
	
	To Do:
*/

#ifndef	__SegaIcons__
#define __SegaIcons__


#ifndef	__Animators__
#include "Animators.h"
#endif

/*
 * Special Player Icons
 */
enum {
	kFirstReservedIcon = 200,
	kNewPlayerIcon = kFirstReservedIcon,
	kXBANDPlayerIcon
};


#define	kStaticGraphicIcon		0
#define	kAnimationIcon			1

#define	kGrayIconFlag			0x1

typedef
struct IconReferenceRec
{
	short			graphicsType;		/* static graphic or animation */
	FrameDelayData	animationData;		/* gets filled with IDSC contents */
	long			reference;			/* a GraphicReference or an AnimationRef */
} IconReferenceRec, *IconReference;


typedef struct IconSetup				/* this is an IDSC resource */
{
	unsigned short	frameDelay;
} IconSetup;



#endif __SegaIcons__


