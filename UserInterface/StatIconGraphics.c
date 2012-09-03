/*
	File:		StatIconGraphics.c

	Contains:	Static Icon screen layout item stuff

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 8/20/94	KON		DrawGraphic now takes a clutID.
		 <5>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		 <4>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <3>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <2>	 6/29/94	JOE		remove stdio.h include
		 <1>	 6/29/94	JOE		first checked in
	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "StatIconGraphics.h"
#include "Errors.h"
#include "SegaText.h"
#include "VDP.h"
#include "SegaIn.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "MungeCode.h"

static void RenderDITLStatIconGraphics( DITLItem *theItem );


static Boolean DoDITLStatIconGraphics( short message, DITLItem *theItem, short keyPress )
{
Boolean		 		   done;
StatIconGraphicsPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(StatIconGraphicsPriv) );
			if ( state )
			{
				theItem->refCon = (long) state;
				state->iconReference = nil;
			}
			break;
			
		case kRenderDITLElement:
			RenderDITLStatIconGraphics( theItem );		
			break;
				
		case kSelectDITLElement:
			break;
			
		case kDeselectDITLElement:
			break;
			
		case kClickDITLElement:
			break;
			
		case kCloseDITLElement:
			state = (StatIconGraphicsPriv *) theItem->refCon;
			if ( state )
			{
				if ( state->iconReference )
				{
					DisposeGraphicReference ( state->iconReference );
				}
								
				DisposeMemory (state );
			}
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLStatIconGraphics" );
			
	}
		
	return done;
}

void RenderDITLStatIconGraphics( DITLItem *theItem )
{
short					pixelWidth;
StatIconGraphicsDesc	*theStatIconGraphic;
segaCharRect			textRect;
short					cellWidth;
short					oldFont;
StatIconGraphicsPriv 	*state;
short					xPos;
short					yPos;

	state = (StatIconGraphicsPriv *) theItem->refCon;
	if ( !state )
		return;
	
	theStatIconGraphic = (StatIconGraphicsDesc *)theItem->objectData;
	
	SetCurrentDevice( kScrollA );

	if( theStatIconGraphic->animationData.bitmap == 0 )
		return;
	
	theStatIconGraphic->animationData.xPos = theItem->xPos;
	theStatIconGraphic->animationData.yPos = theItem->yPos;
		
	SetCurrentDevice( kScrollA );
	
	state->iconReference = DrawGraphic( theStatIconGraphic->animationData.bitmap, 
										theStatIconGraphic->animationData.xPos/8, 
										theStatIconGraphic->animationData.yPos/8, 
										0, 0, 0 );

	SetCurrentDevice( kScrollA );

	VDPIdle();
}


doDITLItemProc GetStatIconGraphicsItemPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLStatIconGraphics + 2));

	size = (*(long *) ((Ptr) GetStatIconGraphicsItemPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for StatIconGraphics proc" );
	
	if ( data )
		{
#ifndef SIMULATOR
		CheckCodeForJTRefs((void *)DoDITLStatIconGraphics, size);
#endif
		SegaBlockMove ( (Ptr) srcFunction, data, size );
		}
	
	return (doDITLItemProc)data;
}
