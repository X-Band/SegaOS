/*
	File:		StatIcon.h

	Contains:	Static Icon (animated) screen layout item stuff header

	Written by:	Joe Britt 

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <1>	 6/30/94	JOE		first checked in

	To Do:
*/

#ifndef __StatIcon__
#define __StatIcon__

#ifndef __SegaBM__
#include "SegaBM.h"
#endif __SegaBM__

#ifndef __DitlMgr__
#include "DitlMgr.h"
#endif __DitlMgr__

#ifndef	__Animation__
#include "Animation.h"
#endif __Animation__

#ifndef	__Animators__
#include "Animators.h"
#endif __Animators__


typedef struct StatIconPriv
{
	AnimationRef	animation;
} StatIconPriv;

typedef struct StatIconDesc
{
	FrameDelayData	animationData;
	unsigned char	textXCenter;
	unsigned char	textYTop;
	unsigned char	pixelWidth;
	unsigned char	color0;
	unsigned char	color1;
	unsigned char	color2;
	unsigned char	color3;
	unsigned char	palette;
	short			font;
	char			*myCString;
} StatIconDesc;

typedef struct StatIconSetup
{
	FrameDelayDataSetup	animationData;
	unsigned char		textXCenter;
	unsigned char		textYTop;
	unsigned char		pixelWidth;
	unsigned char		color0;
	unsigned char		color1;
	unsigned char		color2;
	unsigned char		color3;
	unsigned char		palette;
	short				font;
	DBID				myCStringID;
} StatIconSetup;

doDITLItemProc GetStatIconItemPtr( void );


#endif __StatIcon__
