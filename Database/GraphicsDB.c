/*
	File:		GraphicsDB.c

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<23>	 8/21/94	KON		CreateSpriteInFront now takes a hueShift.
		<22>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<21>	 8/12/94	JOE		put back in Kon's GetIconBitMap() call.3
		<20>	 8/12/94	JOE		SegaIcons call updates
		<19>	 8/10/94	KON		Added GetIconBitmap call.
		<18>	  8/9/94	KON		Check for nil ref in disposeGraphicRef.
		<17>	  8/9/94	HEC		Include errors.h
		<16>	  8/8/94	KON		Mark backdrop dirty (via ClearBackdrop) if drawing a graphic
									into scrollB.
		<15>	  8/5/94	JBH		Added kGetGraphicsReferenceClut
		<14>	  8/3/94	JOE		install new icon routines
		<13>	  8/3/94	JOE		Add DrawDBIconWithoutIREF
		<12>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		<11>	 7/21/94	CMY		Added DrawGraphicGray
		<10>	 7/19/94	CMY		Added GetDBButtonGraphic
		 <9>	 7/18/94	KON		Add drawing and disposing icons by reference.
		 <8>	 7/18/94	ADS		Dispatcher cleanup
		 <7>	  7/2/94	JOE		SetDispatchedFunction() for DrawIcon(), DisposeIconReference()
		 <6>	 6/28/94	CMY		Added graphics references to handle clut and pattern management.
		 <5>	 6/13/94	SAH		Managerized.
		 <4>	  6/3/94	DJ		added screen as param to DrawDBGraphic
		 <3>	 5/29/94	HEC		Added DrawDBGraphicAt()
		 <2>	 5/29/94	KON		Trashed on last checkin.
		 <1>	 5/29/94	KON		first checked in

	To Do:
*/

#include "SegaScrn.h"
#include "SegaBM.h"
#include "DataBase.h"
#include "StringDB.h"
#include "Heaps.h"
#include "DBTypes.h"
#include "SegaText.h"
#include "GraphicsDB.h"
#include "MiscDBItemsPriv.h"
#include "Backdrops.h"
#include "Errors.h"

SegaBitMapPtr _GetDBButtonFrame( DBID buttonNumber, DBID frame );
GraphicReference _DrawGraphicGray( SegaBitMapPtr myGraphic, short xPos, short yPos, long memToUse );
short _GetGraphicReferenceClut( GraphicReference ref );
short _HueShift( short color, unsigned char shiftAmount );

long
_MiscDBItemsControl ( short command, long data )
{
long offset;
short error;

	error = commandSelectorUnknown;

	switch ( command )
		{
		case kHardInitialize:
			/* install our selectors */
			SetDispatchedFunction ( kGetDBGraphics,			kMiscDBItemsManager,	_GetDBGraphics );
			SetDispatchedFunction ( kDrawDBGraphic,			kMiscDBItemsManager,	_DrawDBGraphic );
			SetDispatchedFunction ( kDrawDBGraphicAt,		kMiscDBItemsManager,	_DrawDBGraphicAt );
			SetDispatchedFunction ( kDrawGraphic,			kMiscDBItemsManager,	_DrawGraphic );
			SetDispatchedFunction ( kDisposeGraphicReference, kMiscDBItemsManager,	_DisposeGraphicReference );
			SetDispatchedFunction ( kDrawPlayerIcon, 		kMiscDBItemsManager,	_DrawPlayerIcon );
			SetDispatchedFunction ( kNukePlayerRAMIcon, 	kMiscDBItemsManager,	_NukePlayerRAMIcon );
			SetDispatchedFunction ( kPlayerRAMIconExists, 	kMiscDBItemsManager,	_PlayerRAMIconExists );
			SetDispatchedFunction ( kGetPlayerRAMIconBitMap,kMiscDBItemsManager,	_GetPlayerRAMIconBitMap );
			SetDispatchedFunction ( kGetPlayerIconBitMap,	kMiscDBItemsManager,	_GetPlayerIconBitMap );
			SetDispatchedFunction ( kGetIconBitMap,			kMiscDBItemsManager,	_GetIconBitMap );
			SetDispatchedFunction ( kDisposeIconReference, 	kMiscDBItemsManager,	_DisposeIconReference );
			SetDispatchedFunction ( kGetDBButtonFrame, 		kMiscDBItemsManager,	_GetDBButtonFrame );
			SetDispatchedFunction ( kDrawGraphicGray, 		kMiscDBItemsManager,	_DrawGraphicGray );
			SetDispatchedFunction ( kGetGraphicReferenceClut, kMiscDBItemsManager,	_GetGraphicReferenceClut );
			SetDispatchedFunction ( kHueShift, 				kMiscDBItemsManager,	_HueShift );
			break;
		
		case kSoftInialize:
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


GraphicReference _DrawDBGraphic( long memToUse, DBID theID, short theScreen )
{
SegaBitMapPtr myGraphic;

	if( theScreen == kScrollB )
		ClearBackdrop( false );		//mark backdrop dirty, but don't erase

//
// Draw the graphic
//

	SetCurrentDevice( theScreen );	// was kScrollB
	myGraphic = GetDBGraphics( theID );

#ifdef SIMULATOR

	if( myGraphic == 0 )
		DebugStr( "\pFailed to load graphic" );
	else

#endif

	{
		return DrawGraphic( myGraphic, myGraphic->xPos, myGraphic->yPos, memToUse, 0, 0 );
	}
}


GraphicReference _DrawDBGraphicAt( long memToUse, DBID theID, short xPos, short yPos, short theScreen )
{
SegaBitMapPtr myGraphic;

	if( theScreen == kScrollB )
		ClearBackdrop( false );		//mark backdrop dirty, but don't erase

//
// Draw the graphic
//

	SetCurrentDevice( theScreen );		// was kScrollB
	myGraphic = GetDBGraphics( theID );

#ifdef SIMULATOR

	if( myGraphic == 0 )
		DebugStr( "\pFailed to load graphic" );
	else

#endif

	{
		return DrawGraphic( myGraphic, xPos, yPos, memToUse, 0, 0 );
	}
}


SegaBitMapPtr _GetDBGraphics( DBID graphicNum )
{
SegaBitMapPtr	bm;

	bm = (SegaBitMapPtr) DBGetItem ( kBitMapType, graphicNum );
	
	return bm;
}

SegaBitMapPtr _GetDBButtonFrame( DBID buttonNumber, DBID frame )
{
SegaBitMapPtr	bm;

	bm = (SegaBitMapPtr) DBGetItem ( kButtonGraphicType, buttonNumber*16 + frame );
	
	return bm;
}

/* When finished displaying a graphic, call this to dispose its pattern memory and clut */
void _DisposeGraphicReference( GraphicReference ref )
{
	if( ref == 0 )
		return;
		
	/* Free the pattern block if it was allocated w/ pattern manager */
	if ( PATSIZE(ref->patternBlock) > 0 )
	{
		FreePatternBlock( ref->patternBlock );
	}
	
	/* Free the clut if it's not the default */
	if ( ref->clut )
	{
		ReleaseClut( ref->clut );
#ifdef DEBUG
		ref->clut = -1;
#endif
	}
	
	/* Free the GraphicReferenceRec */
	DisposeMemory((Ptr)ref);
}

short _GetGraphicReferenceClut( GraphicReference ref )
{
	if (ref)
		return (ref->clut);
	else
		return 0;		/* no graphic reference, so just return default clut */
}
