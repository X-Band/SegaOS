/*
	File:		SegaText.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<54>	 8/17/94	KON		Last checkin had bogus characters in it.
		<53>	 8/17/94	KON		Remove asserts that don't make sense. Support fonts with greater
									than 128 characters.
		<52>	 8/15/94	KON		Make set the font colors to grey only happen on debug versions.
		<51>	 8/13/94	CMY		Fixed a problem with rendering text that ends exactly on a cell
									boundary. We would wipe over 1 extra pattern cell.
		<50>	  8/9/94	JBH		Sped up clearing of renderBuffer in RenderSegaString. Fixed a
									nasty bug where font colors were uninitialized on softInit,
									leaving bad expansionTables around!
		<49>	  8/4/94	CMY		Really fixed the RenderSegaText bug. It wasn't taking alignment
									calculations into account when figuring out the rendering width.
		<48>	  8/4/94	CMY		Fixed a bug in RenderSegaText where it would write 1 past the
									last pattern.
		<47>	  8/3/94	CMY		Fixed RestoreFontReference to rebuild the expansion tables
									correctly when the font colors change.
		<46>	 7/31/94	KON		Add writeable string calls.
		<45>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<44>	 7/24/94	KON		Cache the fonts.
		<43>	 7/21/94	KON		Added GetCurFontLineHeight.
		<42>	 7/21/94	KON		Added format parameter to sega num to date.
		<41>	 7/19/94	KON		Fix bugs related to font remapping: the first 32 characters are
									not in the bitmap now.
		<40>	 7/17/94	KON		Update for new fonts.
		<39>	 7/16/94	KON		Install kXBandLight12Font and kXBandBold12Font.
		<38>	 7/13/94	ADS		Vectorize EVERYTHING
		<37>	 7/13/94	ADS		Add ASSERT's for some problem cases
		<36>	 7/11/94	KON		Add allocation of kBig2BitFont.
		<35>	 7/10/94	SAH		Fixed a bug I introduced with DrawClippedSegaText and
									DrawCenteredClippedSegaText - I was passing the clipped with not
									the real text width to RenderSegaString.
		<34>	 7/10/94	SAH		Moved actual text rendering into RenderSegaText and added
									DrawPaddedClippedSegaString.
		<33>	  7/9/94	JBH		Added text state saving and restoring calls.
		<32>	  7/7/94	KON		Make 1 map to kNever string.
		<31>	  7/6/94	ADS		assert hi-ascii chars in strings (we don't print them)
		<30>	  7/5/94	KON		Make zero date the first special date string.
		<29>	  7/4/94	KON		Add compare strings.
		<28>	 6/30/94	SAH		Removed database purge values.
		<27>	 6/29/94	KON		Added headline font.
		<26>	 6/27/94	SAH		Make SoftInit return noErr.
		<25>	 6/17/94	KON		Added EqualCString.
		<24>	 6/17/94	CMY		SetTextPatternStart
		<23>	 6/15/94	CMY		Brought the transparent text->sprite blit in sync with the
									bgColor blit.
		<22>	 6/15/94	KON		Add CopyCString and GetStringLength.
		<21>	 6/13/94	CMY		Fixed tiled text blits.
		<20>	 6/13/94	CMY		Fixed text so it works with background colors. Fixed a bug in
									GetTextPatternAddress.
		<19>	 6/13/94	SAH		Managerized StringDB functions.
		<18>	 6/13/94	CMY		Added TextSprites.
		<17>	 6/12/94	SAH		Optimized VDPCopyBlitDirect some.
		<16>	 6/11/94	CMY		Remove static "beenHere" from InitSegaFonts. A reset during
									unstable OS will necessitate another call to InitSegaFonts.
		<15>	 6/11/94	CMY		More optimizations: use char width instead of font width when
									blitting a character.
		<14>	 6/11/94	CMY		Don't rebuild pixel expansion table when SetFontColors sets to
									the same colors. This happens quite often.
		<13>	  6/4/94	CMY		Speeded up text blitters (colorize, expand, and pattern map text
									to DRAM, then do a fast copy to VRAM)
		<12>	  6/3/94	KON		Made NumToDate subtract 1900 if the year is in the 20th century
									and replace it with a '.
		<11>	  6/2/94	KON		Added AppendSegaText.
		<10>	  6/1/94	KON		Make NumToString handle zero.
		 <9>	 5/31/94	KON		Fixed DrawCenteredClippedSegaText and DrawClippedSegaText so
									they don't crash when bogus or really long strings are passed
									in.
		 <8>	 5/31/94	SAH		Brought date stuff in from SegaMisc.
		 <7>	 5/30/94	SAH		Kill all of InitSegaFonts for sega build.
		 <6>	 5/28/94	SAH		Killed Alternate2bit font to save room in the ROM for digital
									world.
		 <5>	 5/27/94	SAH		Fixed an assert for the sega build.
		 <4>	 5/26/94	SAH		Globals are set up for us now at soft initialize.
		 <3>	 5/26/94	SAH		Simulator conditional on InitSegaFonts.
		 <2>	 5/26/94	SAH		Dispose of the temp memory in InitSegaFonts.
		<12>	 5/24/94	SAH		Made a real manager.
		<11>	 5/24/94	KON		Add GetCurFontHeight.
		<10>	 5/20/94	SAH		Made GetCurFont take no parameters.
		 <9>	 5/20/94	KON		Fixed 2-bit font bugs.
		 <8>	 5/20/94	KON		Added 2-bit font allocation. Not debugged yet!
		 <7>	 5/19/94	KON		Fix up from bad merge on last checkin.
		 <6>	 5/18/94	KON		Update to use SegaFontWorld and multiple fonts.
		 <5>	 5/18/94	HEC		long pattern types
		 <4>	 5/18/94	SAH		Made a public GetTextPatternAddress.
		 <3>	 5/17/94	KON		Update to use four color model.
		 <2>	 5/11/94	KON		Update to render to verticle pattern cells.

	To Do:
*/

#include "Sega.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "SegaTextPriv.h"
#include "SegaUtls.h"
#include "SegaVDP.h"
#include "VDP.h"
#include "DataBase.h"
#include "DBTypes.h"
#include "Errors.h"
#include "Pattern.h"
#include "Heaps.h"
#include "StringDB.h"
#include "StringDBPriv.h"

#ifdef SIMULATOR
	#include <StdIO.h>
#endif

//
//Prototypes -- internal functions
//

#ifdef oldText
void VDPCopyBlit1To4( BitsPtr src, short x, short y, short color0, short color1 );
void VDPCopyBlit2To4( BitsPtr src, short x, short y, short color0, short color1, short color2, short color3 );
void OrBlit1to1( BitsPtr src, BitsPtr dst, short xPos );
void OrBlit2to2( BitsPtr src, BitsPtr dst, short xPos );
#endif

//
// Prototypes - dispatched functions
//

void	_InitSegaFonts( void );
FontID	_SetCurFont( FontID newID );
FontID	_GetCurFont( void );
short	_GetCurFontHeight( void );
short 	_GetCurFontLineHeight( void );
void	_SetFontColors( short color0, short color1, short color2, short color3 );
void	_GetFontColors( short *color0, short *color1, short *color2, short *color3 );
void	_SetupTextGDevice( segaCharRect *myRect, long patternStart );
void	_SetupTextSpriteGDevice( segaCharRect *myRect, long patternStart, short verticalTileSize );
Ptr		_GetTextPatternAddress ( short x, short y, short *scanlinesLeft );
void	_GetTextGDeviceOrigin( short *X, short *Y );
void	_DrawSegaChar( short x, short y, short charToDraw );
void	_DrawSegaString( short *x, short *y, char * myCString );
void	_SetSegaPixel( short x, short y, short value );
short	_MeasureSegaText( char * myCString );
void	_CenterSegaText( short y, char * myCString );
void	_DrawClippedSegaText( short x, short y, short pixelWidth, char * myCString );
void	_DrawCenteredClippedSegaText( short xCenter, short y, short pixelWidth, char * myCString );
short	_GetCharWidth( unsigned char theChar );
void	_SegaNumToString( long theNum, char *theString );
void	_SegaNumToDate( long theDate, char *theString, Boolean format );
void	_SegaAppendText( char *src, char *append, char* dst );
short	_CompareDates( long date1, long date2 );
short	_CompareStrings( char *string1, char *string2 );
void	_EraseTextGDevice( unsigned short bgColor );
unsigned short _GetStringLength( char *src );
void 	_CopyCString( char *src, char* dst );
Boolean _EqualCStrings( char *src, char* dst );
void	_SetTextPatternStart(Ptr start);
void *	_GetTextStateReference(void);
void	_SaveTextState( void *textGDeviceRef );
void	_RestoreTextState( void *textGDeviceRef );
void	_DisposeTextStateReference( void *textGDeviceRef );
void	_DrawPaddedClippedSegaText( short x, short y, short padWidth, short justify, char *string );

// private/internal, but vectorized

void	_RenderSegaString ( short *x, short *y, char * myCString, short justify, long textWidth, long renderWidth );
void	_VDPCopyBlitDirect( BitsPtr src, short x, short y );
void	_VDPCopyBlitDirectBGColor( BitsPtr src, short x, short y, short bgColor);
void	_VDPCopyBlitTiled( BitsPtr src, short x, short y);
void	_VDPCopyBlitTiledBGColor( BitsPtr src, short x, short y, short bgColor);
void	_OrBlit2to4( BitsPtr src, BitsPtr dst, short xPos );
void	_OrBlit1to4( BitsPtr src, BitsPtr dst, short xPos );



#ifndef SIMULATOR
TextGlobals text;
#endif	

long
_TextControl ( short command, long data )
{
long offset;
short error;
TextGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(text,offset);
			
			error = AllocateGlobalSpace ( kTextManager, offset,
				sizeof(TextGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create text globals" );
			if ( error != noErr )
				{
				return error;
				}
			
			globals->renderBuffer = 0L;
			globals->expansionTable1Bit = 0L;
			globals->expansionTable2Bit = 0L;
			globals->theFont = 0L;
			globals->curFontID = 0;

			
			/* install our selectors */
			SetDispatchedFunction ( kInitSegaFonts,			kTextManager,	_InitSegaFonts );
			SetDispatchedFunction ( kSetCurFont,			kTextManager,	_SetCurFont );
			SetDispatchedFunction ( kGetCurFont,			kTextManager,	_GetCurFont );
			SetDispatchedFunction ( kGetCurFontHeight,		kTextManager,	_GetCurFontHeight );
			SetDispatchedFunction ( kGetCurFontLineHeight,	kTextManager,	_GetCurFontLineHeight );
			SetDispatchedFunction ( kSetFontColors,			kTextManager,	_SetFontColors );
			SetDispatchedFunction ( kGetFontColors,			kTextManager,	_GetFontColors );
			SetDispatchedFunction ( kSetupTextGDevice,		kTextManager,	_SetupTextGDevice );
			SetDispatchedFunction ( kGetTextPatternAddress,	kTextManager,	_GetTextPatternAddress );
			SetDispatchedFunction ( kGetTextGDeviceOrigin,	kTextManager,	_GetTextGDeviceOrigin );
			SetDispatchedFunction ( kDrawSegaString,		kTextManager,	_DrawSegaString );
			SetDispatchedFunction ( kMeasureSegaText,		kTextManager,	_MeasureSegaText );
			SetDispatchedFunction ( kCenterSegaText,		kTextManager,	_CenterSegaText );
			SetDispatchedFunction ( kDrawClippedSegaText,	kTextManager,	_DrawClippedSegaText );
			SetDispatchedFunction ( kDrawCenteredClippedSegaText,	kTextManager,	_DrawCenteredClippedSegaText );
			SetDispatchedFunction ( kDrawPaddedClippedSegaText,	kTextManager,	_DrawPaddedClippedSegaText );
			SetDispatchedFunction ( kGetCharWidth,			kTextManager,	_GetCharWidth );
			SetDispatchedFunction ( kSegaNumToString,		kTextManager,	_SegaNumToString );
			SetDispatchedFunction ( kSegaNumToDate,			kTextManager,	_SegaNumToDate );
			SetDispatchedFunction ( kSegaAppendText,		kTextManager,	_SegaAppendText );
			SetDispatchedFunction ( kCompareDates,			kTextManager,	_CompareDates );
			SetDispatchedFunction ( kCompareStrings,		kTextManager,	_CompareStrings );
			SetDispatchedFunction ( kSetupTextSpriteGDevice,kTextManager,	_SetupTextSpriteGDevice );
			SetDispatchedFunction ( kDrawDBXYString,		kTextManager,	_DrawDBXYString );
			SetDispatchedFunction ( kGetDBXYString,			kTextManager,	_GetDBXYString );
			SetDispatchedFunction ( kGetSegaString,			kTextManager,	_GetSegaString );
			SetDispatchedFunction ( kGetWriteableString,	kTextManager,	_GetWriteableString );
			SetDispatchedFunction ( kSetWriteableString,	kTextManager,	_SetWriteableString );
			SetDispatchedFunction ( kDeleteWriteableString,	kTextManager,	_DeleteWriteableString );
			SetDispatchedFunction ( kGetUniqueWriteableStringID,	kTextManager,	_GetUniqueWriteableStringID );
			SetDispatchedFunction ( kEraseTextGDevice,		kTextManager,	_EraseTextGDevice );
			SetDispatchedFunction ( kGetStringLength,		kTextManager,	_GetStringLength );
			SetDispatchedFunction ( kCopyCString,			kTextManager,	_CopyCString );
			SetDispatchedFunction ( kSetTextPatternStart,	kTextManager,	_SetTextPatternStart );
			SetDispatchedFunction ( kEqualCStrings,			kTextManager,	_EqualCStrings );
			SetDispatchedFunction ( kGetTextStateReference,	kTextManager,	_GetTextStateReference );
			SetDispatchedFunction ( kSaveTextState,			kTextManager,	_SaveTextState );
			SetDispatchedFunction ( kRestoreTextState,		kTextManager,	_RestoreTextState );
			SetDispatchedFunction ( kDisposeTextStateReference,kTextManager,	_DisposeTextStateReference );

			SetDispatchedFunction ( kRenderSegaString,			kTextManager,	_RenderSegaString );
			SetDispatchedFunction ( kVDPCopyBlitDirect,			kTextManager,	_VDPCopyBlitDirect );
			SetDispatchedFunction ( kVDPCopyBlitDirectBGColor,	kTextManager,	_VDPCopyBlitDirectBGColor );
			SetDispatchedFunction ( kVDPCopyBlitTiled,			kTextManager,	_VDPCopyBlitTiled );
			SetDispatchedFunction ( kVDPCopyBlitTiledBGColor,	kTextManager,	_VDPCopyBlitTiledBGColor );
			SetDispatchedFunction ( kOrBlit2to4,				kTextManager,	_OrBlit2to4 );
			SetDispatchedFunction ( kOrBlit1to4,				kTextManager,	_OrBlit1to4 );
			break;
		
		case kSoftInialize:
			REFGLOBAL(text,renderBuffer) = NewMemory ( kTemp, kRenderBufferSize );
			REFGLOBAL(text, expansionTable1Bit) = NewMemory(kTemp, 16 * sizeof(short));
			REFGLOBAL(text, expansionTable2Bit) = NewMemory(kTemp, 256 * sizeof(short));
			REFGLOBAL(text,fontWorld).color0 = -1;		/* So SetCurFont will initialize expansion tables */
			REFGLOBAL(text,fontWorld).color1 = -1;		/* the first time!!! */
			REFGLOBAL(text,fontWorld).color2 = -1;
			REFGLOBAL(text,fontWorld).color3 = -1;
			ASSERT_MESG ( REFGLOBAL(text,renderBuffer), "Can't allocate text render buffer" );
			error = noErr;
			break;
		
		case kHardClose:
			error = noErr;
			break;
		
		case kSoftClose:
			error = noErr;
			break;
					
		case kCodeBlockMoved:
			error = noErr;
			break;
					
		case kGlobalsMoved:
			error = noErr;
			break;
		}
		
	return error;
}


#ifdef SIMULATOR
void WaitVBL( short count );

void WaitVBL( short count )
{
long	delayCount;

	Delay( count, &delayCount );
}
#endif

void _InitSegaFonts( void )
{
#ifdef SIMULATOR
SegaFontTablePtr theFont;
DBErr	theErr;
Handle	fontHandle;
//
// Allocate all the fonts and add them to the DB
//
	REFGLOBAL(text,theFont) = 0;

	fontHandle = GetResource( 'FNTC', kMainFont );
	if( fontHandle == 0 )
	{
		printf("Cacheing font: kMainFont\n" );
		theFont = Allocate2BitFont( kMainFont );
		theErr = DBAddItem( kFontType, kMainFont, (Ptr) theFont );
		ASSERT( !theErr );
		fontHandle = NewHandle( GetMemorySize( theFont ) );
		BlockMove( theFont, *fontHandle, GetMemorySize( theFont ) );
		AddResource( fontHandle, 'FNTC', kMainFont, "\pMain Font" );
		WriteResource( fontHandle );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );
		DisposeMemory ( theFont );
	}
	else
	{
		printf("Loading Cached font (FNTC): kMainFont\n" );
		theFont = NewMemory( kTemp, GetHandleSize( fontHandle ) );
		BlockMove( *fontHandle, theFont, GetHandleSize( fontHandle ) );
		theErr = DBAddItem( kFontType, kMainFont, (Ptr) theFont );
		ASSERT( !theErr );
		DisposeMemory ( theFont );	
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );	
	}

	fontHandle = GetResource( 'FNTC', kXBandBold10Font );
	if( fontHandle == 0 )
	{
		printf("Cacheing font: kXBandBold10Font\n" );
		theFont = Allocate2BitFont( kXBandBold10Font );
		theErr = DBAddItem( kFontType, kXBandBold10Font, (Ptr) theFont );
		ASSERT( !theErr );
		fontHandle = NewHandle( GetMemorySize( theFont ) );
		BlockMove( theFont, *fontHandle, GetMemorySize( theFont ) );
		AddResource( fontHandle, 'FNTC', kXBandBold10Font, "\pXBandBold 10 Font" );
		WriteResource( fontHandle );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );
		DisposeMemory ( theFont );
	}
	else
	{
		printf("Loading Cached font (FNTC):  kXBandBold10Font\n" );
		theFont = NewMemory( kTemp, GetHandleSize( fontHandle ) );
		BlockMove( *fontHandle, theFont, GetHandleSize( fontHandle ) );
		theErr = DBAddItem( kFontType, kXBandBold10Font, (Ptr) theFont );
		ASSERT( !theErr );
		DisposeMemory ( theFont );	
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );	
	}
	
	fontHandle = GetResource( 'FNTC', kXBandLight9Font );
	if( fontHandle == 0 )
	{
		printf("Cacheing font: kXBandLight9Font\n" );
		theFont = Allocate2BitFont( kXBandLight9Font );
		theErr = DBAddItem( kFontType, kXBandLight9Font, (Ptr) theFont );
		ASSERT( !theErr );
		fontHandle = NewHandle( GetMemorySize( theFont ) );
		BlockMove( theFont, *fontHandle, GetMemorySize( theFont ) );
		AddResource( fontHandle, 'FNTC', kXBandLight9Font, "\pXBandLight 9 Font" );
		WriteResource( fontHandle );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );
		DisposeMemory ( theFont );
	}
	else
	{
		printf("Loading Cached font (FNTC): kXBandLight9Font\n" );
		theFont = NewMemory( kTemp, GetHandleSize( fontHandle ) );
		BlockMove( *fontHandle, theFont, GetHandleSize( fontHandle ) );
		theErr = DBAddItem( kFontType, kXBandLight9Font, (Ptr) theFont );
		ASSERT( !theErr );
		DisposeMemory ( theFont );	
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );	
	}
	

	fontHandle = GetResource( 'FNTC', kXBandHeavy );
	if( fontHandle == 0 )
	{
		printf("Cacheing font: kXBandHeavy\n" );
		theFont = Allocate2BitFont( kXBandHeavy );
		theErr = DBAddItem( kFontType, kXBandHeavy, (Ptr) theFont );
		ASSERT( !theErr );
		fontHandle = NewHandle( GetMemorySize( theFont ) );
		BlockMove( theFont, *fontHandle, GetMemorySize( theFont ) );
		AddResource( fontHandle, 'FNTC', kXBandHeavy, "\pkXBandHeavy" );
		WriteResource( fontHandle );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );
		DisposeMemory ( theFont );
	}
	else
	{
		printf("Loading Cached font (FNTC): kXBandHeavy\n");
		theFont = NewMemory( kTemp, GetHandleSize( fontHandle ) );
		BlockMove( *fontHandle, theFont, GetHandleSize( fontHandle ) );
		theErr = DBAddItem( kFontType, kXBandHeavy, (Ptr) theFont );
		ASSERT( !theErr );
		DisposeMemory ( theFont );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );	
	}

	fontHandle = GetResource( 'FNTC', kHeadlineFont );
	if( fontHandle == 0 )
	{
		printf("Cacheing font: kHeadlineFont\n" );
		theFont = AllocateFont( kHeadlineFont );
		theErr = DBAddItem( kFontType, kHeadlineFont, (Ptr) theFont );
		ASSERT( !theErr );
		fontHandle = NewHandle( GetMemorySize( theFont ) );
		BlockMove( theFont, *fontHandle, GetMemorySize( theFont ) );
		AddResource( fontHandle, 'FNTC', kHeadlineFont, "\pkHeadlineFont" );
		WriteResource( fontHandle );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );
		DisposeMemory ( theFont );
	}
	else
	{
		printf("Loading Cached font (FNTC): kHeadlineFont\n");
		theFont = NewMemory( kTemp, GetHandleSize( fontHandle ) );
		BlockMove( *fontHandle, theFont, GetHandleSize( fontHandle ) );
		theErr = DBAddItem( kFontType, kHeadlineFont, (Ptr) theFont );
		ASSERT( !theErr );
		DisposeMemory ( theFont );
		ReleaseResource( fontHandle );
		DisposeHandle( fontHandle );	
	}

	UpdateResFile( CurResFile() );
	
#endif
}


FontID _SetCurFont( FontID newID )
{
FontID oldFontID;

	oldFontID = REFGLOBAL(text,curFontID);
	REFGLOBAL(text,curFontID) = newID;
//
// Gets the selected font out of the database and makes it the current font
// Returns the ID of the current font before the switch
//

	REFGLOBAL(text,theFont) = (SegaFontTablePtr) DBGetItem( kFontType, newID );
	ASSERT( REFGLOBAL(text,theFont) )
	
	ASSERT_MESG( REFGLOBAL(text,theFont)->height <= kMaxTextHeight, "Text blit buffer too small for font");
	return oldFontID;
}

FontID _GetCurFont( void )
{
	return REFGLOBAL(text,curFontID);
}

short _GetCurFontHeight( void )
{
	return REFGLOBAL(text,theFont)->height;
}

short _GetCurFontLineHeight( void )
{
	return REFGLOBAL(text,theFont)->yLineSpacing;
}

void _SetupTextGDevice( segaCharRect *myRect, long patternNum )
{
	ASSERT(patternNum);
//
// set to solid color to bring out uninitialized state
//
#ifdef ROMDEBUG
	REFGLOBAL(text,fontWorld).color0 = 8;
	REFGLOBAL(text,fontWorld).color1 = 8;
	REFGLOBAL(text,fontWorld).color2 = 8;
	REFGLOBAL(text,fontWorld).color3 = 8;
#endif

	REFGLOBAL(text,fontWorld).charStartX = myRect->left;
	REFGLOBAL(text,fontWorld).charStartY = myRect->top;
	REFGLOBAL(text,fontWorld).charScreenWidth = myRect->right - myRect->left;
	REFGLOBAL(text,fontWorld).charScreenHeight =  myRect->bottom - myRect->top;
	REFGLOBAL(text,fontWorld).visibleWidth = REFGLOBAL(text,fontWorld).charScreenWidth; //SEGAVisibleWidth;
	REFGLOBAL(text,fontWorld).verticalTileSize = REFGLOBAL(text,fontWorld).charScreenHeight;	//No tiling...all is linear mapped
	REFGLOBAL(text,fontWorld).screenBase = (Ptr)(PATOFFSET(patternNum) * 32);
}

void	_SetTextPatternStart(Ptr start)
{
	REFGLOBAL(text,fontWorld).screenBase = start;
}

void _EraseTextGDevice( unsigned short bgColor )
{
	register short patterns = REFGLOBAL(text,fontWorld).charScreenWidth * REFGLOBAL(text,fontWorld).charScreenHeight;
	register unsigned long fillData;
	
	bgColor = bgColor<<12 | bgColor<<8 | bgColor<<4 | bgColor;		// Replicate color into all 4 pixels in the word
	fillData = ((unsigned long)bgColor << 16) | (unsigned long)bgColor;
	
	patterns <<= 3;													// Turn pattern count into long count

	FillVRAM(fillData, REFGLOBAL(text,fontWorld).screenBase, patterns);
}

/*
 * Set up a text device to draw into tiled sprites. The sprites are tiled horizontally as such:
 *
 *			0	1	2	3	4
 *			5	6	7	8	9
 *
 * Within each sprite, the pattern memory is mapped as such (vertical tile size of 2, 3, and 4):
 *
 *			0	2			0	3	6		0	4	8	12		<-1
 *			1	3 			1	4	7		1	5	9	13		<-2 Vertical Tile
 *							2	5	8		2	6	10	14		<-3		Size
 *											3	7	11	15		<-4
 *
 *			[2x2]			  [3x3]				[4x4]
 *
 * Thus, when blitting, the pattern memory is mapped like stacked TextGDevices where each stack is of
 * verticalTileSize pattern cells in height. For verticalTileSize of 3 and segaCharRect width=12, height=6
 * pattern memory is laid out thus:
 *
 *		 ----- ----- ----- -----
 *		|0  2 |4  6 |8  10|12 14|			==== Sprite Text GDevice ====
 *		|1  3 |5  7 |9  11|13 15|		
 *		 ----- ----- ----- -----			2x2 Sprites, verticalTileSize=2
 *		|16 18|20 22|24 26|28 30|			charRect width=12, height=6
 *		|17 19|21 23|25 27|29 31|			
 *		 ----- ----- ----- -----			The sprites are allocated elsewhere,
 *		|32 34|36 38|40 42|44 46|			the caller simply tells us the vertical
 *		|33 35|37 39|41 43|45 47|			tile size.
 *		 ----- ----- ----- -----
 */
void _SetupTextSpriteGDevice( segaCharRect *myRect, long patternNum, short verticalTileSize )
{
//
// set to solid color to bring out uninitialized state
//
#ifdef ROMDEBUG
	REFGLOBAL(text,fontWorld).color0 = 8;
	REFGLOBAL(text,fontWorld).color1 = 8;
	REFGLOBAL(text,fontWorld).color2 = 8;
	REFGLOBAL(text,fontWorld).color3 = 8;
#endif

	REFGLOBAL(text,fontWorld).charStartX = myRect->left;
	REFGLOBAL(text,fontWorld).charStartY = myRect->top;
	REFGLOBAL(text,fontWorld).charScreenWidth = myRect->right - myRect->left;
	REFGLOBAL(text,fontWorld).charScreenHeight =  myRect->bottom - myRect->top;
	REFGLOBAL(text,fontWorld).visibleWidth = REFGLOBAL(text,fontWorld).charScreenWidth;	//SEGAVisibleWidth;
	REFGLOBAL(text,fontWorld).verticalTileSize = verticalTileSize;						//Set the sprite vertical tile size
	REFGLOBAL(text,fontWorld).screenBase = (Ptr)(PATOFFSET(patternNum) * 32);
}


Ptr _GetTextPatternAddress ( short x, short y, short *scansLeft )
{
unsigned long	vOffset;
unsigned long	hOffset;
short			yTileNum;

	yTileNum = (y>>3) / REFGLOBAL(text,fontWorld).verticalTileSize;		// Which row of sprites are we in?
	y -= (yTileNum << 3) * REFGLOBAL(text,fontWorld).verticalTileSize;	// Which scanline within that row?
	*scansLeft = (REFGLOBAL(text,fontWorld).verticalTileSize<<3) - y;
	vOffset =	((yTileNum *
				REFGLOBAL(text,fontWorld).verticalTileSize *
				REFGLOBAL(text,fontWorld).charScreenWidth) << 5) +
				4*y;
	hOffset = ((x&0x07)>>1) + 32*(long)REFGLOBAL(text,fontWorld).verticalTileSize*((x&0xff8)>>3);		//pos in cell + cell offset
	return( REFGLOBAL(text,fontWorld).screenBase + vOffset + hOffset );
}


void _GetTextGDeviceOrigin( short *X, short *Y )
{		
	*X = REFGLOBAL(text,fontWorld).charStartX;
	*Y = REFGLOBAL(text,fontWorld).charStartY;
}

void _SetFontColors( short color0, short color1, short color2, short color3 )
{
	/*
	 * Check to see if the colors haven't changed. If not, just return because building
	 * the expansion tables is pretty time consuming.
	 */
	if (REFGLOBAL(text,fontWorld).color0 == color0 &&
		REFGLOBAL(text,fontWorld).color1 == color1 &&
		REFGLOBAL(text,fontWorld).color2 == color2 &&
		REFGLOBAL(text,fontWorld).color3 == color3)
			return;
			
	REFGLOBAL(text,fontWorld).color0 = color0;
	REFGLOBAL(text,fontWorld).color1 = color1;
	REFGLOBAL(text,fontWorld).color2 = color2;
	REFGLOBAL(text,fontWorld).color3 = color3;
	
	if (REFGLOBAL(text, expansionTable1Bit) == 0)	// If we're just initing fonts, don't build
		return;
		
	/* Now build the text expansion tables */
	{
		short index;
		unsigned short colors[4];
		unsigned short *table2to4 = REFGLOBAL(text, expansionTable2Bit);
		unsigned short *table1to4 = REFGLOBAL(text, expansionTable1Bit);
		
		colors[0] = 0;		// Background is always transparent in this table
		colors[1] = color1;
		colors[2] = color2;
		colors[3] = color3;
		for (index=0; index<256; ++index) {
			table2to4[index] = 	(colors[(index&0xc0)>>6] << 12) |
								(colors[(index&0x30)>>4] << 8) |
								(colors[(index&0x0c)>>2] << 4) |
								colors[(index&0x03)];
		}

		colors[0] = 0;		// Background is always transparent in this table
		colors[1] = color0;
		for (index=0; index<16; ++index) {
			table1to4[index] = 	(colors[(index&8)>>3] << 12) |
								(colors[(index&4)>>2] << 8) |
								(colors[(index&2)>>1] << 4) |
								colors[(index&1)];
		}

	}
}

void _GetFontColors( short *color0, short *color1, short *color2, short *color3 )
{
	if( color0 )
		*color0 = REFGLOBAL(text,fontWorld).color0;

	if( color1 )
		*color1 = REFGLOBAL(text,fontWorld).color1;

	if( color2 )
		*color2 = REFGLOBAL(text,fontWorld).color2;

	if( color3 )
		*color3 = REFGLOBAL(text,fontWorld).color3;
}

#ifdef oldText
void VDPCopyBlit1To4( BitsPtr src, short x, short y, short foreColor, short backColor )
{
short			scanLine;
short			srcPos;
long			dstLineStart;
Ptr				dstPtr;
Ptr				srcPtr;
Ptr				srcLineStart;
short			srcPix;
unsigned char	srcByte;
unsigned char	table1To4[4];
short			myX;
short			dstWord;
short			pixCount;
short			vCellBump;

	table1To4[0] = (backColor<<4) | backColor;
	table1To4[1] = (backColor<<4) | foreColor;
	table1To4[2] = (foreColor<<4) | backColor;
	table1To4[3] = (foreColor<<4) | foreColor;

	dstLineStart = (long) GetTextPatternAddress( x, y );
	
	srcLineStart = src->address;
	
	OwnVDP();
	
	SetVDPIncrement( 2 );	//increment 1 word at a time
	vCellBump = 32 * (REFGLOBAL(text,fontWorld).charScreenHeight+1) - 34;
	
	for( scanLine = 0; scanLine < src->yBounds; scanLine++ )
	{
		srcPtr = srcLineStart;
		myX = x;
		dstWord = 0;
		pixCount = 4;
		dstPtr = (Ptr) dstLineStart;
		
		// init our dst address
		SetVDPVRAMAddress( (unsigned long) dstPtr );
		
		srcPix = 0;		// always grab byte first time
		
		for( srcPos = ( src->xBounds >> 1 ); srcPos > 0 ; --srcPos )
			{
			//get next src
			if( srcPix == 0 )
				{
				srcByte = *srcPtr++;
				srcPix = 6;		//set to shift top of byte down
				}
			else
				{
				srcPix -= 2;
				}

			dstWord <<= 8;
			dstWord |= table1To4[ srcByte >> 6 ];
			srcByte <<= 2;
			pixCount -= 2;
			
			if ( pixCount == 0 )
				{
				/* write out another word */
				WriteWordToVDP( dstWord );
				
				dstWord = 0;
				pixCount = 4;
				//bump dst
				myX += 4;					//skip to next four pixels in this cell
				if( (myX & 7) == 0 )		//did we wrap?
					{
					dstPtr += vCellBump;			//skip to next cell in dst
					SetVDPVRAMAddress( (unsigned long) dstPtr );
					}
				else
					{
					dstPtr += 2;
					}
				}
			}
		
		/* make sure we write out that last word */
		if ( pixCount == 2 )
			{
			dstWord <<= 8;
			WriteWordToVDP( dstWord );
			}
	
	//bump src row
		srcLineStart += src->rowBytes;	//skip to next source line

	//bump dst row
		y++;
		dstLineStart += 4;				//skip the next dst line in this cell or the next
										//cell. This works since we are vertically mapped
	}
	
	FreeVDP();
}

//
// Simple 1to1 OR blit loop...does no clipping, just blasts away a byte at a time!
//
void OrBlit1to1( BitsPtr src, BitsPtr dst, short xPos )
{
Ptr				srcPtr;
Ptr				dstPtr;
short			scanLine;
unsigned short	sourceData;
Ptr				dstLineStart;
Ptr				srcLineStart;
char			srcShift;
short			bytePos;

	srcLineStart = src->address;
	dstLineStart = dst->address + ((xPos & 0xff8)>>3);
	srcShift = 8 - (xPos & 0x7);				// amount to shift source left by to get to dst
		
	for( scanLine = 0; scanLine < src->yBounds; scanLine++ )
	{
		srcPtr = srcLineStart;
		dstPtr = dstLineStart;
		for( bytePos = 0; bytePos < src->rowBytes; bytePos++ )
		{
			sourceData = *(unsigned char *)srcPtr++ << srcShift;
			*(unsigned char *)(dstPtr++) |= (sourceData>>8);
			*(unsigned char *)dstPtr |= sourceData;
		}
	
	//bump src row
		srcLineStart += src->rowBytes;	//skip to next source line

	//bump dst row
		dstLineStart += dst->rowBytes;	//skip to next dst line
	}
}

//
// 2 to 4 blit loop
//
void VDPCopyBlit2To4( BitsPtr src, short x, short y, short color0, short color1, short color2, short color3 )
{
short			scanLine;
short			srcPos;
long			dstLineStart;
Ptr				dstPtr;
Ptr				srcPtr;
Ptr				srcLineStart;
short			srcPix;
unsigned char	srcByte;
unsigned char	table2To4[16];
short			myX;
short			dstWord;
short			pixCount;
short			vCellBump;

	table2To4[0] = (color0<<4) | color0;
	table2To4[1] = (color0<<4) | color1;
	table2To4[2] = (color0<<4) | color2;
	table2To4[3] = (color0<<4) | color3;
	table2To4[4] = (color1<<4) | color0;
	table2To4[5] = (color1<<4) | color1;
	table2To4[6] = (color1<<4) | color2;
	table2To4[7] = (color1<<4) | color3;
	table2To4[8] = (color2<<4) | color0;
	table2To4[9] = (color2<<4) | color1;
	table2To4[10] = (color2<<4) | color2;
	table2To4[11] = (color2<<4) | color3;
	table2To4[12] = (color3<<4) | color0;
	table2To4[13] = (color3<<4) | color1;
	table2To4[14] = (color3<<4) | color2;
	table2To4[15] = (color3<<4) | color3;

	dstLineStart = (long) GetTextPatternAddress( x, y );
	
	srcLineStart = src->address;
	
	OwnVDP();
	
	SetVDPIncrement( 2 );	//increment 1 word at a time
	vCellBump = 32 * (REFGLOBAL(text,fontWorld).charScreenHeight+1) - 34;
	
	for( scanLine = 0; scanLine < src->yBounds; scanLine++ )
	{
		srcPtr = srcLineStart;
		myX = x;
		dstWord = 0;
		pixCount = 4;					//dest pixels in a short
		dstPtr = (Ptr) dstLineStart;
		
		// init our dst address
		SetVDPVRAMAddress( (unsigned long) dstPtr );
		
		srcPix = 0;		// always grab byte first time
		
		for( srcPos = ( src->xBounds >> 1 ); srcPos > 0 ; --srcPos )
			{
			//get next src
			if( srcPix == 0 )
				{
				srcByte = *srcPtr++;
				srcPix = 2;		//set to shift top of byte down
				}
			else
				{
				srcPix -= 2;
				}

			dstWord <<= 8;
			dstWord |= table2To4[ srcByte >> 4 ];
			srcByte <<= 4;
			pixCount -= 2;					//two dest pixels at a time
			
			if ( pixCount == 0 )
				{
				/* write out another word */
				WriteWordToVDP( dstWord );
				
				dstWord = 0;
				pixCount = 4;				//dest pixels in a short
			
			//bump dst
				myX += 4;					//skip to next four pixels in this cell
				if( (myX & 7) == 0 )		//did we wrap?
					{
					dstPtr += vCellBump;			//skip to next cell in dst
					SetVDPVRAMAddress( (unsigned long) dstPtr );
					}
				else
					{
					dstPtr += 2;
					}
				}
			}
		
		/* make sure we write out that last word */
		if ( pixCount == 2 )
			{
			dstWord <<= 8;
			WriteWordToVDP( dstWord );
			}
	
	//bump src row
		srcLineStart += src->rowBytes;	//skip to next source line

	//bump dst row
		y++;
		dstLineStart += 4;				//skip the next dst line in this cell or the next
										//cell. This works since we are vertically mapped
	}
	
	FreeVDP();
}
#endif

//
// 4 to 4 blit loop
//
void _VDPCopyBlitDirect( BitsPtr src, short x, short y)
{
register long			dstLineStart;
register Ptr			dstPtr;
register unsigned short	*srcPtr;
register Ptr			srcLineStart;
register short			scanlines;
register short			horizontalCells;
register long			hDstBump;
short					unused;

	dstLineStart = (long) GetTextPatternAddress( x, y, &unused );
	srcLineStart = src->address;
	horizontalCells = (src->rowBytes+3) >> 2;					// Number of cell columns to copy
	
	OwnVDP();
	
	SetVDPIncrement( 2 );										//increment 1 word at a time
	
	hDstBump = (long) GetTextPatternAddress( x + 8, y, &unused ) - dstLineStart;
#ifdef SIMULATOR		
	while (horizontalCells--) {
		SetVDPVRAMAddress(dstLineStart);
		srcPtr = (unsigned short *)srcLineStart;
		
		ASSERT( ((long)srcPtr & 1) == 0 );						// No odd-aligned short writes
		for (scanlines=src->yBounds; scanlines>0; --scanlines) {
			WriteWordToVDP(*srcPtr++);
			WriteWordToVDP(*srcPtr++);
		}
		
		dstLineStart += hDstBump;
		srcLineStart += src->yBounds*4;
	}
#else
	asm
		{
		move.l		src,a0
		move.w		OFFSET(Bits,yBounds)(a0),d1
		lea			0xc00000,a0
		bra.s		@hRow
		
@hNext	move.l		dstLineStart,d0
		lsl.l		#2,d0
		lsr.w		#2,d0
		or.w		#0x4000,d0
		swap.w		d0
		move.l		d0,4(a0)
		move.l		srcLineStart,srcPtr
		
		move.w		d1,d2
		bra.s		@vRow

@vNext
		move.l		(srcPtr)+,(a0)
@vRow	dbra		d2,@vNext

		add.l		hDstBump,dstLineStart
		move.w		d1,d0
		lsl.w		#2,d0
		add.w		d0,srcLineStart
		
@hRow	dbra		horizontalCells,@hNext
		}
#endif
	
	FreeVDP();
}

//
// 4 to 4 blit loop, but we must convert transparent pixels to the bgColor
//
void _VDPCopyBlitDirectBGColor( BitsPtr src, short x, short y, short bgColor)
{
register long			dstLineStart;
register Ptr			dstPtr;
register unsigned short	*srcPtr;
register Ptr			srcLineStart;
register short			scanlines;
register short			horizontalCells;
register long			hDstBump;
short					unused;

	bgColor = bgColor<<12 | bgColor<<8 | bgColor<<4 | bgColor;
	dstLineStart = (long) GetTextPatternAddress( x, y, &unused );
	srcLineStart = src->address;
	horizontalCells = (src->rowBytes+3) >> 2;					// Number of cell columns to copy
	
	OwnVDP();
	
	SetVDPIncrement( 2 );										//increment 1 word at a time
	
	hDstBump = (long) GetTextPatternAddress( x + 8, y, &unused ) - dstLineStart;

		while (horizontalCells--) {
			SetVDPVRAMAddress(dstLineStart);
			srcPtr = (unsigned short *)srcLineStart;
			
			ASSERT( ((long)srcPtr & 1) == 0 );						// No odd-aligned short writes
		for (scanlines=src->yBounds; scanlines>0; --scanlines) {
			short data;
			
			data = *srcPtr++;
			if ((data & 0xF000) == 0)
				data |= bgColor & 0xF000;
			if ((data & 0xF00) == 0)
				data |= bgColor & 0xF00;
			if ((data & 0xF0) == 0)
				data |= bgColor & 0xF0;
			if ((data & 0xF) == 0)
				data |= bgColor & 0xF;
			WriteWordToVDP(data);
			
			data = *srcPtr++;
			if ((data & 0xF000) == 0)
				data |= bgColor & 0xF000;
			if ((data & 0xF00) == 0)
				data |= bgColor & 0xF00;
			if ((data & 0xF0) == 0)
				data |= bgColor & 0xF0;
			if ((data & 0xF) == 0)
				data |= bgColor & 0xF;
			WriteWordToVDP(data);
			}
			
			dstLineStart += hDstBump;
			srcLineStart += src->yBounds*4;
		}
		
	FreeVDP();
}

//
// 4 to 4 blit loop, but pattern memory is linearized to draw into sprites
//
void _VDPCopyBlitTiled( BitsPtr src, short x, short y)
{
register long			dstLineStart;
register Ptr			dstPtr;
register unsigned short	*srcPtr;
register Ptr			srcLineStart;
register short			scanlineIndex;
register short			horizontalCells;
register long			hDstBump;
short					scanlinesLeftInTile;
short					scanlines;
short					scanlineOffset = 0;

	scanlines = src->yBounds;		// Scanlines to draw
	dstLineStart = (long) GetTextPatternAddress( x, y, &scanlinesLeftInTile );
	hDstBump = (long) GetTextPatternAddress( x + 8, y, &scanlinesLeftInTile ) - dstLineStart;
	srcLineStart = src->address;
	
	OwnVDP();
	
	SetVDPIncrement( 2 );										//increment 1 word at a time

	while (scanlines) {
		if (scanlinesLeftInTile > scanlines)					// Never draw more than "scanlines" scanlines
			scanlinesLeftInTile = scanlines;
		horizontalCells = (src->rowBytes+3) >> 2;				// Number of cell columns to copy
		
		while (horizontalCells--) {
			SetVDPVRAMAddress(dstLineStart);
			srcPtr = (unsigned short *)srcLineStart;
			
			ASSERT( ((long)srcPtr & 1) == 0 );						// No odd-aligned short writes
			for (scanlineIndex=scanlinesLeftInTile; scanlineIndex>0; --scanlineIndex) {
				WriteWordToVDP(*srcPtr++);
				WriteWordToVDP(*srcPtr++);
			}
			
			dstLineStart += hDstBump;
			srcLineStart += src->yBounds*4;
		}
		scanlines -= scanlinesLeftInTile;
		y += scanlinesLeftInTile;
		scanlineOffset += scanlinesLeftInTile;
		srcLineStart = src->address + scanlineOffset*4;
		dstLineStart = (long) GetTextPatternAddress( x, y, &scanlinesLeftInTile );
	}
	
	FreeVDP();
}

//
// 4 to 4 blit loop, but pattern memory is linearized to draw into sprites, and handles
// background colors
//
void _VDPCopyBlitTiledBGColor( BitsPtr src, short x, short y, short bgColor)
{
register long			dstLineStart;
register Ptr			dstPtr;
register unsigned short	*srcPtr;
register Ptr			srcLineStart;
register short			scanlineIndex;
register short			horizontalCells;
register long			hDstBump;
short					scanlinesLeftInTile;
short					scanlines;
short					scanlineOffset = 0;

	bgColor = bgColor<<12 | bgColor<<8 | bgColor<<4 | bgColor;
	scanlines = src->yBounds;		// Scanlines to draw
	dstLineStart = (long) GetTextPatternAddress( x, y, &scanlinesLeftInTile );
	hDstBump = (long) GetTextPatternAddress( x + 8, y, &scanlinesLeftInTile ) - dstLineStart;
	srcLineStart = src->address;
	
	OwnVDP();
	
	SetVDPIncrement( 2 );										//increment 1 word at a time

	while (scanlines) {
		if (scanlinesLeftInTile > scanlines)					// Never draw more than "scanlines" scanlines
			scanlinesLeftInTile = scanlines;
		horizontalCells = (src->rowBytes+3) >> 2;				// Number of cell columns to copy
		
		while (horizontalCells--) {
			SetVDPVRAMAddress(dstLineStart);
			srcPtr = (unsigned short *)srcLineStart;
			
			ASSERT( ((long)srcPtr & 1) == 0 );						// No odd-aligned short writes
			for (scanlineIndex=scanlinesLeftInTile; scanlineIndex>0; --scanlineIndex) {
				short data;
				
				data = *srcPtr++;
				if ((data & 0xF000) == 0)
					data |= bgColor & 0xF000;
				if ((data & 0xF00) == 0)
					data |= bgColor & 0xF00;
				if ((data & 0xF0) == 0)
					data |= bgColor & 0xF0;
				if ((data & 0xF) == 0)
					data |= bgColor & 0xF;
				WriteWordToVDP(data);
				
				data = *srcPtr++;
				if ((data & 0xF000) == 0)
					data |= bgColor & 0xF000;
				if ((data & 0xF00) == 0)
					data |= bgColor & 0xF00;
				if ((data & 0xF0) == 0)
					data |= bgColor & 0xF0;
				if ((data & 0xF) == 0)
					data |= bgColor & 0xF;
				WriteWordToVDP(data);
			}
			
			dstLineStart += hDstBump;
			srcLineStart += src->yBounds*4;
		}
		scanlines -= scanlinesLeftInTile;
		y += scanlinesLeftInTile;
		scanlineOffset += scanlinesLeftInTile;
		srcLineStart = src->address + scanlineOffset*4;
		dstLineStart = (long) GetTextPatternAddress( x, y, &scanlinesLeftInTile );
	}
	
	FreeVDP();
}

#ifdef oldText
//
// Simple 2to2 OR blit loop...does no clipping, just blasts away a byte at a time!
//
void OrBlit2to2( BitsPtr src, BitsPtr dst, short xPos )
{
Ptr				srcPtr;
Ptr				dstPtr;
short			scanLine;
unsigned short	sourceData;
Ptr				dstLineStart;
Ptr				srcLineStart;
char			srcShift;
short			bytePos;

	srcLineStart = src->address;
	dstLineStart = dst->address + ((xPos & 0xffc)>>2);
	srcShift = 2*(4 - (xPos & 0x3));				// amount to shift source left by to get to dst
		
	for( scanLine = 0; scanLine < src->yBounds; scanLine++ )
	{
		srcPtr = srcLineStart;
		dstPtr = dstLineStart;
		for( bytePos = 0; bytePos < src->rowBytes; bytePos++ )
		{
			sourceData = *(unsigned char *)srcPtr++ << srcShift;
			*(unsigned char *)(dstPtr++) |= (sourceData>>8);
			*(unsigned char *)dstPtr |= sourceData;
		}
	
	//bump src row
		srcLineStart += src->rowBytes;	//skip to next source line

	//bump dst row
		dstLineStart += dst->rowBytes;	//skip to next dst line
	}
}
#endif

//
// 2->4 OR blit loop...does no clipping. Remaps from mac-style scanline data to SEGA character data
//
void _OrBlit2to4( BitsPtr src, BitsPtr dst, short xPos )
{
	Ptr						srcPtr;
	Ptr						dstPtr;
	register Ptr			dstLineStart;
	register Ptr			srcLineStart;
	register short			cellBytesLeft;
	register unsigned long	sourceData;
	register char			dstShift;
	register short			bytePos;
	short					scanLine;
	register unsigned short	*table2to4 = REFGLOBAL(text, expansionTable2Bit);
	short					cellBytes;
	short					destXBytes;
	short					charByteWidth = (src->xBounds + 3) >> 2;	// 2-bit byte width
		
	/* Set up source addresses */
	srcLineStart = src->address;
	
	/* Set up dest addresses */
	destXBytes = ((xPos & 0xffe)>>1);					// Byte offset into dst
	cellBytes = dst->yBounds*kCellRowBytes;				// Bump to next cell's data
	dstLineStart =	dst->address +						// Address
					((destXBytes>>2) * cellBytes) +		// + offset to the correct cell
					(destXBytes&3);						// + offset within that cell
	cellBytes -= 4;
	
	/* Set up shifts */
	dstShift = (xPos&1) ? 4 : 8;						// amount to shift expanded src left

	for( scanLine = 0; scanLine < src->yBounds; scanLine++ )
	{
		cellBytesLeft = kCellRowBytes - (destXBytes&3);		// bytes left until we must bump to next horizontal cell
		srcPtr = srcLineStart;
		dstPtr = dstLineStart;
		
		for( bytePos = 0; bytePos < charByteWidth; bytePos++ )
		{
			/* Get data and expand it into sourceData */
			sourceData = (unsigned long)table2to4[*(unsigned char *)srcPtr++] << dstShift;
			
			if (sourceData == 0) {
				dstPtr += 2;
				cellBytesLeft -= 2;
				if (cellBytesLeft <= 0) {
					dstPtr += cellBytes;
					cellBytesLeft = kCellRowBytes+cellBytesLeft;
				}
				continue;
			}
			
			/* Write first byte of expanded data */
			*(unsigned char *)(dstPtr) |= (sourceData>>16);
			++dstPtr;
			if (--cellBytesLeft == 0) {
				cellBytesLeft = kCellRowBytes;
				dstPtr += cellBytes;
			}
			
			/* Write second byte of expanded data */
			*(unsigned char *)dstPtr |= (sourceData>>8);
			++dstPtr;
			if (--cellBytesLeft == 0) {
				cellBytesLeft = kCellRowBytes;
				dstPtr += cellBytes;
			}
			
			/* Do partial byte if necessary */
			if (dstShift==4) {
				*(unsigned char *)dstPtr |= sourceData;
			}
		}
	
	//bump src row
		srcLineStart += src->rowBytes;	//skip to next source line

	//bump dst row
		dstLineStart += kCellRowBytes;	//skip to next dst line
	}
}


//
// 1->4 OR blit loop...does no clipping. Remaps from mac-style scanline data to SEGA character data
//
void _OrBlit1to4( BitsPtr src, BitsPtr dst, short xPos )
{
	Ptr				srcPtr;
	Ptr				dstPtr;
	short			scanLine;
	unsigned long	sourceData;
	Ptr				dstLineStart;
	Ptr				srcLineStart;
	char			dstShift;
	short			bytePos;
	short			cellBytes;
	short			destXBytes;
	unsigned short	*table1to4 = REFGLOBAL(text, expansionTable1Bit);
	short			cellBytesLeft;
	short			charByteWidth = (src->xBounds + 7) >> 3;	// 1-bit byte width
		
	/* Set up source addresses */
	srcLineStart = src->address;
	
	/* Set up dest addresses */
	destXBytes = ((xPos & 0xffe)>>1);					// Byte offset into dst
	cellBytes = dst->yBounds*kCellRowBytes;				// Bump to next cell's data
	dstLineStart =	dst->address +						// Address
					((destXBytes>>2) * cellBytes) +		// + offset to the correct cell
					(destXBytes&3);						// + offset within that cell
	cellBytes -= 4;
	
	/* Set up shifts */
	dstShift = (xPos&1) ? 4 : 8;						// amount to shift expanded src left

	for( scanLine = 0; scanLine < src->yBounds; scanLine++ )
	{
		cellBytesLeft = kCellRowBytes - (destXBytes&3);		// bytes left until we must bump to next horizontal cell
		srcPtr = srcLineStart;
		dstPtr = dstLineStart;
		
		for( bytePos = 0; bytePos < charByteWidth; bytePos++ )
		{
			/* Get data and expand it into sourceData */
			sourceData = (unsigned long)table1to4[(*(unsigned char *)srcPtr) >> 4] << dstShift;
			
			if (sourceData == 0) {
				dstPtr += 2;
				cellBytesLeft -= 2;
				if (cellBytesLeft <= 0) {
					dstPtr += cellBytes;
					cellBytesLeft = kCellRowBytes+cellBytesLeft;
				}
			} else {
				/* Write first byte of expanded data */
				*(unsigned char *)(dstPtr) |= (sourceData>>16);
				++dstPtr;
				if (--cellBytesLeft == 0) {
					cellBytesLeft = kCellRowBytes;
					dstPtr += cellBytes;
				}
				
				/* Write second byte of expanded data */
				*(unsigned char *)dstPtr |= (sourceData>>8);
				++dstPtr;
				if (--cellBytesLeft == 0) {
					cellBytesLeft = kCellRowBytes;
					dstPtr += cellBytes;
				}
				
				/* Do partial byte if necessary */
				if (dstShift==4) {
					*(unsigned char *)dstPtr |= sourceData;
				}
			}

			/* Get data and expand it into sourceData */
			sourceData = (unsigned long)table1to4[(*(unsigned char *)srcPtr++) & 0xF] << dstShift;
			
			if (sourceData == 0) {
				dstPtr += 2;
				cellBytesLeft -= 2;
				if (cellBytesLeft <= 0) {
					dstPtr += cellBytes;
					cellBytesLeft = kCellRowBytes+cellBytesLeft;
				}
			} else {
			
				/* Write first byte of expanded data */
				*(unsigned char *)(dstPtr) |= (sourceData>>16);
				++dstPtr;
				if (--cellBytesLeft == 0) {
					cellBytesLeft = kCellRowBytes;
					dstPtr += cellBytes;
				}
				
				/* Write second byte of expanded data */
				*(unsigned char *)dstPtr |= (sourceData>>8);
				++dstPtr;
				if (--cellBytesLeft == 0) {
					cellBytesLeft = kCellRowBytes;
					dstPtr += cellBytes;
				}
				
				/* Do partial byte if necessary */
				if (dstShift==4) {
					*(unsigned char *)dstPtr |= sourceData;
				}
			}
		}
	
	//bump src row
		srcLineStart += src->rowBytes;	//skip to next source line

	//bump dst row
		dstLineStart += kCellRowBytes;	//skip to next dst line
	}
}


short _GetCharWidth( unsigned char theChar )
{
short	remappedChar;

	remappedChar = REFGLOBAL(text,theFont)->remappingTable[theChar];
	return REFGLOBAL(text,theFont)->widthTable[remappedChar] + 1;
}

void _DrawSegaString( short *x, short *y, char * myCString )
{
short	imageWidth;

	imageWidth = MeasureSegaText( myCString );
	RenderSegaString ( x, y, myCString, kJustifyLeft, imageWidth, imageWidth );
}


void _RenderSegaString ( short *x, short *y, char * myCString, short justify, long textWidth, long renderWidth )
{
short iii;
unsigned char 	*myStringPtr;
short	curXPos;
short	theChar;
Ptr		srcAddr;
Bits	source;
Bits	compositeBits;
short	localX;
char	*myBuffer;
unsigned short	bufferSkip;

	myBuffer = REFGLOBAL(text,renderBuffer);
	
	if ( !myBuffer )
		return;
		
	ASSERT_MESG ( renderWidth >= textWidth, "Render width is less than text with" );

	/* figure out how much of the buffer to skip on the left */
	/* we don't have to do anything for left justification */
	/* we assume that render width is always greater than textWidth */
	switch ( justify )
		{
		case kJustifyLeft:
			bufferSkip = 0;
			break;
			
		case kJustifyCenter:
			bufferSkip = ( renderWidth - textWidth ) >> 1;
			break;
		
		case kJustifyRight:
			bufferSkip = renderWidth - textWidth;
			break;
		
		default:
			ERROR_MESG ( "Bad justify constant" );
			break;
		}
//
// Copy string into 2-bit buffer
//

// Setup compositeBits -- allocate buffer (pixel width + 7)/8 * height
	{
	long *bufferClear;
	
		/* If this ASSERT fires, we need to rev this loop! */
		ASSERT_MESG( (kRenderBufferSize & 3) == 0, "kRenderBufferSize is not mult of 4!");

		bufferClear = (long *) myBuffer;
		for (iii=kRenderBufferSize / 4; iii > 0; iii-- )
			*bufferClear++ = 0;
	}

	/* add the text alignment shift plus byte padding to textwidth as we align the buffer to it's eventual
	 * byte alignment and always render whole bytes. use this new value if it's bigger than render width */
	textWidth += (*x & 7);
	if ( textWidth > renderWidth )
		{
		renderWidth = textWidth;
		}
	
	compositeBits.xBounds = renderWidth;
	
#ifdef oldText
	if( REFGLOBAL(text,theFont)->bitDepth == 1 )
		compositeBits.rowBytes = ((compositeBits.xBounds + 7)/8);
	else
		compositeBits.rowBytes = ((compositeBits.xBounds + 3)/4);
#else
	compositeBits.rowBytes = ((compositeBits.xBounds + 3)/2) & ~1;	/* 4-bit short-aligned buffer */
#endif
	compositeBits.address = (Ptr) myBuffer;
	compositeBits.yBounds = (REFGLOBAL(text,theFont)->height+1) & ~1;

// or the fucker into the 1-bit buffer starting at the left edge
	curXPos = ( *x & 7 ) + bufferSkip;			// Draw into the compositeBits aligned to eventual pattern mems alignment
	myStringPtr = (unsigned char*)myCString;
	while( *myStringPtr )
	{
	// Remap the character
		theChar = REFGLOBAL(text,theFont)->remappingTable[*myStringPtr];
	
	// Get address of character to blit
		source.address = (Ptr)&REFGLOBAL(text,theFont)->fontTable[0];
		source.address += (theChar * REFGLOBAL(text,theFont)->offset);
	
	// Get address of character to blit
		source.rowBytes = REFGLOBAL(text,theFont)->rowBytes;
		source.xBounds = GetCharWidth( *myStringPtr );
		source.yBounds = REFGLOBAL(text,theFont)->height;
#ifdef oldText	
		if( REFGLOBAL(text,theFont)->bitDepth == 1 )
			OrBlit1to1( &source, &compositeBits, curXPos );
		else
			OrBlit2to2( &source, &compositeBits, curXPos );
#else
		if( REFGLOBAL(text,theFont)->bitDepth == 1 )
			OrBlit1to4( &source, &compositeBits, curXPos );
		else
			OrBlit2to4( &source, &compositeBits, curXPos );
#endif		

		curXPo