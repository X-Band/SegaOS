/*
	File:		MiscDBItemsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Shannon Holland

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/21/94	KON		It's a hueShift, not a clutID, that DrawGraphic takes.
		<14>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<13>	 8/12/94	JOE		put back in Kon's GetIconBitMap() call
		<12>	 8/12/94	JOE		SegaIcons call updates
		<11>	 8/10/94	KON		Added GetIconBitmap call.
		<10>	  8/3/94	JOE		protos for new icon routines
		 <9>	  8/3/94	JOE		Add DrawDBIconWithoutIREF
		 <8>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		 <7>	 7/21/94	CMY		DrawIconGray
		 <6>	 7/18/94	KON		Return IconReference when drawing an icon.
		 <5>	 7/18/94	KON		Add drawing and disposing of icons by reference.
		 <4>	  7/5/94	JOE		added proto for NukeRAMDBIcon()
		 <3>	  7/2/94	JOE		add protos for _DrawIcon, _DisposeIconReference, moved proto for
									_DisposegraphicReference here from GraphicsDB.c
		 <2>	 6/28/94	CMY		Added graphics references.
		 <1>	 6/13/94	SAH		first checked in

	To Do:
*/


#ifndef __MiscDBItemsPriv__
#define	__MiscDBItemsPriv__


#ifndef __Database__
#include "Database.h"
#endif

#ifndef __SegaGraphics__
#include "Sega Graphics.h"
#endif

/* prototypes from GraphicsDB.c */
SegaBitMapPtr	_GetDBGraphics( DBID graphicNum );
GraphicReference _DrawDBGraphic( long memToUse, DBID theID, short theScreen );
GraphicReference _DrawDBGraphicAt( long memToUse, DBID theID, short xPos, short yPos, short theScreen );

/* prototypes from SegaMB.c */
GraphicReference _DrawGraphic( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse, long decompressorOptions, unsigned char hueShiftAmount );
void _DisposeGraphicReference( GraphicReference ref );

/* prototypes from SegaIcons.c */
IconReference _DrawPlayerIcon( userIdentification *user, short xPos, short yPos, long flags );
void _NukePlayerRAMIcon( userIdentification *user );
DBID _PlayerRAMIconExists( userIdentification *user );
SegaBitMapPtr _GetPlayerRAMIconBitMap( userIdentification *user );
SegaBitMapPtr _GetPlayerIconBitMap( userIdentification *user );
SegaBitMap * _GetIconBitMap( DBID iconID );
void _DisposeIconReference( IconReference ref );

#endif