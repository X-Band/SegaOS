/*
	File:		SegaVDP.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 7/29/94	JOE		include errors.h for the sim build so ERROR_MESG works
		 <7>	 7/29/94	JOE		Made gVDPBusy a counter
		 <6>	 7/29/94	JOE		Added OwnVDP/FreeVDP/VDPBusy macros
		 <5>	  7/6/94	CMY		Added support for VRAM reads
		 <4>	 5/30/94	SAH		More of the below.
		 <3>	 5/28/94	SAH		Added some really nasty asm macros to get around think
									nastiness.
		 <2>	 5/26/94	SAH		Made a manager.
		 <2>	 5/24/94	SAH		Moved non-simulator VDPIdle define to vdp.h

	To Do:
*/


#ifndef __SegaVDP__
#define	__SegaVDP__

#ifndef __SegaOS__
#include "SegaOS.h"
#endif

#ifndef __VDP__
#include "vdp.h"
#endif


#ifndef SIMULATOR
#define	VDPIsBusy()		gVDPBusy
#define	OwnVDP()		gVDPBusy++
#define	FreeVDP()		if (gVDPBusy) gVDPBusy--
#else
#ifndef __Errors__
#include "Errors.h"
#endif
#define	VDPIsBusy()		gVDPBusy
#define	OwnVDP()		gVDPBusy++
#define	FreeVDP()		if (gVDPBusy) gVDPBusy--; else ERROR_MESG ( "Trying to decrement gVDPBusy when 0!  Someone fucked up!" );
#endif


/*
* These functions do not go through the dispatcher as they are turned into macros
* on the sega build.
*/

#ifdef SIMULATOR

extern void SetVDPIncrement( short newIncrement );
extern void SetVDPVRAMAddress( unsigned long a );
extern void SetVDPCRAMAddress( long a );
extern void WriteByteToVDP( unsigned char myByte );
extern void WriteWordToVDP( unsigned short myWord );
extern void SetVDPVRAMReadAddress( unsigned long a);
extern unsigned short ReadWordFromVDP(void);
extern Rect	gSpriteRect;
extern void InitSegaVDP( void );

#define WriteVDPRegister( a, b )	VRegWrite( 0x8000 | ( a<<8 ) | b )

#else

#if 0

#define SetVDPIncrement( a )  *(short *)0x00C00004 = ((0x8f00) | (a) )
#define SetVDPVRAMAddress( a )	{ *(short *)0x00C00004 = ((0x4000) | (((long)a) & 0x3FFF)); *(short *)0x00C00004 = (((long)a) >> 14); }
#define SetVDPCRAMAddress( a )	{ *(short *)0x00C00004 = ((0xC000) | (((long)a) & 0x3FFF)); *(short *)0x00C00004 = (((long)a) >> 14); }
#define WriteByteToVDP( b ) *(unsigned char *)0x00C00000 = (((unsigned char) b))
#define WriteWordToVDP( b ) *(unsigned short *)0x00C00000 = ((unsigned short) b)
#define WriteVDPRegister( a, b )	( *(short *) 0xc00004 ) =  ( 0x8000 | ( a<<8 ) | b )

#else

/* BRAIN DAMAGE: This code is really gross. It's still better than what think generates in that */
/* it doesn't crash the vdp and is in fact faster in general */
#define SetVDPIncrement( a )  *(short *)0x00C00004 = ((0x8f00) | (a) )
#define SetVDPAddress( a,b )	{ \
									short	vdpBits;			  \
									short	value;				  \
																  \
									vdpBits = b;				  \
									value = a;					  \
									asm {	moveq	#0,d0		} \
									asm {	move.w	value,d0	} \
									asm {	lsl.l	#2,d0 		} \
									asm {	lsr.w	#2,d0		} \
									asm {	or.w	vdpBits,d0	} \
									asm {	swap.w	d0			} \
									asm {	move.l	d0,0xc00004 } \
								}

#define SetVDPVRAMAddress( a )	SetVDPAddress( a, 0x4000 )
#define SetVDPVRAMReadAddress( a )	SetVDPAddress( a, 0x0000 )
#define SetVDPCRAMAddress( a )	SetVDPAddress( a, 0xC000 )

/* BRAIN DAMAGE: I don't like this, but the vdp doesn't like clr.w $c00000 either... */
#define WriteWordToVDP( b ) 	{ \
								unsigned short temp; \
									temp = b; \
									*(unsigned short *) 0xc00000 = temp; \
								}
#define WriteVDPRegister( a, b )	( *(short *) 0xc00004 ) =  ( 0x8000 | ( a<<8 ) | b )
#define ReadWordFromVDP()		( *(short *) 0xc00000 )
#endif

#endif


/*
* These functions are dispatched as they are real code on the sega
*/

void DMAToVRAM(void *src,void *dst,unsigned short wcount) =
	CallDispatchedFunction( kDMAToVRAM );

void CopyToVRAM(void *src,void *dst,unsigned short wcount) =
	CallDispatchedFunction( kCopyToVRAM );

void CopyToCRAM(void *src,void *dst,short wcount) =
	CallDispatchedFunction( kCopyToCRAM );

void CopyToVSRAM(void *src,void *dst,short wcount) =
	CallDispatchedFunction( kCopyToVSRAM );

void CopyToVMap(void *src,void *dst,short x,short y,short width,short height,short bump) =
	CallDispatchedFunction( kCopyToVMap );

void FillVRAM(long value,void *dst,long lcount) =
	CallDispatchedFunction( kFillVRAM );

void FillCRAM(long value,void *dst,long lcount) =
	CallDispatchedFunction( kFillCRAM );

void FillVSRAM(long value,void *dst,long lcount) =
	CallDispatchedFunction( kFillVSRAM );

#endif
