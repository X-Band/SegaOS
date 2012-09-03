/*
	File:		Pattern.c

	Contains:	xxx put contents here xxx

	Written by:	Ted Cohn

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<16>	 8/27/94	HEC		Needed to reserve the "last" pattern cell to prevent it from
									being allocated.
		<15>	 7/27/94	HEC		Checked in a ";".
		<14>	 7/27/94	HEC		Fixed FindFreeRange bug introduced by Mr. Callahan where patEnd
									was way out of line.
		<13>	 7/26/94	CMY		Fixed FindFreeSpaceHigh to search from the correct pattern usage
									bitmap.
		<12>	 7/26/94	CMY		NewPatternBlockHigh - allocates pattern blocks from the top of
									Pattern mems.
		<11>	 7/24/94	ADS		got rid of temp leftovers
		<10>	 7/19/94	ADS		Vector Mania
		 <9>	 7/13/94	ADS		Better warnings from GetPatternBlock
		 <8>	  7/3/94	JBH		First patterns allocated from top of pattern memory. Allowed
									dialog manager to chunk these patterns (ie backdrops) when
									putting up a dialog in low-memory conditions.
		 <7>	  7/3/94	HEC		Integrated Sean's new and improved FindFreeRange code.
		 <6>	  6/5/94	SAH		Moved the clut fix to SetupVDP.
		 <5>	  6/5/94	HEC		Fix ugly transitions using clut blanking.
		 <4>	  6/4/94	SAH		Make sure to clear the patMap ptr at soft init time so we
									allocate some mems at InitPatternMgr.
		 <3>	 5/29/94	CMY		Increment free pattern count when client calls FreePatternBlock
		 <2>	 5/26/94	SAH		Include SegaVDP.h instead of VDP.h
		 <5>	 5/24/94	SAH		Fix the PatternGlobals name. Screw up Ted's static data until we
									get a better solution.
		 <4>	 5/23/94	SAH		Clear vdp memory when disposing patterns.
		 <3>	 5/19/94	HEC		Return offset+1 to caller to fix display bugs.
		 <2>	 5/18/94	HEC		Fixed bugs
		 <1>	 5/18/94	HEC		first checked in

	To Do:
*/


#include "SegaOS.h"
#include "Reliability.h"
#include "Heaps.h"
#include "Pattern.h"
#include "PatternPriv.h"
#include "Errors.h"
#include "SegaVDP.h"
#include "SegaScrn.h"
#include "SegaCluts.h"
 

#ifdef SIMULATOR 
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#endif

/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
PatternGlobals pattern;
#endif

short			_InitPatternManager( short numPats );
long			_NewPatternBlock( short numPats );
long			_NewPatternBlockHigh( short numPats );
void			_FreePatternBlock( long pattern );
void			_DeallocateTopPatternBlock( void );

//
// local vectorized
//
static long		_NewFirstPatternBlock( short numPats );
static void		_SetRange( short start, short size );
static void		_ClearRange( short start, short size );
static Boolean	_RangeIsFree( short start, short size );
static short	_FindFreeRange( short size );
static short *	_GetLeftOnesTable ( void );
static short *	_GetRightOnesTable ( void );

long	NewFirstPatternBlock( short numPats ) =
			CallDispatchedFunction( kNewFirstPatternBlock );

void	SetRange( short start, short size ) =
			CallDispatchedFunction( kSetRange );

void	ClearRange( short start, short size ) =
			CallDispatchedFunction( kClearRange );

Boolean	RangeIsFree( short start, short size ) =
			CallDispatchedFunction( kRangeIsFree );

short	FindFreeRange( short size ) =
			CallDispatchedFunction( kFindFreeRange );

short *	GetLeftOnesTable ( void ) =
			CallDispatchedFunction( kGetLeftOnesTable );

short *	GetRightOnesTable ( void ) =
			CallDispatchedFunction( kGetRightOnesTable );





long
_PatternControl ( short command, long data )
{
	long offset;
	short error;
	PatternGlobals *globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(pattern,offset);
			
			error = AllocateGlobalSpace ( kPatternManager, offset,
				sizeof(PatternGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create pattern globals" );
			if ( error != noErr )
				{
				return error;
				}
			
			globals->free = 0;
			globals->max = 0;
			globals->topPat = 0;
			globals->patMap = nil;
	
			/* install our selectors */
			SetDispatchedFunction ( kInitPatternManager,	kPatternManager,	_InitPatternManager );
			SetDispatchedFunction ( kNewPatternBlock,		kPatternManager,	_NewPatternBlock );
			SetDispatchedFunction ( kNewPatternBlockHigh,	kPatternManager,	_NewPatternBlockHigh );
			SetDispatchedFunction ( kFreePatternBlock,		kPatternManager,	_FreePatternBlock );
			SetDispatchedFunction ( kDeallocateTopPatternBlock, kPatternManager, _DeallocateTopPatternBlock );

			SetDispatchedFunction ( kNewFirstPatternBlock,	kPatternManager,	_NewFirstPatternBlock );
			SetDispatchedFunction ( kSetRange,				kPatternManager,	_SetRange );
			SetDispatchedFunction ( kClearRange,			kPatternManager,	_ClearRange );
			SetDispatchedFunction ( kRangeIsFree,			kPatternManager,	_RangeIsFree );
			SetDispatchedFunction ( kFindFreeRange,			kPatternManager,	_FindFreeRange );
			SetDispatchedFunction ( kGetLeftOnesTable,		kPatternManager,	_GetLeftOnesTable );
			SetDispatchedFunction ( kGetRightOnesTable,		kPatternManager,	_GetRightOnesTable );
			break;
		
		case kSoftInialize:
			error = noErr;
			REFGLOBAL(pattern,patMap) = 0L;
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

short 
_InitPatternManager ( short numPats )
{
	// This reinitializes the pattern buffer bitmap. This is equivalent to a flush. */
	unsigned short *patMap;
	short i;
	unsigned short size = numPats;

	if (numPats == 0)
		size = REFGLOBAL(pattern,max);

	if (REFGLOBAL(pattern,max) != size && REFGLOBAL(pattern,patMap))
	{
		DisposeMemory(REFGLOBAL(pattern,patMap));
		REFGLOBAL(pattern,patMap) = 0;
	}

	/* [Re]Initialize pattern heap to number of patterns passed in count */
	if (!REFGLOBAL(pattern,patMap))
	{
		REFGLOBAL(pattern,patMap) = NewMemory( kTemp, ((size+7)>>3)+2);
		ASSERT_MESG ( REFGLOBAL(pattern,patMap), "Can't allocate pattern bitmap" );
	}
	if (patMap = REFGLOBAL(pattern,patMap)) /* clear out pattern bitmap */
	{
		for (i = ((size+15)>>4); i; i--) 
			*patMap++ = 0;
		REFGLOBAL(pattern,max) = size;
		REFGLOBAL(pattern,free) = size-1;
		SetRange( size-1, 1 );	// cannot allocate last cell
	}
	
	REFGLOBAL(pattern, topPat) = 0;
	
	/* now go and actually erase the vdp patterns */
	size = numPats;
	if ( numPats == 0 )
		size = REFGLOBAL(pattern,max);
	
	FillVRAM ( 0, 0L, size << 3 );
	
	return (patMap != 0);
}

// returns 0 to indicate no pattern space available at requested size
// else returns long word token indicating offset and size of request.
// Use macros PATOFFSET() and PATSIZE() to get appropriate values.

long
_NewPatternBlock ( short numPats )
{
	long patBlock;
	short start;

	if (numPats > REFGLOBAL(pattern,free))
	{
		WARNING_MESG ( "Pattern buffer full" );
		return 0;
	}
	if (numPats <= 0)
	{
		WARNING_MESG ( "Trying to allocate zero patterns" );
		return 0;
	}
	
	if ( REFGLOBAL(pattern, free) == REFGLOBAL(pattern, max) || REFGLOBAL(pattern, topPat) == 0 )
		return (NewFirstPatternBlock( numPats ));
	
	/* Find free contiguous pattern block from pattern bitmap */
	start = FindFreeRange( numPats );
	/* If no contiguous space found, return nil */
	if (start < 0)
	{
		WARNING_MESG ( "Contiguous space not available in pattern buffer" );
		return 0;
	}
	patBlock = ((long)numPats<<16) | ((start+1)&0xFFFF);
	SetRange(start, numPats);
	REFGLOBAL(pattern,free) -= numPats;
	
	if (PATOFFSET(patBlock) >= REFGLOBAL(pattern,max))
		WARNING_MESG("NewPatternBlock bug: Offset too big!");
	
	if (PATOFFSET(patBlock) + PATSIZE(patBlock) >= REFGLOBAL(pattern,max))
		WARNING_MESG("NewPatternBlock bug: Size too big!");
	
	return patBlock;
}


// Finds first free chunk of bits of the requested size
static short
FindFreeRangeHigh(short size)
{
	unsigned short *patPtr = (unsigned short *)((char *)REFGLOBAL(pattern,patMap) + (REFGLOBAL(pattern,max)>>3));
	unsigned short *patEnd = REFGLOBAL(pattern,patMap);
	register unsigned short mask;
	register short data;
	short start = REFGLOBAL(pattern,max);
	short free = 0;
	
	// skipping ones
	
	--patPtr;
	while (patPtr >= patEnd) {
		data = *patPtr--;
		if (data == 0) {
			goto fullZero;
		} else if (data == (short)0xffff) {
fullOne:	start -= 16;
			continue;
		} else {
			mask = 0x0001;
			while (mask) {
				if (!(data & mask)) {
					free = 0;
					goto partialZero;
				}
partialOne:		--start;
				mask <<= 1;
			}
		}
	}
	goto bail;
	
	// counting zeroes
	
	while (patPtr >= patEnd) {
		data = *patPtr--;
		if (data == 0)
fullZero:	free += 16;
		else if (data == (short)0xffff) {
			start -= free;
			free = 0;
			goto fullOne;
		} else {
			mask = 0x0001;
			while (mask) {
				if (data & mask) {
					if (free >= size)
						return (start-size);
					start -= free;
					free = 0;
					goto partialOne;
				}
partialZero:	++free;
				mask <<= 1;
			}
		}
		if (free >= size)
			return (start-size);
	}
bail:
	return (-1);
}

static long
_NewPatternBlockHigh( short numPats )
{
	short start = PATOFFSET(REFGLOBAL(pattern,topPat));
	long patBlock;
	
	if (start == 0)
		return NewFirstPatternBlock( numPats );
	
	start = FindFreeRangeHigh( numPats );
	if (start < 0)
	{
		WARNING_MESG ( "Contiguous space not available in pattern buffer" );
		return 0;
	}
	patBlock = ((long)numPats<<16) | ((start+1)&0xFFFF);

#ifdef SIMULATOR
	if ( RangeIsFree(start, numPats) == false )
	{
		ERROR_MESG( "High pattern allocator bug");
	}
#endif

	SetRange(start, numPats);
	REFGLOBAL(pattern,free) -= numPats;
	
	if (PATOFFSET(patBlock) >= REFGLOBAL(pattern,max))
		WARNING_MESG("NewPatternBlock bug: Offset too big!");
		
	if (PATOFFSET(patBlock) + PATSIZE(patBlock) >= REFGLOBAL(pattern,max))
		WARNING_MESG("NewPatternBlock bug: Size too big!");
		
	return patBlock;
}

static long
_NewFirstPatternBlock( short numPats )
{
	long patBlock;
	short start;

	if ((numPats > REFGLOBAL(pattern,free)) || (numPats <= 0))
	{
		WARNING_MESG ( "Pattern buffer full" );
		return 0;
	}
	
	start = REFGLOBAL(pattern, max) - numPats - 4;		/* why 4??? Who owns these? */
	
	patBlock = ((long)numPats<<16) | ((start+1)&0xFFFF);
	SetRange(start, numPats);
	REFGLOBAL(pattern, free) -= numPats;
	REFGLOBAL(pattern, topPat) = patBlock;
	return patBlock;
}

void
_FreePatternBlock ( long patBlock )
{
unsigned long base;

	if ((PATSIZE(patBlock) <= 0) || (PATOFFSET(patBlock) + PATSIZE(patBlock) >= REFGLOBAL(pattern,max)))
	{
		WARNING_MESG( "invalid pattern size on free" );
	}
	else
	{
		ClearRange(PATOFFSET(patBlock)-1, PATSIZE(patBlock));
		REFGLOBAL(pattern,free) += PATSIZE(patBlock);
		
		/* erase the vdp patterns */
		base = PATOFFSET(patBlock);
		base <<= 5;
		FillVRAM ( 0, (Ptr) base, PATSIZE(patBlock) << 3 );
	}
	
	/* if we're destroying the "top" pattern (it's a deep-shit dialog), mark next alloc from top */
	if (patBlock == REFGLOBAL(pattern, topPat)) {
		REFGLOBAL(pattern, topPat) = 0;
	}
	
}

void
_DeallocateTopPatternBlock( void )
{
	_FreePatternBlock(REFGLOBAL(pattern, topPat));
}


static
short * _GetRightOnesTable ( void )
{

	asm {
		lea		@rightOnes,a0
		move.l	a0,d0
		rts

@rightOnes
		dc.w	0xffff
		dc.w	0x7fff
		dc.w	0x3fff
		dc.w	0x1fff
		dc.w	0x0fff
		dc.w	0x07ff
		dc.w	0x03ff
		dc.w	0x01ff
		dc.w	0x00ff
		dc.w	0x007f
		dc.w	0x003f
		dc.w	0x001f
		dc.w	0x000f
		dc.w	0x0007
		dc.w	0x0003
		dc.w	0x0001
	}
}

static
short * _GetLeftOnesTable ( void )
{
	asm {
		lea		@leftOnes,a0
		move.l	a0,d0
		rts

@leftOnes
		dc.w	0x8000
		dc.w	0xc000
		dc.w	0xe000
		dc.w	0xf000
		dc.w	0xf800
		dc.w	0xfc00
		dc.w	0xfe00
		dc.w	0xff00
		dc.w	0xff80
		dc.w	0xffc0
		dc.w	0xffe0
		dc.w	0xfff0
		dc.w	0xfff8
		dc.w	0xfffc
		dc.w	0xfffe
		dc.w	0xffff
	}
}


static void
_SetRange(short start, short size)
{
	unsigned short *patPtr = REFGLOBAL(pattern,patMap);
	short soffset = start / 16;				// word containing first bit
	short sindex  = start % 16;				// bit index into word
	short eoffset = (start+size-1) / 16;	// word containing last bit
	short eindex  = (start+size-1) % 16;	// last bit of last word
	short i;
	short * leftOnes;
	short * rightOnes;
	
	leftOnes = GetLeftOnesTable();
	rightOnes = GetRightOnesTable();

	patPtr += soffset;
	if (soffset == eoffset) {
		*patPtr |= (rightOnes[sindex] & leftOnes[eindex]);
	} else {
		// set left side
		*patPtr++ |= rightOnes[sindex];
		// set center
		for (i=eoffset-soffset-1; i>0; i--)
			*patPtr++ |= 0xffff;
		// set right side
		*patPtr |= leftOnes[eindex];
	}
}

static void
_ClearRange(short start, short size)
{
	unsigned short *patPtr = REFGLOBAL(pattern,patMap);
	short soffset = start / 16;				// word containing first bit
	short sindex  = start % 16;				// bit index into word
	short eoffset = (start+size-1) / 16;	// word containing last bit
	short eindex  = (start+size-1) % 16;	// last bit of last word
	short i;
	short * leftOnes;
	short * rightOnes;
	
	leftOnes = GetLeftOnesTable();
	rightOnes = GetRightOnesTable();

	patPtr += soffset;
	if (soffset == eoffset) {
		*patPtr &= ~(rightOnes[sindex] & leftOnes[eindex]);
	} else {
		// clear left side
		*patPtr++ &= ~rightOnes[sindex];
		// clear center
		for (i=eoffset-soffset-1; i>0; i--)
			*patPtr++ = 0;
		// clear right side
		*patPtr &= ~leftOnes[eindex];
	}
}

static Boolean
_RangeIsFree(short start, short size)
{
	unsigned short *patPtr = REFGLOBAL(pattern,patMap);
	short soffset = start / 16;				// word containing first bit
	short sindex  = start % 16;				// bit index into word
	short eoffset = (start+size-1) / 16;	// word containing last bit
	short eindex  = (start+size-1) % 16;	// last bit of last word
	short i;
	short * leftOnes;
	short * rightOnes;
	
	leftOnes = GetLeftOnesTable();
	rightOnes = GetRightOnesTable();

	patPtr += soffset;
	if (soffset == eoffset) {
		return (*patPtr & (rightOnes[sindex] & leftOnes[eindex])) == 0;
	} else {
		// check left side
		if (*patPtr++ & rightOnes[sindex])
			return false;
			
		// check center
		for (i=eoffset-soffset-1; i>0; i--)
			if ( *patPtr++ )
				return false;
				
		// check right side
		if (*patPtr & leftOnes[eindex])
			return false;
	}
	
	return true;
}

// Finds first free chunk of bits of the requested size
static short
_FindFreeRange(short size)
{
	unsigned short *patPtr = REFGLOBAL(pattern,patMap);
	unsigned short *patEnd = patPtr + (REFGLOBAL(pattern,max) >> 4);
	register unsigned short mask;
	register short data;
	short start = -1;
	short free = 0;
	
	// skipping ones
	
	while (patPtr < patEnd) {
		data = *patPtr++;
		if (data == 0) {
			goto fullZero;
		} else if (data == (short)0xffff) {
fullOne:	start += 16;
			continue;
		} else {
			mask = 0x8000;
			while (mask) {
				if (!(data & mask)) {
					free = 0;
					goto partialZero;
				}
partialOne:		++start;
				mask >>= 1;
			}
		}
	}
	goto bail;
	
	// counting zeroes
	
	while (patPtr < patEnd) {
		data = *patPtr++;
		if (data == 0)
fullZero:	free += 16;
		else if (data == (short)0xffff) {
			start += free;
			free = 0;
			goto fullOne;
		} else {
			mask = 0x8000;
			while (mask) {
				if (data & mask) {
					if (free >= size)
						return (start + 1);
					start += free;
					free = 0;
					goto partialOne;
				}
partialZero:	++free;
				mask >>= 1;
			}
		}
		if (free >= size)
			return (start + 1);
	}
bail:
	return (-1);
}


