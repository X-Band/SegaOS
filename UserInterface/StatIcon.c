/*
	File:		StatIcon.c

	Contains:	Static Icon (animated) screen layout item stuff

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	  7/9/94	CMY		Made it Think C debuggable.
		 <3>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <2>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <1>	 6/30/94	JOE		first checked in
	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "StatIcon.h"
#include "Errors.h"
#include "SegaText.h"
#include "VDP.h"
#include "SegaIn.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "MungeCode.h"

static void RenderDITLStatIcon( DITLItem *theItem );


static Boolean DoDITLStatIcon( short message, DITLItem *theItem, short keyPress )
{
Boolean		   done;
StatIconPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(StatIconPriv) );
			if ( state )
			{
				theItem->refCon = (long) state;
				state->animation = nil;
			}
			break;
			
		case kRenderDITLElement:
			RenderDITLStatIcon( theItem );		
			break;
				
		case kSelectDITLElement:
			break;
			
		case kDeselectDITLElement:
			break;
			
		case kClickDITLElement:
			done = (keyPress & kButtonPressed) != 0;
			break;
			
		case kCloseDITLElement:
			state = (StatIconPriv *) theItem->refCon;
			if ( state )
			{
				if ( state->animation )
				{
					DisposeAnimation ( state->animation );
				}
								
				DisposeMemory (state );
			}
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLStatIcon" );
			
	}
		
	return done;
}

void RenderDITLStatIcon( DITLItem *theItem )
{
StatIconDesc			*theStatIcon;
short					oldFont;
StatIconPriv 			*state;
short					xPos;
short					yPos;
AnimationRef			animation;

	state = (StatIconPriv *) theItem->refCon;
	if ( !state )
		return;
	
	theStatIcon = (StatIconDesc *)theItem->objectData;
	
	SetCurrentDevice( kScrollA );

	if( theStatIcon->animationData.bitmap == 0 )
		return;
	
	theStatIcon->animationData.xPos = theItem->xPos;
	theStatIcon->animationData.yPos = theItem->yPos;
		
	SetCurrentDevice( kScrollA );
	
	animation = SpawnAnimation ( kFrameAnimate, (long) &theStatIcon->animationData );
	if ( animation )
	{
		state->animation = animation;
		SetCurrentDevice ( kScrollA );
		DrawAnimationFrame ( animation );
	}

	SetCurrentDevice( kScrollA );

	VDPIdle();
}


doDITLItemProc GetStatIconItemPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLStatIcon + 2));

	size = (*(long *) ((Ptr) GetStatIconItemPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for StatIcon proc" );
	
	if ( data )
		{
#ifndef SIMULATOR
		CheckCodeForJTRefs((void *)DoDITLStatIcon, size);
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) DoDITLStatIcon;
#endif
		SegaBlockMove ( (Ptr) srcFunction, data, size );
		}
	
	return (doDITLItemProc)data;
}
