/*
	File:		SegaBM.h

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<18>	 8/20/94	KON		DrawGraphic now takes a hueShift rather than a clut ID.
		<17>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<16>	 8/12/94	JOE		put back in Kon's GetIconBitMap() call
		<15>	 8/12/94	JOE		update SegaIcons protos
		<14>	 8/10/94	KON		Add getIconBitMap call.
		<13>	  8/5/94	JBH		Added kGetGraphicsReferenceClut.
		<12>	  8/3/94	JOE		new icon routines
		<11>	  8/3/94	JOE		Add DrawDBIconWithoutIREF
		<10>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		 <9>	 7/21/94	CMY		Added DrawGraphicGray.
		 <8>	 7/18/94	KON		Return IconReference when drawing an icon.
		 <7>	 7/18/94	KON		Add drawing and disposing of icons by reference.
		 <6>	 7/16/94	BET		Add #ifdef __SERVER__ to vectorized code.
		 <5>	  7/5/94	JOE		added extern for NukeRAMDBIcon().  put in support for lipless
									crankbaits, jolly rigs coming soon.
		 <4>	  7/2/94	JOE		add externs for DrawIcon, DisposeIconReference
		 <3>	 6/28/94	CMY		Added graphics references to handle clut and pattern management.
		 <2>	 6/13/94	SAH		Managerized.
		 <3>	 5/19/94	SAH		Moved SegaBitMap definition.
		 <2>	 5/18/94	HEC		Changing return type of DrawGraphic to long

	To Do:
*/


#ifndef __SegaBM__
#define	__SegaBM__

#ifndef __SegaGraphics__
#include "SegaGraphics.h"
#endif

#ifndef __SegaIcons__
#include "SegaIcons.h"
#endif


/*
 * Flags bit masks for the "options" parameter in SetDecompressorOptions
 */
#define kOmitTransparentCells	1		/* Don't write cells that are all 0 to the destination */
#define kDrawGraphicInFront		2		/* Set the priority bit */


#include "Pattern.h"

#ifdef __SERVER__

#define DrawGraphic( a,b,c,d,e,f )				_DrawGraphic( a,b,c,d,e,f )
#define DisposeGraphicReference( a )			_DisposeGraphicReference( a )
#define DrawIcon( a,b,c )						_DrawIcon( a,b,c )
#define DisposeIconReference( a )				_DisposeIconReference( a )
#define NukeRAMDBIcon( a )						_NukeRAMDBIcon( a )
#define	GetGraphicReferenceClut( a )			_GetGraphicReferenceClut( a )

#else

GraphicReference DrawGraphic( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse, long decompressorOptions, unsigned char hueShift ) =
	CallDispatchedFunction( kDrawGraphic );

GraphicReference DrawGraphicGray( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse, long decompressorOptions ) =
	CallDispatchedFunction( kDrawGraphicGray );

void DisposeGraphicReference( GraphicReference ref ) =
	CallDispatchedFunction( kDisposeGraphicReference );
	
short GetGraphicReferenceClut( GraphicReference ref ) =
	CallDispatchedFunction( kGetGraphicReferenceClut );
	

IconReference DrawPlayerIcon( userIdentification *user, short xPos, short yPos, long flags ) =
	CallDispatchedFunction( kDrawPlayerIcon );

void NukePlayerRAMIcon( userIdentification *user ) =
	CallDispatchedFunction( kNukePlayerRAMIcon );
	
DBID PlayerRAMIconExists( userIdentification *user ) =
	CallDispatchedFunction( kPlayerRAMIconExists );
	
SegaBitMapPtr GetPlayerRAMIconBitMap( userIdentification *user ) =
	CallDispatchedFunction( kGetPlayerRAMIconBitMap );
	
SegaBitMapPtr GetPlayerIconBitMap( userIdentification *user ) =
	CallDispatchedFunction( kGetPlayerIconBitMap );

SegaBitMap * GetIconBitMap( DBID iconID ) =
	CallDispatchedFunction( kGetIconBitMap );
		
void DisposeIconReference( IconReference ref ) = 
	CallDispatchedFunction( kDisposeIconReference );
	
#endif // __SERVER__

#endif
