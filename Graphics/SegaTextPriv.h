/*
	File:		SegaTextPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<14>	 8/17/94	KON		Remapping table now fully populated.
		<13>	 8/12/94	KON		Make headline font 18 point
		<12>	 7/31/94	KON		kMain font is actually 13 pixels high! Fix the struct so the
									tops don't get clipped off.
		<11>	 7/22/94	KON		Bump xband light font to 10 point.
		<10>	 7/21/94	KON		Scrunch fonts down a little.
		 <9>	 7/19/94	KON		Remap fonts so the first 32 characters are not in the set.
		 <8>	 7/17/94	KON		Update for new fonts.
		 <7>	 7/14/94	ADS		Remove placeholder
		 <6>	 7/13/94	ADS		Vectorize EVERYTHING
		 <5>	 7/11/94	KON		Moved a bunch of font generation constants and types to this
									file to make iterative compiling faster.
		 <4>	 6/29/94	KON		Add headline font.
		 <3>	 6/13/94	CMY		Added tiling information to the TextGDevice. This allows us to
									blit text into Sprites.
		 <2>	  6/4/94	CMY		Added 1->2 and 1->4 colorizing pixel expansion tables to text
									globals.
		 <1>	 5/24/94	SAH		first checked in

	To Do:
*/


#ifndef SEGA_PRIV_TEXT_H
#define SEGA_PRIV_TEXT_H


#define MacCharSize	12
#define SEGACharSize 15

#define kHeadlineFontPointSize 18
#define kXBandHeavyPointSize 18

#define kHeadlineFontWidth (kHeadlineFontPointSize+2)
#define kHeadlineFontHeight (kHeadlineFontPointSize+2)

#define kXBandHeavyWidth (kXBandHeavyPointSize+2)
#define kXBandHeavyHeight (kXBandHeavyPointSize+2)


#define kMaxTextHeight		kHeadlineFontHeight
#define	kRenderBufferSize	( 320 * kMaxTextHeight / 2 )	/* Width*Height in 4-bit pixels */
#define kCellRowBytes		4								/* Bump address to next row */
#define kCellSizeBytes		32L								/* Size of a 4-bit char cell */
															/* Bump address to next horizontal cell */
#define kCellCellBytes		(REFGLOBAL(text,fontWorld).charScreenHeight+1) * kCellSizeBytes

typedef struct SegaFontWorld
{
	short			color0;					//pixel value for 0-bits
	short			color1;					//pixel value for 1-bits in Copy mode
	short			color2;					//pixel value for 2-bits
	short			color3;					//pixel value for 3-bits in Copy mode
	short			charStartX;
	short			charStartY;
	short			charScreenWidth;
	short			charScreenHeight;
	short			visibleWidth;
	short			verticalTileSize;		//number of pattern cells per vertical tile
	Ptr				screenBase;
} SegaFontWorld;

//
// Red-headed step children
//

typedef struct SegaFontTableHeadline
{
	short	height;							//y-size in pixels
	short	width;							//x-size in pixels
	short	yLineSpacing;					//duh
	short	offset;							//offset between characters in bytes
	short	rowBytes;						//each char has same rowBytes
	short	bitDepth;						//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ ((kHeadlineFontPointSize+2+7)/8) * kHeadlineFontPointSize * kNumCharsUsed ];
} SegaFontTableHeadline, *SegaFontTableHeadlinePtr;

typedef struct SegaFontTableBig2BitFont
{
	short	height;							//y-size in pixels
	short	width;							//x-size in pixels
	short	yLineSpacing;					//duh
	short	offset;							//offset between characters in bytes
	short	rowBytes;						//each char has same rowBytes
	short	bitDepth;						//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ ((kXBandHeavyWidth+2+7)/4) * kXBandHeavyPointSize * kNumCharsUsed ];
} SegaFontTableBig2BitFont, *SegaFontTableBig2BitFontPtr;


typedef struct SegaFontTable2Bit
{
	short	height;							//y-size in pixels
	short	width;							//x-size in pixels
	short	yLineSpacing;					//duh
	short	offset;							//offset between characters in bytes
	short	rowBytes;						//each char has same rowBytes
	short	bitDepth;						//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ (1+(FontWidth>>2)) * FontHeight * kNumCharsUsed ];
} SegaFontTable2Bit, *SegaFontTable2BitPtr;

typedef struct MainFontTable2Bit
{
	short	height;							//y-size in pixels
	short	width;							//x-size in pixels
	short	yLineSpacing;					//duh
	short	offset;							//offset between characters in bytes
	short	rowBytes;						//each char has same rowBytes
	short	bitDepth;						//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ (4) * 13 * kNumCharsUsed ];	//this font is 13 pixels high!
} MainFontTable2Bit, *MainFontTable2BitPtr;

typedef struct XBandBold10FontTable2Bit
{
	short	height;							//y-size in pixels
	short	width;							//x-size in pixels
	short	yLineSpacing;					//duh
	short	offset;							//offset between characters in bytes
	short	rowBytes;						//each char has same rowBytes
	short	bitDepth;						//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ (4) * 10 * kNumCharsUsed ];
} XBandBold10FontTable2Bit;

typedef struct XBandLight10FontTable2Bit
{
	short	height;							//y-size in pixels
	short	width;							//x-size in pixels
	short	yLineSpacing;					//duh
	short	offset;							//offset between characters in bytes
	short	rowBytes;						//each char has same rowBytes
	short	bitDepth;						//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ (4) * 10 * kNumCharsUsed ];
} XBandLight10FontTable2Bit;


typedef
struct TextGlobals
{
	SegaFontTablePtr	theFont;
	SegaFontWorld		fontWorld;
	FontID				curFontID;
	Ptr					renderBuffer;
	unsigned short		*expansionTable1Bit;
	unsigned short		*expansionTable2Bit;
} TextGlobals;




// internal functions, vectorized for patching


void RenderSegaString( short *x, short *y, char * myCString, short justify, long textWidth, long renderWidth ) = 
	CallDispatchedFunction( kRenderSegaString );

void VDPCopyBlitDirect( BitsPtr src, short x, short y ) =
	CallDispatchedFunction( kVDPCopyBlitDirect );

void VDPCopyBlitDirectBGColor( BitsPtr src, short x, short y, short bgColor) =
	CallDispatchedFunction( kVDPCopyBlitDirectBGColor );

void VDPCopyBlitTiled( BitsPtr src, short x, short y) =
	CallDispatchedFunction( kVDPCopyBlitTiled );

void VDPCopyBlitTiledBGColor( BitsPtr src, short x, short y, short bgColor) =
	CallDispatchedFunction( kVDPCopyBlitTiledBGColor );

void OrBlit2to4( BitsPtr src, BitsPtr dst, short xPos ) =
	CallDispatchedFunction( kOrBlit2to4 );

void OrBlit1to4( BitsPtr src, BitsPtr dst, short xPos ) =
	CallDispatchedFunction( kOrBlit1to4 );








#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE	TextGlobals
#else
extern TextGlobals text;
#endif

#endif			// SEGA_PRIV_TEXT_H
