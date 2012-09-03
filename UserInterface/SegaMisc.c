/*
	File:		SegaMisc.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 7/29/94	JOE		Added OwnVDP()/FreeVDP() macro brackets to VDP accesses
		 <7>	 5/30/94	SAH		Moved OSIdle stuff to time mgr.
		 <6>	 5/29/94	KON		Moved the graphics database type stuff into it's own file.
		 <5>	 5/28/94	KON		Added SegaNumToDate.
		 <4>	 5/28/94	SAH		Ok, I'm an idiot. Fixed my last chnage for the simulator.
		 <3>	 5/28/94	SAH		Took out the last change as I changed the macros. Moved the VDP
									register setup some so the sega likes it better.
		 <2>	 5/27/94	SAH		Work around WriteWordToVDP(0) gnerating a clr.w bug.
		 <7>	 5/24/94	SAH		Call the database for the non-simulator version of
									GetSegaGraphics.
		 <6>	 5/18/94	HEC		long pattern types
		 <5>	 5/17/94	KON		Include Threads.h...ok, so eyeballing a compare is not a good
									idea.
		 <4>	 5/17/94	KON		Make OSIdle call YieldToAnyThread.
		 <3>	 5/16/94	SAH		Set the sprite table to the correct address.
		 <2>	 5/12/94	SAH		Made SegaOSIdle call TimeIdle until we make it a list or
									something real.

	To Do:
*/

#include "sega.h"
#include "SegaVDP.h"
#include "vdp.h"
#include "SegaScrn.h"
#include "SegaBM.h"
#include "SegaMisc.h"
#include "SegaOS.h"
#include "Time.h"
#include "Database.h"
#include "DBTypes.h"
#include "StringDB.h"
#include "SegaCluts.h"
#include "SegaTraps.h"

//
// Globals
//
SegaOSIdlePtr gOSIdle;

//
// Turns the number into a C-string
//
void SegaNumToString( long theNum, char *theString )
{
short	iii;
long	curPlace;
short	value;

	if( theNum < 0 )
	{
		*theString++ = '-';
		theNum = -theNum;
	}
		
	curPlace = 1000000;
//
// Strip off leading zeros
//
	while( curPlace > theNum )
		curPlace /= 10;
		
	while( curPlace )
	{
		value = -1;
		while( theNum >= 0 )
		{
			theNum -= curPlace;
			value++;
		}
		theNum += curPlace;
		*theString++ = value + '0';
		curPlace /= 10;
	}	
	*theString++ = 0;
}

//
// Turns the number into a date
//
// Special dates have the real date encoded in them so they can be compared to 
// other dates, but only the bits left after kSpecialIndexMask is used for the
// lookup in the stringDB.
//
void SegaNumToDate( long theDate, char *theString )
{
short	iii;
long	curPlace;
short	value;
char	*srcPtr;
char	scratchString[10];

//
// Check for a special date
//
	if( theDate < 0 )
	{
		srcPtr = GetSegaString( kSpecialDateStart + (theDate & kSpecialDateIndexMask) );	
		while( *srcPtr )
			*theString++ = *srcPtr++;
		
		*theString = 0;
		return;
	}
//
// Date format is Month date, year
// get the month and copy it to theString
//
	srcPtr = GetSegaString( kJanuaryString + ( (theDate & kMonthMask) >> kMonthShift ) );
	while( *srcPtr )
		*theString++ = *srcPtr++;

//
// Get day and copy it to the string
//
	SegaNumToString( ( (theDate & kDayMask) >> kDayShift ), scratchString );
	srcPtr = scratchString;
	while( *srcPtr )
		*theString++ = *srcPtr++;
		
	*theString++ = ',';
	*theString++ = ' ';

//
// Get year and copy it to the string
//
	SegaNumToString( ( (theDate & kYearMask) >> kYearShift ), scratchString );
	srcPtr = scratchString;
	while( *srcPtr )
		*theString++ = *srcPtr++;

	*theString = 0;			//end the mayhem!!!
}

short CompareDates( long date1, long date2 )
{
	if( (kSpecialDateEscapeMask & date1) > (kSpecialDateEscapeMask & date2) )
		return 1;
	else
	if( (kSpecialDateEscapeMask & date1) == (kSpecialDateEscapeMask & date2) )
		return 0;
	else
		return -1;
}


void SetupVDP( void )
{
	OwnVDP();
	
	WriteVDPRegister( 0, 0x04 );	//disable counter, hinterrupt
	WriteVDPRegister( 1, 0x04 );	//disable Display, disable DMA, disable V interrupt
	WriteVDPRegister( 2, kScrollABaseAddr >> 10 );	//Set pattern name table for ScrollA to $E000
#ifdef	SIMULATOR
	/* BRAIN DAMAGE: Be sure to set the one for the sega below */
	WriteVDPRegister( 12, 0x81 + 0x08);	//Set no interlace, 40 cell horizontal, enable shadow/highright
#endif
	WriteVDPRegister( 3, kWindowBaseAddr >> 10 );	//Set pattern name table for window to $C000
	WriteVDPRegister( 4, kScrollBBaseAddr >> 13 );	//Set pattern name table for ScrollB to $C000
	WriteVDPRegister( 5, kSpriteBaseAddr >> 9 );	//set sprite attribute table base address to $A000

#ifndef SIMULATOR

	WriteVDPRegister( 6, 0x00 );	//nop
	
#endif

	WriteVDPRegister( 7, 0x00 );	//background color

#ifndef SIMULATOR

	WriteVDPRegister( 8, 0x00 );	//nop
	WriteVDPRegister( 9, 0x00 );	//nop

#endif

	WriteVDPRegister( 10, 0xff );	//H-interrupt
	WriteVDPRegister( 11, 0x00 );	//H-scroll, full scroll, no interrupts									//auto increment
#ifndef	SIMULATOR
	/* BRAIN DAMAGE: Be sure to set the one for the simulator above */
	WriteVDPRegister( 12, 0x81 + 0x08);	//Set no interlace, 40 cell horizontal, enable shadow/highright
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
	WriteVDPRegister( 1, 0x64 );	//enable Display, disable DMA, enable V interrupt
		
//
// Clear all of VRAM
//
	SetVDPIncrement( 2 );			//increment 2 bytes at a time
	SetVDPVRAMAddress( 0x0000 );
	{
	short	iii;
	
		
		for( iii = 0; iii < 0xFFFF/2; iii++ )
		{
			WriteWordToVDP( 0 );
		}
	}
	SetVDPVRAMAddress( 0xF000 );
	WriteWordToVDP( 0 );
	
	FreeVDP();

//
// Set the colors
//

	SetupClutDB( 0, kZeusClut );
	SetupClutDB( 1, kDefaultClut1 );
	SetupClutDB( 3, kDefaultClut3 );
}





/* *** BEWARE! ****
   this is only called by SetupSprite(), below.  It expects the VDP address to have already
   been set, and OwnVDP() to have already been done.
 */
 
void ExpandDataToVDP( unsigned char *myBitData, short numCells )
{
short	iii;
unsigned short expandTable[16] = {0x0000, 0x000f, 0x00f0, 0x00ff, 0x0f00, 0x0f0f, 0x0ff0, 0x0fff,
								  0xf000, 0xf00f, 0xf0f0, 0xf0ff, 0xff00, 0xff0f, 0xfff0, 0xffff };
//unsigned short expandTable[16] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
//								  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff };
								
	for( iii = 0; iii < numCells; iii++ )
	{
		WriteWordToVDP( expandTable[myBitData[iii]>>4] );
		WriteWordToVDP( expandTable[myBitData[iii] & 0x0F] );
	}
}


void SetupSprite( void )
{
unsigned char	myBitData[8*24] = { 0xff,0x80,0x80,0x80,0x80,0x80,0x80,0x80,	//column 0
									0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xff,
									0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//column 1
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
									0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//column 2
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
									0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//column 3
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

									0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//column 3
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
									0xf0,0x04,0x04,0x04,0x04,0x04,0x04,0x04,	//column 5
									0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xf0,
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//column 6
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//column 6

									// and the 16x16 sprite
									0xff,0x80,0x80,0x80,0x80,0x80,0x80,0x80,	//column 0
									0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xff,
									0xff,0x01,0x01,0x01,0x01,0x01,0x01,0x01,	//column 7
									0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xff, 
									
									// and the vbar cursor
									0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,	//column 0
									0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00	};
									
	OwnVDP();
	SetVDPIncrement( 2 );					//increment 2 bytes at a time
	SetVDPVRAMAddress( (unsigned long)kSpriteBase * 32 );
	
	ExpandDataToVDP( myBitData, 8*24 );		//Write out thin and wide cursors and insertion bar
											//8-bytes (at 1-bit) each, 8+8+4+4 chars total

	WriteVDPRegister( 12, 0x81 + 0x08 );	//Set no interlace, 40 cell horizontal, enable shadow and highright
	FreeVDP();
}
