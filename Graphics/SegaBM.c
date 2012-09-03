/*
	File:		SegaBM.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 8/21/94	KON		CreateSpriteInFront now takes a hueShift.
		<12>	 8/21/94	JBH		Actually implemented hueShift. She ain't pretty, but she works!
		<11>	 8/20/94	KON		fixed typo...compile, then check in.
		<10>	 8/20/94	KON		Added hueShift to DrawGraphic rather than taking a new color
									table. Stubbed out the HueShift function so Josh can do all the
									real work!
		 <9>	 8/20/94	KON		DrawGraphic now takes a clutID.
		 <8>	  8/8/94	KON		Mark backdrop dirty if graphics are drawn into scrollB.
		 <7>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		 <6>	 7/21/94	CMY		Added DrawGraphicGray.
		 <5>	 7/18/94	CMY		It now asks the decompressor what clut the image wants instead
									of figuring it out by itself.
		 <4>	 6/28/94	CMY		Added graphics references to handle clut and pattern management.
		 <3>	 6/15/94	CMY		Support custom palettes for bitmaps.
		 <2>	 6/13/94	SAH		Managerized.
		 <5>	 5/23/94	SAH		Updated to new CreateDecompressor interface.
		 <4>	 5/19/94	SAH		Made use the decompression manager.
		 <3>	 5/18/94	HEC		long pattern types
		 <2>	 5/11/94	KON		Updated to map screen cells vertically instead of horizontally.

	To Do:
*/

#include "Sega.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaVDP.h"
#include "Heaps.h"
#include "vdp.h"
#include "SegaOS.h"
#include "Decompress.h"
#include "Errors.h"
#include "Decompressors.h"
#include "MiscDBItemsPriv.h"
#include "Backdrops.h"
#include "DBTypes.h"
#include "GraphicsDB.h"

GraphicReference _DrawGraphicGray( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse, long decompressorFlags );
short _HueShift( short color, unsigned char shiftAmount );

/*
*	The dispatched calls in here are part of the misc db items mgr. they are installed in
*	GraphicsDB.c
*/

//
// Draws the graphic to the current device at character location xPos and yPos
//

GraphicReference _DrawGraphic( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse, long decompressorOptions, unsigned char hueShiftAmount )
{
short xxx,yyy;
short	charWalker;
unsigned long screenBase;
Ptr 	sourcePtr;	
short	drawHeight;
short	vCells;
long 	temp;
short	uniqueChars;
short	totalChars;
unsigned long	charsToSkip;
Decompressor	dec;
short	palette = 0;			// If unspecified, use the default palette
GraphicReference ref;
short	clut[16];
short	iii;
short	*colors;
	
	if( GetCurrentDevice() == kScrollB )
		ClearBackdrop( false );		//mark backdrop dirty, but don't erase
			
	dec = CreateDecompressor ( myGraphic, palette, 1 );
	ASSERT_MESG ( dec, "Can't allocate decompressor" );
	if ( !dec )
		{
		return nil;
		}
	
//
// if we are forcing a clut, use that
// otherwise, get the clut from the image
//
	if (GetImageClut(dec, clut) == 0)
	{
		if( hueShiftAmount != 0 )
		{
			for( iii = 0; iii < 16; iii++ )
			{
				clut[iii] = HueShift( clut[iii], hueShiftAmount );
			}
		}

		palette = RequestClut(clut);
		SetDecompressorPalette(dec, palette);
	}
	
	SetDecompressorOptions( dec, decompressorOptions );

	memToUse = SetDecompressDstPattern ( dec, memToUse );
	SetImageOrigin ( dec, xPos, yPos );
	
	DecompressFrame ( dec, 0 );
	
	DisposeDecompressor ( dec );
	
	/* Now build the reference to return to the caller */
	ref = (GraphicReference) NewMemory(kTemp, sizeof(GraphicReferenceRec));
	if (ref)
	{
		ref->patternBlock = memToUse;
		ref->clut = palette;
	}
	VDPIdle();

	return ref;
}


/* Converts the color into HSV space, bumps hue by shiftAmount, back to RGB color space */
/* Warning - much nasty shifting ahead. */
/* See Foley & Van Dam, 2nd ed., p. 590 */
short _HueShift( short color, unsigned char shiftAmount )
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;

	unsigned short max;
	unsigned short min;
	unsigned short delta;
	
	unsigned short sat, hue;

	unsigned short p, q, t;

	short hextant;

	long tempDelta;

	if ( shiftAmount == 0 )
		return color;
	
	red = ( color & 0xe ) << 12;
	green = ( color & 0xe0 ) << 8;
	blue = ( color & 0xe00 ) << 4;
	
	/* The color is grayscale, so merely return it */
	if ( red == green && green == blue )
		return color;
	
	red |= red >> 3;
	red |= red >> 6;
	red |= red >> 6;

	green |= green >> 3;
	green |= green >> 6;
	green |= green >> 6;

	blue |= blue >> 3;
	blue |= blue >> 6;
	blue |= blue >> 6;
	
	max = red;
	if ( green > max )
		max = green;
	if ( blue > max )
		max = blue;

	min = red;
	if ( green < min )
		min = green;
	if ( blue < min )
		min = blue;
	
	delta = max - min;

	/* The << 15 is so we don't end up in the sign bit of the longword */
	if ( min == 0 )
		sat = 0xffff;
	else
	{
		sat = ( (long) delta << 15 ) / max;		/* sat is a 16-bit fract */
		sat <<= 1;
	}

	ASSERT_MESG( sat, "didn't expect non-saturated color here!");

	if ( red == max )
	{
		tempDelta = (long) green - blue;
		
		if ( tempDelta == 65535 )		/* yellow */
			hue = 0x2aaa;
		else if ( tempDelta == -65535)	/* magenta */
			hue = 0xd556;
		else
			tempDelta = (tempDelta << 15)  / delta;
			hue = tempDelta / 3;
	}
	else if ( green == max )
	{
		tempDelta = (long) blue - red;
		
		if ( tempDelta == 65535 )
			hue = 0x8000;			/* cyan */
		else
		{
			tempDelta = ( tempDelta << 15 ) / delta;
			tempDelta /= 3;
			hue = tempDelta + 0x5555;
		}
	}
	else
	{
		tempDelta = (long) red - green;
		
		tempDelta = (tempDelta << 15) / delta;
		tempDelta /= 3;
		hue = tempDelta + 0xAAAA;
	}

	/* rotate the hue around! */
	hue += (shiftAmount << 8) | shiftAmount;
	
	/* now back to RGB space */
	hextant = hue / 0x2aaa;
	
	hue = hue - (hextant * 0x2aaa);
	hue *= 6;
	
	p = ((long) max * (0xffff-sat)) >> 16;

	sat >>= 1;		/* so we don't overflow into the sign bit */
	hue >>= 1;
	
	q = ((long) max * (0xffff-(((long) sat * hue)>>14))) >> 16;
	hue = 0x7fff-hue;
	t = ((long) max * (0xffff-(((long) sat * hue)>>14))) >> 16;
	
	switch (hextant)
	{
		case 0:
			red = max;
			green = t;
			blue = p;
			break;
		case 1:
			red = q;
			green = max;
			blue = p;
			break;
		case 2:
			red = p;
			green = max;
			blue = t;
			break;
		case 3:
			red = p;
			green = q;
			blue = max;
			break;
		case 4:
			red = t;
			green = p;
			blue = max;
			break;
		case 5:
			red = max;
			green = p;
			blue = q;
			break;
	}
	
	color = (( blue & 0xE000 ) >> 4) | (( green & 0xE000 ) >> 8) | (( red & 0xE000) >> 12);
	
	return color;
}

GraphicReference _DrawGraphicGray( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse, long decompressorOptions )
{
short			xxx,yyy;
short			charWalker;
unsigned		long screenBase;
Ptr 			sourcePtr;	
short			drawHeight;
short			vCells;
long 			temp;
short			uniqueChars;
short			totalChars;
unsigned long	charsToSkip;
Decompressor	dec;
GraphicReference ref;
short			clut[16];
char			grayMapping[16];
	

	dec = CreateDecompressor ( myGraphic, 0, 1 );
	ASSERT_MESG ( dec, "Can't allocate decompressor" );
	if ( !dec )
		{
		return nil;
		}
		
	// Generate the gray map for the clut
	GetImageClut(dec, clut);
	GenerateGrayMap(clut, grayMapping);
	SetDecompressorPixelMapping(dec, grayMapping);
	
	SetDecompressorOptions( dec, decompressorOptions );
	memToUse = SetDecompressDstPattern ( dec, memToUse );
	SetImageOrigin ( dec, xPos, yPos );
	DecompressFrame ( dec, 0 );
	
	DisposeDecompressor ( dec );
	
	/* Now build the reference to return to the caller */
	ref = (GraphicReference) NewMemory(kTemp, sizeof(GraphicReferenceRec));
	if (ref)
	{
		ref->patternBlock = memToUse;
		ref->clut = 0;
	}
	VDPIdle();
	
	return ref;
}
