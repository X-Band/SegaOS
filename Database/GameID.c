/*
	File:		GameID.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<20>	 8/23/94	HEC		Use consts for game ids
		<19>	 8/22/94	SAH		Use FORCE_HARD_HERE conditional.
		<18>	 8/22/94	ADS		Fixed Fred mapping problems
		<17>	 8/22/94	CMY		Changed code that tried to locate code to assume procPtrs point
									to the actual code, not a JT entry.
		<16>	 8/21/94	ADS		New lomem layout
		<15>	 8/17/94	HEC		Added (commented out) game ids for NEW JAMS and MK for testing.
		<14>	  8/2/94	SAH		Fixed some whacky shit for the stimulator.
		<13>	 7/12/94	SAH		The stimulator now uses game id 666.
		<12>	  7/8/94	SAH		Killed CheckCodeForJTREfs as it fucks the sega build.
		<11>	  7/8/94	CMY		Fixed call to CheckCodeForJTREfs
		<10>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <9>	  7/6/94	HEC		Disable interrupts during ReadCurrentCartridge.
		 <8>	 6/16/94	SAH		Revved to latest global structure.
		 <7>	 6/15/94	SAH		Read the cartridge the new way.
		 <6>	 6/12/94	DJ		Simulator now thinks it is playing Mortal Kombat
		 <5>	  6/5/94	SAH		Only checksum from 100 to 1e0 to avoid activating rosko's magic
									return.
		 <4>	  6/4/94	SAH		Fixed.
		 <3>	  6/4/94	SAH		Managerized.
		 <2>	  6/4/94	HEC		Install ReadGetGameID as temp mem proc to actually do its thing.

	To Do:
*/

#include "SegaOS.h"
#include "Errors.h"
#include "GameID.h"
#include "Heaps.h"
#include "MungeCode.h"
#include "RoskoSega.h"
#include "GameIDPriv.h"
#include "GameDB.h"
#include "OSGlobals.h"
#include "hardDefines.h"
#include "debugSelectors.h"

/*******************************************************************
The following ID Criteria seem to define uniqueness of cartridges,
in order of presumed importance:

1. Product Number

From $184 until $18e. This includes the publisher's ID code (e.g. T-81 
seems to be Arena Entertainment), the cart code, and version codes. The
version codes are all zeros in the carts I've seen, and they either follow
spaces, dashes, or commas.

The publisher's ID codes I've seen are "T-XX" for non-Sega, and "MK-1" for
Sega. On the carts I've looked at, it is always T-XX followed by a 
3 digit game code, or MK-1 followed by a 3 digit code. The only exception
I've found is NBA JAM which has "T-081326" (there is a zero before the
publisher's ID code of 81. So, perhaps the rule should be to find the
publisher's ID code after the first non-zero digit. Clearly, we need
to test this on more carts. Anyway, we get 1 bit as to whether it is
MK or T, and 5 decimal digits which yield around 16 bits, and since the
version codes seem to cluster around zero, we might just add their value
into the mix.

2. Check Sum

A short at $18E. Checksum appears to be a sum of all of the words in
the ROM, starting at address $200. This is 16 bits.

3. Country

Starting at $1f0, some permutation of up to one of each 'J', 'U',
and 'E' (for Japan, US, and Europe), until the first space. Order
does vary in the carts I've checked. That gives us 15 distinct
possibilities (about 4 bits), but certain combinations are much
more likely than others.

4. Copyright Date

Assuming we don't expect this product to live for more than 10
years, then use the x from 199x or 198x is at $11b, and the 3
chars of the month (e.g. "APR") from $11d to $11f. That is 10
possibilities for the year, clustered around the lower numbers,
and 12 possibilities for the month, possibly clustered around
certain months (e.g. for XMas intro). This yields a total of 
10x12 = 120 combinations, or about 7 bits.

5. Game name

Domestic use name: from $120 until first space or $150.
Overseas use name: from $150 until first space or $180.

Note that at least one cart, Tecmo Super Bowl, has no domestic
use name. Most of chars are the uppercase ASCII alphabet, but
char $87 occurs in TECMO SUPERBOWL, and "'" ($27) occurs in Street
Fighter II Champion Edition.

So...

Proposed method:

MSW: company/product number (16-bit binary representation of first five
non-zero digits after '-') + sum of all words from $180 through $18f.

LSW: Sum of all words rom $100 through $1ff.

This certainly could be defeated if someone deliberately tried, but
barring that, it is highly unlikely that it will accidentally match up.
And, if someone defeats it, we'll simply special case for that card.

*************************************************************************/

#ifdef SIMULATOR
// Sample cartridge data
unsigned char IDData[256] = { // test data: jams.id 

0x53,0x45,0x47,0x41,0x20,0x47,0x45,0x4E,0x45,0x53,0x49,0x53,0x20,0x20,0x20,0x20,
0x28,0x43,0x29,0x54,0x2D,0x38,0x31,0x20,0x31,0x39,0x39,0x33,0x2E,0x44,0x45,0x43,
0x4E,0x42,0x41,0x20,0x4A,0x41,0x4D,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x4E,0x42,0x41,0x20,0x4A,0x41,0x4D,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x47,0x4D,0x20,0x54,0x2D,0x30,0x38,0x31,0x33,0x32,0x36,0x20,0x30,0x30,0x69,0x4F,
0x4A,0x34,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0x00,0xFF,0x00,0x00,0x00,0xFF,0xFF,0xFF,
0x52,0x41,0xE8,0x40,0x00,0x20,0x00,0x01,0x00,0x20,0x00,0x01,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x45,0x55,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20 };

#endif


/* dispatched functions */
long _GetGameID( void );

/* internal functions */
static	long ReadGameID ( void );
static	void ReadCurrentCartridge ( void );


/* if we're not on the simulator, we need to allocate the globals for real */
#ifndef SIMULATOR
CartridgeGlobals cart;
#endif


long
_CartridgeControl ( short command, long data )
{
long offset;
short error;
CartridgeGlobals * globals;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* allocate and initialize our globals */
			GETMGRGLOBALSOFFSET(cart,offset);
			
			error = AllocateGlobalSpace ( kCartManager, offset, sizeof(CartridgeGlobals), (Ptr *) &globals );
			ASSERT_MESG ( !error, "Can't create cartridge globals" );
			if ( error != noErr )
				{
				return error;
				}
			globals->gameID = 0;
			
			/* install our selectors */
			SetDispatchedFunction ( kGetGameID,		kCartManager,	_GetGameID );
			break;
		
		case kSoftInialize:
			error = noErr;
			/* init our globals for this time around */
			ReadCurrentCartridge();
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



static
long ReadGameID( void )
{
unsigned char *	bytePtr;
unsigned short *wordPtr;
short			i;
unsigned char	c;
unsigned long	leftShift;
unsigned long	accum;

//long			controlRegBase = gControlRegisters;

/*  controlregbase MUST be copied to the stack BEFORE we throw the
	switch, because it's in the 100-180 space and that will make it disappear!
*/

#ifdef FORCE_HARD_HERE
	
	/* we know we're executing out of ram here. so, we flip rom and ram */
	/* so that rom is low. this leaves $100-200 unmapped and we can read the cartridge */
	asm
		{
		moveq	#0,d0
		move.b	d0,kDefaultControl+kControlReg+1
		}
	
#else
	/* we know we're executing out of ram here. so, we flip rom and ram */
	/* so that rom is low. this leaves $100-200 unmapped and we can read the cartridge */

	// The following code is EXTREMELY magic, it depends heavily on the
	// mode settings used elsewhere in the chip.  Don't touch it unless
	// you really understand the workings of Fred!!!!!   Fucker!!!!

	asm {
		move.b		#kEnFixedInternal+kEnSafeRom,kSoftOffset+kControlReg+1
		move.b		#kHereAssert,kSoftOffset+kKillReg+1
		
		move.w		0x64,d0
	}
#endif
	
	bytePtr = (unsigned char *)(IDBlock + cartIDStart);	// point to cart ID line 	

	while ((*bytePtr++ != '-') &&
				((long)bytePtr != cartIDEnd)) ;			// find 1st '-' 
	
	while ((*bytePtr == '0') &&
				((long)bytePtr != cartIDEnd)) 			// skip leading zeros 
		bytePtr++;

	leftShift = 16;
	accum = 0;
	for (i = 0; i < 5; i++) {							// ascii dec to bin 
		c = *bytePtr++;
		if ((c >= '0') && (c <= '9'))
			accum |= ( (long) (c - '0')) << leftShift;
		else
			break;
		
		leftShift -= 4;
	}
	
	for (wordPtr = (unsigned short *)(IDBlock + cartIDStart);			// checksum cartID line
			(long)wordPtr != IDBlock + cartIDStart + 0x10; wordPtr++)
		accum += *wordPtr;
	accum <<= 16;										// move to MSW
	
	/* checksum out to 0x1e0 so we don't hit the magic patch */
	for (wordPtr = (unsigned short *)IDBlock;			// checksum IDBlock
			wordPtr != (unsigned short *)(IDBlock + 0xe0); wordPtr++)
		accum += *wordPtr;

#ifdef FORCE_HARD_HERE
	asm
		{
		move.b	#kRomHi,kDefaultControl+kControlReg+1
		}
#else
	// The following code is EXTREMELY magic, it depends heavily on the
	// mode settings used elsewhere in the chip.  Don't touch it unless
	// you really understand the workings of Fred!!!!!   Fucker!!!!

	asm {
		moveQ		#0,d0
		move.b		d0,kDefaultControl+kKillReg+1
		
		move.w		0x64,d0

		move.b		#kEnSegaExcept+kEnFixedInternal+kEnSafeRom+kRomHi,kSoftOffset+kControlReg+1
	}
#endif
		
	return accum;
}




long _GetGameID( void )
{
	return REFGLOBAL(cart,gameID);
}

typedef long (*ReadGameIDProc)( void );

/*
* Go off to cartridge space and read all that our manager needs to know
*/
static
void ReadCurrentCartridge ( void )
{
long	size;
Ptr		data;

	/* read the game id first */

	
#ifndef SIMULATOR
	size = (long)_GetGameID -  (long)ReadGameID;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for ReadGameID proc" );
#endif
	
	if ( data )
	{
#ifdef SIMULATOR
		/* the test game patch id */
		REFGLOBAL(cart,gameID) = 666;
		//REFGLOBAL(cart,gameID) = kNBAJamGameID;
		//REFGLOBAL(cart,gameID) = kMortalKombatGameID;
#else
		SegaBlockMove ( (Ptr)ReadGameID, data, size );
		
		asm { move.w	sr,-(sp) }
		asm { move.w	#0x2700,sr }
		REFGLOBAL(cart,gameID) = (* (ReadGameIDProc) data)();
		asm { move.w	(sp)+,sr }
		DisposeMemory( data );
#endif
	}
}
