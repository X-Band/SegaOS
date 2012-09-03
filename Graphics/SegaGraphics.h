/*
	File:		SegaGraphics.h

	Contains:	Basic graphics types

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 8/12/94	JOE		added RAMIconBitMap struct
		 <5>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		 <4>	 6/28/94	CMY		Added graphics references to handle clut and pattern management.
		 <3>	 6/15/94	CMY		Custom palettes for bitmaps.
		 <2>	 5/29/94	KON		Add xPos and yPos to bitmap data structure.
		 <1>	 5/19/94	SAH		first checked in

	To Do:
*/



#ifndef __SegaGraphics__
#define	__SegaGraphics__

#ifndef __SegaTypes__
#include "SegaTypes.h"
#endif

#define kCustomPaletteCompressor	0x8000


typedef
struct	SegaBitMap
{
	short			xPos;
	short			yPos;
	short			compressType;
	unsigned short	frames;
	unsigned short	width;
	unsigned short	height;
} SegaBitMap, *SegaBitMapPtr;


typedef 
struct RAMIconBitMap
{
	BoxSerialNumber	box;			//unique per box
	unsigned char	userID;			//0-3 for local, > kOpponentRangeStart for others
	unsigned char	pad;
	SegaBitMap		bitmap;	
} RAMIconBitMap, *RAMIconBitMapPtr;


typedef
struct segaCharRect
{
	short	left;
	short	top;
	short	right;
	short	bottom;
} segaCharRect, *segaCharRectPtr;

typedef
struct GraphicReferenceRec
{
	long	patternBlock;			/* Memory for pattern block */
	short	clut;					/* 0, or clut 1..3 if the graphic uses custom colors */
} GraphicReferenceRec, *GraphicReference;

#endif
