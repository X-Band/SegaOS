/*
	File:		SegaUtils.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<26>	 8/17/94	KON		Change to use Joey font names.
		<25>	 8/15/94	CMY		Always initialize the font entries to a full byte width of data.
		<24>	 8/15/94	KON		Fix the lowercase 'v' in the headline font so the left edge
									doesn't get lopped off.
		<23>	 8/12/94	KON		Make headline font 18 point.
		<22>	 7/31/94	KON		kMain font is actually 13 pixels high! Fix the top clipping.
		<21>	 7/28/94	KON		Move the bold font up a pixel so the bottom doesn't get chopped
									off.
		<20>	 7/22/94	KON		Make XBandlight 10 point.
		<19>	 7/21/94	KON		Scrunch the fonts down.
		<18>	 7/20/94	KON		Font tweaks to push XBAND Light 9 and XBAND Bold 10 characters
									to upper left of cell.
		<17>	 7/20/94	KON		Adjust font spacing in XBandLight 9.
		<16>	 7/19/94	KON		Remap fonts so the first 32 characters are not in the set.
		<15>	 7/19/94	KON		Make fontheight for all fonts equal to the font size + 2.
		<14>	 7/18/94	KON		Fix the x-offset in the XBandHeavy font.
		<13>	 7/18/94	KON		Changed the order of shadow and character drawing for two bit
									fonts.
		<12>	 7/17/94	KON		Set fontadjust for kMainFont to please Joey.
		<11>	 7/17/94	KON		Update to new fonts.
		<10>	 7/16/94	KON		Added two more 10 point two bit fonts.
		 <9>	 7/11/94	KON		Added kBig2BitFont.
		 <8>	  7/8/94	KON		New headline font...serious hack!!!
		 <7>	  7/1/94	KON		Remove garbage from bottom two lines of headline font.
		 <6>	 6/29/94	KON		Added headline font -- Times for now.
		 <5>	  6/3/94	SAH		Cut the fonts down to a more reasonable size.
		 <4>	  6/2/94	KON		Try Espy Sans font for Joey.
		 <3>	  6/1/94	SAH		Changed Dom Casual spacing.
		 <2>	 5/27/94	KON		Use Dom Casual as 2-bit font.
		 <7>	 5/25/94	HEC		new memory manager
		 <6>	 5/24/94	SAH		Only build AllocateFont with the simulator. Removed dead
									DrawSegaChar and SetSegaPixel.
		 <5>	 5/20/94	KON		Fix bugs in 2-bit font allocation.
		 <4>	 5/20/94	KON		Added 2-bit font allocation. Not debugged yet!
		 <3>	 5/18/94	KON		Add get and set of multiple fonts and store them in the
									database.
		 <2>	 5/17/94	KON		Update to use SetFontColors.

	To Do:
*/

#include "Sega.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "SegaUtls.h"
#include "Errors.h"
#include "Heaps.h"
#include "SegaTextPriv.h"

//
// Support routines for SegaText.c
//

void DrawSegaStringOld( short *x, short *y, char * myPString );


#ifdef SIMULATOR
short	GetRemappedCharID( short theChar );

short	GetRemappedCharID( short theChar )
{
Handle	myResource;
short	returnVal;
short *	myFontMapper;

	myResource = GetResource( 'fmap', 0 );
	myFontMapper = (short *)*myResource;
	returnVal = myFontMapper[theChar];
	ReleaseResource( myResource );
	return( returnVal );
}

SegaFontTablePtr AllocateFont( FontID fontID )
{
SegaFontTablePtr	theFontTable;
short				iii;
short				jjj;
long				charOffset;
GWorldPtr			charGWorld;
Rect				charRect;
PixMapHandle		charPixMap;
CGrafPtr			savePort;
GDHandle			saveGDHandle;
Ptr					bitData;
short				rowBytes;
short				fontNum;
short				theSize;
short				bitDataIndex;

	GetGWorld( &savePort, &saveGDHandle );

	if( fontID != kHeadlineFont )
	{
		SetRect( &charRect, 0, 0, 16, 16 );
	}
	else
	{
		SetRect( &charRect, 0, 0, (kHeadlineFontWidth+7) & ~7, kHeadlineFontHeight );
	}
	
	NewGWorld( &charGWorld, 1, &charRect, 0, 0, 0 );
	
	SetFontColors( 12, 0, 0, 0 );

	theSize = MacCharSize;

	if( fontID == kHeadlineFont )
	{
		GetFNum( "\pXBAND Headline Black", &fontNum );
		theSize = kHeadlineFontPointSize;
		theFontTable = (SegaFontTablePtr) NewMemoryClear( false, sizeof( SegaFontTableHeadline ) );
		theFontTable->height = theSize;
		theFontTable->width = theSize+2;
		theFontTable->yLineSpacing = theSize+2;
	}
	else
	{
		ERROR_MESG( "AllocateFont called with illegal font num!" );
	}

	theFontTable->rowBytes = (theSize+2+7)/8;
	theFontTable->offset = theFontTable->rowBytes * theFontTable->height;
	theFontTable->bitDepth = 1;
	
	charPixMap = GetGWorldPixMap( charGWorld );
	LockPixels( charPixMap );
	bitData = GetPixBaseAddr( charPixMap );
	SetGWorld( charGWorld, 0 );
	rowBytes = (**charPixMap).rowBytes & 0x7fff;

	
	TextFont( fontNum );
	TextSize( theSize );
	
// run through the character set
	for( jjj = 0; jjj < kNumCharsUsed; jjj++ )
	{
	short	newChar;
	
		newChar = GetRemappedCharID( jjj );

		if( fontID != kHeadlineFont )
		{
			EraseRect( &charRect );
			MoveTo( 0, theSize - 2 );
			DrawChar( newChar );
		}
		else
		{
			EraseRect( &charRect );
			if( newChar == 'v' )
				MoveTo( 1, theSize - 4 );	//the lowercase 'v' is fucked up in the headline font!!
			else
				MoveTo( 0, theSize - 4 );
			DrawChar( newChar );
		}

		theFontTable->remappingTable[newChar] = (unsigned char)jjj;
		
		theFontTable->widthTable[jjj] = CharWidth( newChar );
		
		if( newChar == 'v' )
			theFontTable->widthTable[jjj]++;	//fucker!!
		
		if( fontID != kHeadlineFont )
		{
		// run through each character	
			for( iii = 0; iii < theFontTable->offset; iii++ )
			{
				theFontTable->fontTable[jjj*theFontTable->offset + iii] = bitData[(iii&0x01) + ((iii&0xfe)>>1)*rowBytes];
			}
		}
		else
		{
		// run through each character	
			for( iii = 0; iii < theFontTable->offset; iii++ )
			{
				bitDataIndex = (iii/theFontTable->rowBytes) * rowBytes + (iii%theFontTable->rowBytes);
				theFontTable->fontTable[jjj*theFontTable->offset + iii] = bitData[bitDataIndex];
			}
		}
	}
	SetGWorld( savePort, saveGDHandle );
	DisposeGWorld( charGWorld );

	return theFontTable;
}

SegaFontTablePtr Allocate2BitFont( FontID fontID )
{
SegaFontTablePtr	theFontTable;
short				iii;
short				jjj;
long				charOffset;
GWorldPtr			charGWorld;
Rect				charRect;
PixMapHandle		charPixMap;
CGrafPtr			savePort;
GDHandle			saveGDHandle;
Ptr					bitData;
short				rowBytes;
short				fontNum;
short				fontNum2;
CTabHandle			my2BitCTable;
RGBColor			gray1;
RGBColor			gray2;
short				fontSize;
short				fontAdjustValue;
short				bitDataIndex;
short				moveXPos1, moveXPos2;
short				moveYPos1, moveYPos2;
short				fontHeight;

	my2BitCTable = GetCTable( 34 );		//2-bit gray scale

	GetGWorld( &savePort, &saveGDHandle );

	if( fontID != kXBandHeavy )
	{
		SetRect( &charRect, 0, 0, 16, 16 );
	}
	else
	{
		SetRect( &charRect, 0, 0, kXBandHeavyWidth, kXBandHeavyHeight );
	}

	
	NewGWorld( &charGWorld, 2, &charRect, my2BitCTable, 0, 0 );
	
	SetFontColors( 0, 5, 10, 15 );

	
	charPixMap = GetGWorldPixMap( charGWorld );
	LockPixels( charPixMap );
	bitData = GetPixBaseAddr( charPixMap );
	SetGWorld( charGWorld, 0 );
	rowBytes = (**charPixMap).rowBytes & 0x7fff;
	fontSize = MacCharSize;
	fontAdjustValue = 0;
	
	moveXPos1 = 1;
	moveXPos2 = 0;
	moveYPos1 = MacCharSize;
	moveYPos2 = MacCharSize - 1;
	
	if( fontID == kMainFont )
	{
	//
	// This "12"-point font is actually 13 pixels high!
		fontAdjustValue = 1;
		fontSize = 12;
		moveXPos1 = moveXPos2;
		moveYPos1 = moveYPos2 = fontSize - 2;
		theFontTable = (SegaFontTablePtr) NewMemoryClear( false, sizeof( MainFontTable2Bit ) );
		theFontTable->yLineSpacing = fontSize+2;
		theFontTable->height = fontSize+1;
		theFontTable->width = fontSize+2;
		theFontTable->rowBytes = 4;
		theFontTable->offset = theFontTable->rowBytes * theFontTable->height;
		theFontTable->bitDepth = 2;
		GetFNum( "\pXBAND Bold Gray", &fontNum2 );
		GetFNum( "\pXBAND Bold Black", &fontNum );
		if( fontNum == 0 )
		{
			WARNING_MESG( "Install XBand Bold from the Hell server!!!" );
		}
	}
	else
	if( fontID == kXBandBold10Font )
	{
		fontAdjustValue = 1;
		fontSize = 10;
		moveXPos1 = moveXPos2 = -1;
		moveYPos1 = moveYPos2 = fontSize - 2;
		theFontTable = (SegaFontTablePtr) NewMemoryClear( false, sizeof( XBandBold10FontTable2Bit ) );
		theFontTable->yLineSpacing = fontSize+2;
		theFontTable->height = fontSize;
		theFontTable->width = fontSize+2;
		theFontTable->rowBytes = ((SEGACharSize+3)/4);
		theFontTable->offset = theFontTable->rowBytes * fontSize;
		theFontTable->bitDepth = 2;
		GetFNum( "\pXBAND Bold Gray", &fontNum2 );
		GetFNum( "\pXBAND Bold Black", &fontNum );
		if( fontNum == 0 )
		{
			WARNING_MESG( "Install XBand Bold from the Hell server!!!" );
		}
	}
	else
	if( fontID == kXBandLight9Font )
	{
		fontSize = 10;
		moveXPos1 = moveXPos2;
		moveYPos1 = moveYPos2 = fontSize - 2;
		fontAdjustValue = 1;
		theFontTable = (SegaFontTablePtr) NewMemoryClear( false, sizeof( XBandLight10FontTable2Bit ) );
		theFontTable->yLineSpacing = fontSize+4;
		theFontTable->height = fontSize;
		theFontTable->width = fontSize+2;
		theFontTable->rowBytes = ((SEGACharSize+3)/4);
		theFontTable->offset = theFontTable->rowBytes * fontSize;
		theFontTable->bitDepth = 2;
		GetFNum( "\pXBAND Light Gray", &fontNum2 );
		GetFNum( "\pXBAND Light Black", &fontNum );
		if( fontNum == 0 )
		{
			WARNING_MESG( "Install XBandLight from the Hell server!!!" );
		}
	}
	else
	if( fontID == kXBandHeavy )
	{
		fontSize = kXBandHeavyPointSize;
		moveXPos1 = moveXPos2;
		moveYPos1 = moveYPos2 = fontSize - 2;
		fontAdjustValue = 1;
		theFontTable = (SegaFontTablePtr) NewMemoryClear( false, sizeof( SegaFontTableBig2BitFont ) );
		theFontTable->yLineSpacing = fontSize+2;
		theFontTable->height = fontSize;
		theFontTable->width = fontSize+2;
		theFontTable->rowBytes = ((fontSize+3)/4);
		theFontTable->offset = theFontTable->rowBytes * fontSize;
		theFontTable->bitDepth = 2;
		GetFNum( "\pXBAND Heavy Black", &fontNum );
		GetFNum( "\pXBAND Heavy Gray", &fontNum2 );
		if( fontNum == 0 )
		{
			WARNING_MESG( "Install pXBandHeavy from the Hell server!!!" );
		}
	}
	else
	if( fontID == kMain2BitFont )
	{
		ERROR_MESG( "Should not be allocating kMain2BitFont." );
		fontAdjustValue = 0;
		fontSize = 14;
		moveXPos1 = moveXPos2;
		moveYPos1 = moveYPos2 = fontSize - 2;
		GetFNum( "\pDom Casual", &fontNum );
		if( fontNum == 0 )
		{
			fontAdjustValue = 0;
			fontSize = MacCharSize;
			GetFNum( "\pPalatino", &fontNum );
		}
		theFontTable = (SegaFontTablePtr) NewMemoryClear( false, sizeof( SegaFontTable2Bit ) );
		theFontTable->height = fontSize;
		theFontTable->width = fontSize+2;
		theFontTable->rowBytes = ((SEGACharSize+3)/4);
		theFontTable->offset = theFontTable->rowBytes * fontSize;
		theFontTable->bitDepth = 2;
		fontNum2 = fontNum;
	}
	else
	{
		ERROR_MESG( "Allocate2BitFont called with illegal font num!" );
	}
	
	TextSize( fontSize );

	gray1.red = 0x5555; 
	gray1.green = 0x5555; 
	gray1.blue = 0x5555; 

	gray2.red = 0xAAAA; 
	gray2.green = 0xAAAA; 
	gray2.blue = 0xAAAA; 
	
// run through the character set
	for( jjj = 0; jjj < kNumCharsUsed; jjj++ )
	{
	short	newChar;
	
		newChar = GetRemappedCharID( jjj );
		
		ForeColor( blackColor );
		EraseRect( &charRect );

//
// Draw shadow
//
		TextFont( fontNum2 );
		MoveTo( moveXPos2, moveYPos2 );
		RGBForeColor( &gray2 );
		DrawChar( newChar );

//
// Draw character
//
		TextFont( fontNum );
		RGBForeColor( &gray1 );
		MoveTo( moveXPos1, moveYPos1 );
		DrawChar( newChar );

		RGBForeColor( &gray1 );
		
		theFontTable->remappingTable[newChar] = (unsigned char)jjj;
		theFontTable->widthTable[jjj] = CharWidth( newChar ) - fontAdjustValue;

		if( fontID != kXBandHeavy )
		{
		// run through each character	
	
			for( iii = 0; iii < theFontTable->offset; iii++ )
			{
				theFontTable->fontTable[jjj*theFontTable->offset + iii] = bitData[(iii&0x03) + ((iii&0xfc)>>2)*rowBytes];
			}
		}
		else
		{
		// run through each character	
	
			for( iii = 0; iii < theFontTable->offset; iii++ )
			{
				bitDataIndex = (iii/theFontTable->rowBytes) * rowBytes + (iii%theFontTable->rowBytes);
				theFontTable->fontTable[jjj*theFontTable->offset + iii] = bitData[bitDataIndex];
			}
		}
	}


	SetGWorld( savePort, saveGDHandle );
	DisposeGWorld( charGWorld );

	return theFontTable;
}

#endif
