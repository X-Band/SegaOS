/*
	File:		SegaText.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<24>	 8/22/94	JOE		added kBiggestFontID
		<23>	 8/17/94	KON		Fonts now have special characters above 128, so the remapping
									table gets bigger.
		<22>	 7/21/94	KON		Added GetCurFontLineHeight.
		<21>	 7/21/94	KON		Added format parameter to num to date.
		<20>	 7/19/94	KON		Remap fonts so the first 32 characters are not in the set.
		<19>	 7/17/94	KON		Update for new fonts.
		<18>	 7/16/94	KON		Added kXBandLight12Font and kXBandBold12Font.
		<17>	 7/15/94	dwh		unix-ise.
		<16>	 7/11/94	KON		Moved a bunch of stuff to SegaTextPriv.h to make iterative
									compiling faster and added kBig2BitFont.
		<15>	 7/10/94	SAH		Added DrawPaddedClippedSegaText.
		<14>	  7/9/94	JBH		Added text state save and restore routines.
		<13>	  7/4/94	KON		Add compare strings.
		<12>	 6/29/94	KON		Add headline font.
		<11>	 6/17/94	KON		Add EqualCString.
		<10>	 6/17/94	CMY		Added SetTextPatternStart
		 <9>	 6/15/94	KON		Add CopyCString and GetStringLength.
		 <8>	 6/13/94	CMY		Added EraseTextGDevice
		 <7>	 6/13/94	CMY		Added TextSprites.
		 <6>	  6/3/94	DJ		moved Date macros to Dates.h
		 <5>	  6/3/94	DJ		Macros for setting the Date
		 <4>	  6/3/94	SAH		Fonts have kNumCharsUsed chars.
		 <3>	  6/2/94	KON		Added SegaAppendText.
		 <2>	 5/31/94	SAH		Brought date stuff in from SegaMisc.
		<10>	 5/24/94	SAH		Removed an extern declaration of gTheFuck, oops, Font.
		 <9>	 5/24/94	SAH		Made a real manager.
		 <8>	 5/24/94	KON		Add GetCurFontHeight.
		 <7>	 5/20/94	SAH		Made GetCurFont take no parameters
		 <6>	 5/20/94	KON		Added 2-bit font allocation. Not debugged yet!
		 <5>	 5/18/94	KON		Add support for multiple fonts and fontworld (rather than
									device).
		 <4>	 5/18/94	HEC		long pattern types
		 <3>	 5/18/94	SAH		Made a public GetTextPatternAddress.
		 <2>	 5/17/94	KON		Updated to use new four color text model.

	To Do:
*/

#ifndef __SegaText__
#define __SegaText__

#ifndef __SegaOS__
#include "SegaOs.h"
#endif

#ifndef __SegaScrn__
#include "SegaScrn.h"
#endif __SegaScrn__

#ifndef __Dates__
#include "Dates.h"
#endif __Dates__


#ifdef SIMULATOR

#include <QDOffscreen.h>

#endif

/*
*	Justifcation values for DrawPaddedClippedSegaText
*/
enum
{
	kJustifyLeft,
	kJustifyCenter,
	kJustifyRight
};

#define FontWidth 14
#define FontHeight 15

#define kMainFont 0
#define kXBandBold10Font 1
#define kXBandHeavy 2
#define kXBandLight9Font 6

#define	kBiggestFontID	kXBandLight9Font	/* Be sure to bump this fucker */

#define kMain2BitFont kMainFont

//#define kAlternateFont 1
//#define kMain2BitFont 2
#define kAlternate2BitFont 3

#define kHeadlineFont 4
#define kCharSkipAdjust	32
#define	kNumCharsUsed		(128+16 - kCharSkipAdjust)

#define kLongDateFormat true
#define kShortDateFormat false

//
// ==BRAIN DAMAGE==
// NOTE: There are other versions of these Sega font tables in SegaUtils.c. They are nasty
// little bastards, and I haven't fixed them since they are used to generate the fonts
// one time on the Simulator and that's it. Adding different size fonts or different point
// sizes is hard because the code is such shit. This should be fixed some day...
//

typedef struct SegaFontTable
{
	short	height;									//y-size in pixels
	short	width;									//x-size in pixels
	short	yLineSpacing;							//duh
	short	offset;									//offset between characters in bytes
	short	rowBytes;								//each char has same rowBytes
	short	bitDepth;								//font bit depth - currently 1 or 2
	unsigned char	remappingTable[256];	//map from characters to table offsets
	unsigned char	widthTable[kNumCharsUsed];
	unsigned char	fontTable[ (1+(FontWidth>>3)) * FontHeight * kNumCharsUsed ];
} SegaFontTable, *SegaFontTablePtr;


typedef struct Bits
{
	Ptr		address;
	short	rowBytes;
	short	xBounds;
	short	yBounds;
} Bits, *BitsPtr;

typedef short FontID;
	

#ifndef	unix
void InitSegaFonts( void ) = 
	CallDispatchedFunction( kInitSegaFonts );

FontID SetCurFont( FontID newID ) = 
	CallDispatchedFunction( kSetCurFont );

FontID GetCurFont( void ) = 
	CallDispatchedFunction( kGetCurFont );

short GetCurFontHeight( void ) = 
	CallDispatchedFunction( kGetCurFontHeight );

short GetCurFontLineHeight( void ) = 
	CallDispatchedFunction( kGetCurFontLineHeight );

void SetFontColors( short color0, short color1, short color2, short color3 ) = 
	CallDispatchedFunction( kSetFontColors );

void GetFontColors( short *color0, short *color1, short *color2, short *color3 ) = 
	CallDispatchedFunction( kGetFontColors );

void SetupTextGDevice( segaCharRect *myRect, long patternStart ) = 
	CallDispatchedFunction( kSetupTextGDevice );

void EraseTextGDevice( short bgColor ) =
	CallDispatchedFunction( kEraseTextGDevice );
	
Ptr GetTextPatternAddress ( short x, short y, short *scanlinesLeft ) = 
	CallDispatchedFunction( kGetTextPatternAddress );

void GetTextGDeviceOrigin( short *X, short *Y ) = 
	CallDispatchedFunction( kGetTextGDeviceOrigin );

void DrawSegaString( short *x, short *y, char * myCString ) = 
	CallDispatchedFunction( kDrawSegaString );

short MeasureSegaText( char * myCString ) = 
	CallDispatchedFunction( kMeasureSegaText );

void CenterSegaText( short y, char * myCString ) = 
	CallDispatchedFunction( kCenterSegaText );

void DrawClippedSegaText( short x, short y, short pixelWidth, char * myCString ) = 
	CallDispatchedFunction( kDrawClippedSegaText );

void DrawCenteredClippedSegaText( short xCenter, short y, short pixelWidth, char * myCString ) = 
	CallDispatchedFunction( kDrawCenteredClippedSegaText );

void DrawPaddedClippedSegaText( short x, short y, short padWidth, short justify, char *string ) =
	CallDispatchedFunction( kDrawPaddedClippedSegaText );

short GetCharWidth( unsigned char theChar ) =
	CallDispatchedFunction( kGetCharWidth );

void SegaNumToString( long theNum, char *theString ) =
	CallDispatchedFunction( kSegaNumToString );

void SegaNumToDate( long theDate, char *theString, Boolean format ) =
	CallDispatchedFunction( kSegaNumToDate );

void SegaAppendText( char *src, char *append, char* dst ) =
	CallDispatchedFunction( kSegaAppendText );

unsigned short GetStringLength( char *src ) =
	CallDispatchedFunction( kGetStringLength );

short CompareDates( long date1, long date2 ) =
	CallDispatchedFunction( kCompareDates );

short CompareStrings( char *cString1, char *cString2 ) =
	CallDispatchedFunction( kCompareStrings );

void SetupTextSpriteGDevice( segaCharRect *myRect, long patternStart, short verticalTileSize ) =
	CallDispatchedFunction( kSetupTextSpriteGDevice );

void CopyCString( char *src, char *dst ) =
	CallDispatchedFunction( kCopyCString );

Boolean EqualCStrings( char *src, char *dst ) =
	CallDispatchedFunction( kEqualCStrings );

void SetTextPatternStart( Ptr mems) =
	CallDispatchedFunction( kSetTextPatternStart );
	
void *	GetTextStateReference( void ) =
	CallDispatchedFunction( kGetTextStateReference );
	
void	SaveTextState( void *textStateRef ) =
	CallDispatchedFunction( kSaveTextState );
	
void	RestoreTextState( void *textStateRef ) =
	CallDispatchedFunction( kRestoreTextState );
	
void	DisposeTextStateReference( void *textStateRef ) =
	CallDispatchedFunction( kDisposeTextStateReference );	
#endif	/* ! unix */

#endif  __SegaText__
