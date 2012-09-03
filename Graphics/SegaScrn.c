/*

	File:		SegaScrn.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<34>	 8/28/94	JBH		Initialize the sega GDevices on soft init, to decrease the
									likelihood of a memory pisser making our graphics look like
									shit!
		<33>	 8/28/94	KON		Setup the clut as the last thing to do in InitVDP to try to
									eliminate booting into a red screen or yellow screen.
		<32>	 8/26/94	CMY		Don't enable the VDP until we've cleared the world out.
		<31>	 8/20/94	JBH		Added IncrementClutReferences.
		<30>	 8/10/94	JBH		Make RequestUniqueClut return 0 and post a message upon failure.
		<29>	  8/9/94	CMY		Added FillNameTable
		<28>	 7/31/94	KON		Make all colors in the clut fade, and make them fade all the way
									to black.
		<27>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		<26>	 7/26/94	CMY		Updated to use VBL flag in SetupClut.
		<25>	 7/23/94	JBH		Really made the background color black (in the globals, as well
									as the hardware)
		<24>	 7/21/94	CMY		Make the background color black.
		<23>	 7/17/94	ADS		Adding comment while not vectoring anything
		<22>	 7/15/94	JBH		Added kGetClut, kGetColorLuminance.
		<21>	 7/15/94	JBH		Added RequestUniqueClut, RequestSpecificClut, SetupClut
									(public), added VBL sync to clut changers.
		<20>	 7/13/94	ADS		ASSERT a problem case
		<19>	  7/7/94	HEC		Added kNonSpecificCode to DelayMS call
		<18>	  7/6/94	BET		Add caller parameter to SegaOSIdle.
		<17>	  7/3/94	SAH		Learned Josh how to spell.
		<16>	  7/3/94	JBH		Added [Get][Set]BackgroundColor.
		<15>	 6/29/94	CMY		Added GenerateGrayMap -- builds a luminance mapping table
									between cluts.
		<14>	 6/28/94	CMY		Palette manager.
		<13>	 6/21/94	HEC		Added FadeInScreen.
		<12>	 6/16/94	BET		Revert Ted's changes.
		<11>	 6/15/94	SAH		Added direction and delay parameters to FadeInClut.
		<10>	 6/15/94	HEC		SegaOSIdle takes a param now
		 <9>	 6/11/94	SAH		Added WaitVBlank.
		 <8>	  6/5/94	SAH		Black the cluts in SetupVDP while clearing VRAM to clean up
									screen transitions some.
		 <7>	  6/5/94	SAH		Init VScroll Ram.
		 <6>	  6/5/94	KON		Disable shadow/highright.
		 <5>	  6/5/94	CMY		Add clut fading calls.
		 <4>	 5/31/94	SAH		Brought in SetupVDp from SegaMisc.
		 <3>	 5/30/94	SAH		Added SetupClutDB.
		 <2>	 5/26/94	SAH		Made a manager.
		 <8>	 5/23/94	SAH		Added EraseGDevice.
		 <7>	 5/20/94	SAH		Made LinearizeScreenArea take a palette parameter and made it
									look at the left location of the rectangle and do the right
									thing by it.
		 <6>	 5/18/94	HEC		Converting over to Pattern Manager
		 <5>	 5/18/94	SAH		Added GetSegaGDevice.
		 <4>	 5/17/94	KON		Add absolute start character position to gDevice.
		 <3>	 5/16/94	SAH		Added kSprite to GetSegaScreenBaseAddr.
		 <2>	 5/11/94	KON		Map character cells vertically instead of horizontally.

	To Do:
*/

#include "Sega.h"
#include "SegaScrn.h"
#include "SegaVDP.h"
#include "VDP.h"
#include "Errors.h"
#include "Pattern.h"
#include "SegaScrnPriv.h"
#include "DBTypes.h"
#include "SegaCluts.h"
#include "SegaTraps.h"
#include "Time.h"

/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
ScreenGlobals screen;
#endif


long	_LinearizeScreenArea( segaCharRect *theRect, short palette );
Ptr		_GetSegaScreenBaseAddr( short theScreen );
void	_InitSegaGDevices( void );
void	_SetCurrentDevice( short theScreen );
short	_GetCurrentDevice( void );
short	_RequestClut( short * clut );
short	_RequestUniqueClut( short *clut);
short	_RequestSpecificClut( short *clut, short clutNumber);
void	_ReleaseClut( short clutReference );
void	_IncrementClutReferences( short clutReference );
void	_SetupClutDB( unsigned char clutNumber, short clutID );
void	_GetSegaScreenOrigin( short theScreen, short *X, short *Y );
SegaGDevice *	_GetSegaGDevice ( short device );
void	_EraseGDevice ( short device );
void	_SetupVDP( void );
void 	_BlankClut( unsigned char clutNumber );
void 	_FadeInClut( unsigned char clutNumber, DBID clutID, short delay, Boolean fadeIn );
void 	_FadeInScreen( short delay, Boolean fadeIn );
void	_WaitVBlank( void );
void	_GenerateGrayMap( short *clut, unsigned char *grayMap );
short	_GetColorLuminance( short color);
void	_SetBackgroundColor(unsigned char color);
unsigned char	_GetBackgroundColor( void );
void	_SetupClut( unsigned char clutNumber, short *values, Boolean syncToVBL );
void	_GetClut( unsigned char clutNumber, short *vales );
void	_FillNameTable( short device, segaCharRect *theRect, short pattern );


long
_ScreenControl ( short command, long data )
{
long offset;
short error;
ScreenGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
		
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(screen,offset);
			
			error = AllocateGlobalSpace ( kScreenManager, offset, sizeof(ScreenGlobals),
				(Ptr *) &globals );
			if ( error != noErr )
				{
				return error;
				}
			
			globals->gCurDevice = 0;
			globals->screenIsBlack = 0;
			
			/* install our selectors */
			SetDispatchedFunction ( kLinearizeScreenArea,	kScreenManager,	_LinearizeScreenArea );
			SetDispatchedFunction ( kGetSegaScreenBaseAddr,	kScreenManager,	_GetSegaScreenBaseAddr );
			SetDispatchedFunction ( kInitSegaGDevices,		kScreenManager,	_InitSegaGDevices );
			SetDispatchedFunction ( kSetCurrentDevice,		kScreenManager,	_SetCurrentDevice );
			SetDispatchedFunction ( kGetCurrentDevice,		kScreenManager,	_GetCurrentDevice );
			SetDispatchedFunction ( kRequestClut,			kScreenManager,	_RequestClut );
			SetDispatchedFunction ( kReleaseClut,			kScreenManager,	_ReleaseClut );
			SetDispatchedFunction ( kIncrementClutReferences,kScreenManager,_IncrementClutReferences );
			SetDispatchedFunction ( kSetupClutDB,			kScreenManager,	_SetupClutDB );
			SetDispatchedFunction ( kGetSegaScreenOrigin,	kScreenManager,	_GetSegaScreenOrigin );
			SetDispatchedFunction ( kGetSegaGDevice,		kScreenManager,	_GetSegaGDevice );
			SetDispatchedFunction ( kEraseGDevice,			kScreenManager,	_EraseGDevice );
			SetDispatchedFunction ( kSetupVDP,				kScreenManager,	_SetupVDP );
			SetDispatchedFunction ( kBlankClut,				kScreenManager,	_BlankClut );
			SetDispatchedFunction ( kFadeInClut,			kScreenManager,	_FadeInClut );
			SetDispatchedFunction ( kFadeInScreen,			kScreenManager,	_FadeInScreen );
			SetDispatchedFunction ( kWaitVBlank,			kScreenManager,	_WaitVBlank );
			SetDispatchedFunction ( kGenerateGrayMap,		kScreenManager,	_GenerateGrayMap );
			SetDispatchedFunction ( kSetBackgroundColor,	kScreenManager,	_SetBackgroundColor );
			SetDispatchedFunction ( kGetBackgroundColor,	kScreenManager,	_GetBackgroundColor );
			SetDispatchedFunction ( kRequestUniqueClut,		kScreenManager,	_RequestUniqueClut );
			SetDispatchedFunction ( kRequestSpecificClut,	kScreenManager,	_RequestSpecificClut );
			SetDispatchedFunction ( kSetupClut,				kScreenManager,	_SetupClut );
			SetDispatchedFunction ( kGetClut,				kScreenManager,	_GetClut );
			SetDispatchedFunction ( kGetColorLuminance,		kScreenManager,	_GetColorLuminance );
			SetDispatchedFunction ( kFillNameTable,			kScreenManager,	_FillNameTable );
			break;
		
		case kSoftInialize:
		{
			unsigned char *luminance;
			
			REFGLOBAL(screen,screenIsBlack) = 0;
			REFGLOBAL(screen,gCurDevice) = 0;
			REFGLOBAL(screen,clutSemaphore)[0] = 0;
			REFGLOBAL(screen,clutSemaphore)[1] = 0;
			REFGLOBAL(screen,clutSemaphore)[2] = 0;
			
			/* Build the luminance tables */
			luminance = REFGLOBAL(screen,redLuminance);
			*luminance++ = 0;
			*luminance++ = 11;
			*luminance++ = 22;
			*luminance++ = 32;
			*luminance++ = 43;
			*luminance++ = 54;
			*luminance++ = 65;
			*luminance++ = 76;
			luminance = REFGLOBAL(screen,greenLuminance);
			*luminance++ = 0;
			*luminance++ = 22;
			*luminance++ = 43;
			*luminance++ = 65;
			*luminance++ = 86;
			*luminance++ = 108;
			*luminance++ = 130;
			*luminance++ = 151;
			luminance = REFGLOBAL(screen,blueLuminance);
			*luminance++ = 0;
			*luminance++ = 4;
			*luminance++ = 8;
			*luminance++ = 12;
			*luminance++ = 16;
			*luminance++ = 20;
			*luminance++ = 24;
			*luminance++ = 28;
			
			InitSegaGDevices();
			
			error = noErr;
			break;
		}
		
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

//
// Init Sega GDevices
//
void _InitSegaGDevices( void )
{
short			iii;
SegaGDevice *	deviceList;
	
	deviceList = REFGLOBAL(screen,gDevices);
	for( iii = kNumGDevices - 1; iii >= 0 ; --iii )
	{
		deviceList->charStartX = 0;
		deviceList->charStartY = 0;
		deviceList->charScreenWidth = SEGARealWidth;
		deviceList->charScreenHeight = SEGAHeight;
		deviceList->visibleWidth = SEGAVisibleWidth;
		deviceList->screenBase = (Ptr) 0x20;
		deviceList++;
	}
	REFGLOBAL(screen,gCurDevice) = kScrollA;
}

void _SetCurrentDevice( short theScreen )
{
	if( (theScreen < kScrollA) || (theScreen > kWindow) )
	{
		ERROR_MESG( "Invalid screen in SetCurrentDevice" );
	}
	REFGLOBAL(screen,gCurDevice) = theScreen;
}

short _GetCurrentDevice( void )
{
	return REFGLOBAL(screen,gCurDevice);
}


void _FillNameTable( short device, segaCharRect *theRect, short pattern )
{
	unsigned long	screenBase = (unsigned long) GetSegaScreenBaseAddr( device );
	unsigned long	rowBytes;
	short			vCells;
	short			xxx,yyy;
	
	OwnVDP();
	
	SetVDPIncrement( 2 );

	rowBytes = 2 * REFGLOBAL(screen,gDevices)[ device ].charScreenWidth;
	screenBase += rowBytes * theRect->top;
	screenBase += theRect->left * 2;
	
	vCells = theRect->bottom - theRect->top;
	for( yyy = theRect->top; yyy < theRect->bottom; yyy++ )
	{
		SetVDPVRAMAddress( screenBase );
		screenBase += rowBytes;
		for( xxx = theRect->left; xxx < theRect->right; xxx++ )
		{
			WriteWordToVDP( pattern );
		}
	}
	FreeVDP();
}

//
// Linearize the current active screen
//
// Map the screen vertically as in:
//
// 1 4 7
// 2 5 8
// 3 6 9
//
long _LinearizeScreenArea( segaCharRect *theRect, short palette )
{
short	numPats;
short	patWalker;
long	pat;
short	vCells;
short	xxx,yyy;
unsigned long screenBase;
unsigned long rowBytes;
unsigned short	cellFlags;

	numPats = (theRect->right - theRect->left)*(theRect->bottom - theRect->top);
	pat = NewPatternBlock( numPats );
	if (!pat)
		WARNING_MESG("Linearize failing");
	if ( pat ) {
		patWalker = PATOFFSET(pat);
		screenBase = (unsigned long) GetSegaScreenBaseAddr( REFGLOBAL(screen,gCurDevice) );
		
		OwnVDP();
		
		SetVDPIncrement( 2 );
	
	// Now linearize 'em
	
		rowBytes = 2 * REFGLOBAL(screen,gDevices)[ REFGLOBAL(screen,gCurDevice) ].charScreenWidth;
		screenBase += rowBytes * theRect->top;
		screenBase += theRect->left * 2;
		
		cellFlags = palette << 13;
		
		vCells = theRect->bottom - theRect->top;
		for( yyy = theRect->top; yyy < theRect->bottom; yyy++ )
		{
			SetVDPVRAMAddress( screenBase );
			screenBase += rowBytes;
			patWalker = PATOFFSET(pat) + (yyy-theRect->top);
			for( xxx = theRect->left; xxx < theRect->right; xxx++ )
			{
				WriteWordToVDP( patWalker | cellFlags );
				patWalker += vCells;
			}
		}
		FreeVDP();
	}
	return pat;
}

Ptr _GetSegaScreenBaseAddr( short theScreen )
{
	switch( theScreen )
	{
		case kScrollA:
			return( (Ptr)kScrollABaseAddr );
			break;
		
		case kScrollB:
			return( (Ptr)kScrollBBaseAddr );
			break;
		
		case kWindow:
			DebugStr( "\pThe window is not setup yet!!!" );
			return( (Ptr)kWindowBaseAddr );
			break;
		
		case kSprite:
			return( (Ptr)kSpriteBaseAddr );
			break;
	}

}

void _SetupClut( unsigned char clutNumber, short * colors, Boolean syncToVBL )
{
short	i;

	ASSERT_MESG(clutNumber >= 0 && clutNumber <= 3, "clut number out of range");

	if ( syncToVBL )
	{
		WaitVBlank();
	}

	OwnVDP();
	if (!REFGLOBAL(screen,screenIsBlack))
	{
		SetVDPIncrement( 2 );			//increment 2 bytes at a time
		SetVDPCRAMAddress( 16*2*clutNumber );
	}
	for( i = 0; i < 16; i++ )
	{
		REFGLOBAL(screen,clutStorage[clutNumber][i]) = *colors;
		if (!REFGLOBAL(screen,screenIsBlack))
			WriteWordToVDP( *colors );
		colors++;
	}
	FreeVDP();
}

void _GetClut( unsigned char clutNumber, short *colors)
{
	short i;
	
	ASSERT_MESG(clutNumber >= 0 && clutNumber <= 3, "clut number out of range");
	
	for (i=0; i<16; i++) {
		*colors++ = REFGLOBAL(screen, clutStorage[clutNumber][i]);
	}
}

void _SetupClutDB( unsigned char clutNumber, short clutID )
{
short	iii;
short *	colors;

	colors = (short *) DBGetItem ( kClutType, clutID );
	ASSERT_MESG ( colors, "Couldn't load clut from database" );
	if ( colors )
	{
		SetupClut( clutNumber, colors, false );
	}
}


void _BlankClut( unsigned char clutNumber )
{
short	iii;

	WaitVBlank();

	OwnVDP();
	SetVDPIncrement( 2 );			//increment 2 bytes at a time
	SetVDPCRAMAddress( 16*2*clutNumber );
	
	for( iii = 15; iii >= 0; --iii )
	{
		WriteWordToVDP( 0 );
		REFGLOBAL(screen,clutStorage)[clutNumber][iii] = 0;
	}
	FreeVDP();
}

/* Find a clut that matches the one being requested */
/* This function will skip an animating (unique) clut */

// As long as _RequestClut is the only caller (and he calls it right away), we'll skip
//  vectorizing this one....

static short FindMatchingClut( short *clut )
{
	short clutIndex;
	short entryIndex;
	
	/* Iterate through all 4 cluts */
	for (clutIndex = 0; clutIndex <= 3; ++clutIndex)
	{
		/* Search the default clut and any in-use (non-unique) cluts for a match */
		if ( (clutIndex == 0) || (REFGLOBAL(screen,clutSemaphore)[clutIndex-1] > 0) )
		{
			/* Iterate through colors 1..15 (0 is always transparent) */
			for (entryIndex = 1; entryIndex <= 16; ++entryIndex)
			{
				/* If the first 15 colors matched, we found a matching clut */
				if (entryIndex == 16)
					return clutIndex;
					
				if (REFGLOBAL(screen,clutStorage)[clutIndex][entryIndex] != clut[entryIndex] )
					break;
			}
		}
	}
	
	return -1;	/* Didn't find one */
}

/* Someone needs a custom clut. Find a free clut or one that matches it */
short _RequestClut( short * clut )
{
	short clutReference = FindMatchingClut(clut);
	
	/* If there's a clut already in-use that matches, bump its owners and return it */
	if (clutReference >= 0)
	{
		/* Default clut is read-only so we don't worry about its references */
		if (clutReference)
			REFGLOBAL(screen,clutSemaphore)[clutReference-1]++;
			
		return clutReference;
	}
	
	/* Didn't find a match. Look for an unused clut and appropriate it */
	for (clutReference = 1; clutReference <= 3; ++clutReference)
	{
		if (REFGLOBAL(screen,clutSemaphore)[clutReference-1] == 0)
		{
			/* Found an unused palette. Bump its owners and set it up */
			
			REFGLOBAL(screen,clutSemaphore)[clutReference-1]++;
			SetupClut(clutReference, clut, false);
			
			return clutReference;
		}
	}
	
	return 0;	/* No match and none free. Give 'em the default palette */
}

/* RequestUniqueClut creates a clut that nobody else will use. It is useful for */
/* color table animations. Release using ReleaseClut. */
short _RequestUniqueClut( short *clut)
{
	short clutReference = 0;
	
	for (clutReference = 1; clutReference <= 3; ++clutReference)
	{
		if (REFGLOBAL(screen,clutSemaphore)[clutReference-1] == 0)
		{
			/* found an unused palette. Set its owners, set it up */
			REFGLOBAL(screen,clutSemaphore)[clutReference-1] = -99;		/* magic unique/specific value */
			SetupClut(clutReference, clut, false);
			
			return clutReference;
		}
	}
	
	MESG("RequestUniqueClut failed.");
	return 0;				
}

/* If a client wants a specific clut number, it should call this. This is useful for */
/* HiWrite (sic) mode, which uses clut 3 on the Sega. */
short _RequestSpecificClut( short *clut, short clutNumber )
{
	if (REFGLOBAL(screen,clutSemaphore)[clutNumber-1] == 0)
	{
		REFGLOBAL(screen,clutSemaphore)[clutNumber-1] = -99;
		SetupClut( clutNumber, clut, false);
		
		return clutNumber;
	}
	else
		WARNING_MESG("warning - clut 3 in use, so RequestSpecificClut failed");
		return 0;
}

/* User is done with the custom clut */
void _ReleaseClut( short clutReference )
{
	ASSERT_MESG(clutReference >= 0 && clutReference <= 3, "ReleaseClut: Bad CLUT reference");
	
	/* The default palette is read-only, so we don't semaphore it */
	if (clutReference == 0)
		return;

	/* If it was a "unique" clut, just release it and bail. */
	if (REFGLOBAL(screen,clutSemaphore)[clutReference-1] == -99)
	{
		REFGLOBAL(screen,clutSemaphore)[clutReference-1] = 0;
		return;
	}
	
	/* Decrement the reference count to this clut */	
	REFGLOBAL(screen,clutSemaphore)[clutReference-1]--;
	
	/* The reference count should never be negative */
	if (REFGLOBAL(screen,clutSemaphore)[clutReference-1] < 0)
	{
		WARNING_MESG("Bad clut reference count");
		REFGLOBAL(screen,clutSemaphore)[clutReference-1] = 0;	/* Just fix it for the ROM */
	}
}

void _IncrementClutReferences( short clutReference )
{
	ASSERT_MESG(clutReference >= 0 && clutReference <= 3, "IncrementClutReferences: Bad CLUT reference");

	/* The default palette is read-only, so we don't semaphore it */
	if ( clutReference == 0 )
		return;
		
	/* If it was a "unique" clut, just warn and bail. */
	if (REFGLOBAL(screen,clutSemaphore)[clutReference-1] == -99)
	{
		WARNING_MESG("Incrementing owners of unique clut!");
		return;
	}
	
	ASSERT_MESG( REFGLOBAL(screen,clutSemaphore)[clutReference-1], "Incrementing owners of unallocated clut!");
	
	/* Increment the reference count to this clut */	
	REFGLOBAL(screen,clutSemaphore)[clutReference-1]++;
	
}

/* Return a 16 byte table that maps from a pixel of data for the given clut to a pixel of data
 * in the default clut that is grayscale
 */
void _GenerateGrayMap( short *clut, unsigned char *grayMap )
{
	short index;
	
	grayMap[0] = 0;				// transparent always maps to transparent
	
	for (index=1; index<16; ++index)
	{
		short luminance = GetColorLuminance( clut[index] );
		
		/* Luminance is 0..255. The default palette has 7 grays. We divide luminance
		 * by 32 to give us a range of 0..7 to index into the 7 available grays (we map
		 * both 0 and 1 to black). Since the grays ramp from 15..12 in order of increasing
		 * lightness, we then subtract this index from 15. This is incredibly palette
		 * specific, of course!
		 */
		grayMap[index] = 7 - (luminance>>5);
		if (grayMap[index] == 0)
			grayMap[index] = 1;
	}
}

short _GetColorLuminance( short color )
{
	short luminance;
	
	luminance = REFGLOBAL(screen,redLuminance)[(color&0xE)>>1] +
				REFGLOBAL(screen,greenLuminance)[(color&0xE0)>>5] +
				REFGLOBAL(screen,blueLuminance)[(color&0xE00)>>9];
				
	/* luminance is now 0..255. It is up to the caller to do something intelligent with it */
	return luminance;
}

void _FadeInScreen( short delay, Boolean fadeIn )
{
short	fadeIndex;
short 	index;
unsigned short color;
unsigned short fadeColor;
short	*colors;
short	level;
short	dir;
short	clut;

	REFGLOBAL(screen,screenIsBlack) = !fadeIn;
	
	/* which way are we going? */
	if ( fadeIn )
	{
		level = 0;
		dir = 1;
	}
	else
	{
		level = 7;
		dir = -1;
	}

	WaitVBlank();
	
	OwnVDP();
	
	for( fadeIndex = 0; fadeIndex <= 7; ++fadeIndex ) 
	{
		for( clut = 0; clut < 4; clut++ ) 
		{
			SetVDPIncrement( 2 );					//increment 2 bytes at a time
			SetVDPCRAMAddress( 16*2*clut );
			colors = &REFGLOBAL(screen,clutStorage[clut][0]);
			for( index = 0; index < 16; ++index )
			{
				color = colors[index];
				fadeColor = 0;
				 
				fadeColor = (level*((color&0xe00)>>9)/7) << 9;
				fadeColor |= (level*((color&0xe0)>>5)/7) << 5;
				fadeColor |= (level*((color&0xe)>>1)/7) << 1;
				WriteWordToVDP( fadeColor );
			}
		}
		level += dir;
		DelayMS( delay, kNonSpecificCode );
	}
	
	FreeVDP();
}

void _FadeInClut( unsigned char clutNumber, DBID clutID, short delay, Boolean fadeIn )
{
	short	fadeIndex;
	short 	index;
	unsigned short color;
	unsigned short fadeColor;
	short	*colors;
	short	level;
	short	dir;
	
	colors = (short *) DBGetItem ( kClutType, clutID );
	ASSERT_MESG ( colors, "Couldn't load clut from database" );
	
	/* which way are we going? */
	if ( fadeIn )
	{
		level = 0;
		dir = 1;
	}
	else
	{
		level = 7;
		dir = -1;
	}
	
	WaitVBlank();

	OwnVDP();
	
	for (fadeIndex = 0; fadeIndex <= 7; ++fadeIndex) {
		SetVDPIncrement( 2 );					//increment 2 bytes at a time
		SetVDPCRAMAddress( 16*2*clutNumber );
		
		for( index = 0; index < 16; ++index )
		{
			color = colors[index];
			fadeColor = 0;
			
			fadeColor = (level*((color&0xe00)>>9)/7) << 9;
			fadeColor |= (level*((color&0xe0)>>5)/7) << 5;
			fadeColor |= (level*((color&0xe)>>1)/7) << 1;
			WriteWordToVDP( fadeColor );
		}
		
		level += dir;
		DelayMS( delay, kNetworkCode );
	}
	
	FreeVDP();
}

void _GetSegaScreenOrigin( short theScreen, short *X, short *Y )
{

	if( (theScreen < kScrollA) || (theScreen > kWindow) )
	{
		ERROR_MESG( "Invalid screen in GetSegaScreenOrigin" );
	}
		
	*X = REFGLOBAL(screen,gDevices)[theScreen].charStartX;
	*Y = REFGLOBAL(screen,gDevices)[theScreen].charStartY;
}


SegaGDevice * _GetSegaGDevice ( short device )
{
	return &REFGLOBAL(screen,gDevices)[device];
}


void _EraseGDevice ( short deviceID )
{
SegaGDevice * device;
Ptr base;
long lCount;

	base = GetSegaScreenBaseAddr ( deviceID );
	device = GetSegaGDevice ( deviceID );
	
	lCount = device->charScreenWidth * device->charScreenHeight;
	lCount >>= 1;
	FillVRAM ( 0, base, lCount );
}


void _SetupVDP( void )
{
	OwnVDP();
	
	WriteVDPRegister( 0, 0x04 );	//disable counter, hinterrupt
	WriteVDPRegister( 1, 0x04 );	//disable Display, disable DMA, disable V interrupt
	WriteVDPRegister( 2, kScrollABaseAddr >> 10 );	//Set pattern name table for ScrollA to $E000
#ifdef	SIMULATOR
	/* BRAIN DAMAGE: Be sure to set the one for the sega below */
	WriteVDPRegister( 12, 0x81);	//Set no interlace, 40 cell horizontal, disable shadow/highright
#endif
	WriteVDPRegister( 3, kWindowBaseAddr >> 10 );	//Set pattern name table for window to $C000
	WriteVDPRegister( 4, kScrollBBaseAddr >> 13 );	//Set pattern name table for ScrollB to $C000
	WriteVDPRegister( 5, kSpriteBaseAddr >> 9 );	//set sprite attribute table base address to $A000

#ifndef SIMULATOR

	WriteVDPRegister( 6, 0x00 );	//nop
	
#endif

	WriteVDPRegister( 7, 0x07 );	//background color

	REFGLOBAL(screen, backgroundColor) = 7;

#ifndef SIMULATOR

	WriteVDPRegister( 8, 0x00 );	//nop
	WriteVDPRegister( 9, 0x00 );	//nop

#endif

	WriteVDPRegister( 10, 0xff );	//H-interrupt
	WriteVDPRegister( 11, 0x00 );	//H-scroll, full scroll, no interrupts									//auto increment
#ifndef	SIMULATOR
	/* BRAIN DAMAGE: Be sure to set the one for the simulator above */
	WriteVDPRegister( 12, 0x81);	//Set no interlace, 40 cell horizontal, disable shadow/highright - +8 to enable shadow/highright
#endif
	WriteVDPRegister( 13, 0x3C );	//HScroll to $F000

#ifndef SIMULATOR

	WriteVDPRegister( 14, 0x00 );	//nop

#endif
									//auto increment
	WriteVDPRegister( 15, 0x01 );	//bytes
	WriteVDPRegister( 16, 0x01 );	//HScroll size to 32 by 64
	WriteVDPRegister( 17, 0x00 );	//window h position
	WriteVDPRegister( 18, 0x00 );	//window v position
#ifdef	SIMULATOR
	WriteVDPRegister( 19, 0xff );	//DMA length low
	WriteVDPRegister( 20, 0xff );	//DMA length high
	WriteVDPRegister( 21, 0x00 );	//DMA source low
	WriteVDPRegister( 22, 0x00 );	//DMA source mid
	WriteVDPRegister( 23, 0x80 );	//DMA source high
#endif
	
	FreeVDP();
	
	/* fade the cluts to black so that anything in there doesn't look too bad.. */
	/* these do their own OwnVDP()s */
	BlankClut(0);
	BlankClut(1);
	BlankClut(3);
	
//
// Clear all of VRAM
//
	OwnVDP();
	
	SetVDPIncrement( 2 );			//increment 2 bytes at a time
	SetVDPVRAMAddress( 0x0000 );
	{
	short	iii;
	
		
		for( iii = 0; iii < 0xFFFF/2; iii++ )
		{
			WriteWordToVDP( 0 );
		}
	}
	
	/* clear the beginning of hscroll */
	SetVDPVRAMAddress( 0xF000 );
	WriteWordToVDP( 0 );
	
	/* clear all of vscroll */
	FillVSRAM ( 0L, 0L, 1 );
//
// Set the colors
//

	SetupClutDB( 0, kZeusClut );

	WriteVDPRegister( 1, 0x64 );	//enable Display, disable DMA, enable V interrupt
	
	FreeVDP();
}

/* spin-wait until a VBL fires - this is currently used by CLUT updates only. Note that calling SegaOSIdle */
/* could kill these. */
void	_WaitVBlank( void )
{
short	vdpStatus;

	do {
#ifdef SIMULATOR
		vdpStatus = 0x80;			/* don't wait - we're on a stimulator */
#else
		vdpStatus = * (unsigned short *) 0xC00004;
#endif
	} while (! (vdpStatus & 0x80) );
}

void _SetBackgroundColor(unsigned char color)
{
	REFGLOBAL(screen, backgroundColor) = color;
	OwnVDP();
	WriteVDPRegister( 7, color );	//background color
	FreeVDP();
}

unsigned char _GetBackgroundColor( void )
{
	return (REFGLOBAL(screen, backgroundColor));
}