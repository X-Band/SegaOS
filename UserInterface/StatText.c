/*
	File:		StatText.c

	Contains:	Static Text screen layout item stuff

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 7/22/94	JOE		Be smarter about calculating pixelwidth of string; take xPos
									offset into account.  If pixelWidth in DESC != 0, use that
									rather than calculated string width.
		 <8>	 7/22/94	JOE		fixed bug in subcell positioning
		 <7>	 7/20/94	JOE		add subcell text positioning
		 <6>	 7/19/94	JOE		autosize cell height for text
		 <5>	 7/17/94	JOE		don't try to render 0-width strings
		 <4>	  7/9/94	CMY		Center the static text if the xCenter value is 1.
		 <3>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <2>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <1>	 6/28/94	JOE		first checked in
	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "StatText.h"
#include "Errors.h"
#include "SegaText.h"
#include "VDP.h"
#include "SegaIn.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "MungeCode.h"


static void RenderDITLStatText( DITLItem *theItem );


static Boolean DoDITLStatText( short message, DITLItem *theItem, short keyPress )
{
Boolean		 done;
StatTextPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(StatTextPriv) );
			if ( state )
			{
				theItem->refCon = (long) state;
				state->textPattern = 0;
			}
			break;
			
		case kRenderDITLElement:
			RenderDITLStatText( theItem );		
			break;
				
		case kSelectDITLElement:
			break;
			
		case kDeselectDITLElement:
			break;
			
		case kClickDITLElement:
			break;
			
		case kCloseDITLElement:
			state = (StatTextPriv *) theItem->refCon;
			if ( state )
			{
				if ( state->textPattern )
				{
					FreePatternBlock ( state->textPattern );
				}
				
				DisposeMemory (state );
			}
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLStatText" );
			
	}
		
	return done;
}

void RenderDITLStatText( DITLItem *theItem )
{
short			pixelWidth;
StatTextDesc	*theStatText;
segaCharRect	textRect;
short			cellWidth;
short			oldFont;
StatTextPriv 	*state;
short			xPos;
short			yPos;
short			height;
short			adjheight;

	state = (StatTextPriv *) theItem->refCon;
	if ( !state )
		return;
	
	theStatText = (StatTextDesc *)theItem->objectData;
	
	SetCurrentDevice( kScrollA );
		
//
// Figure out how to draw the text using the info in the refCon
//
	state->textPattern = 0;
	
	oldFont = GetCurFont ();
	SetCurFont ( ((StatTextDesc *) theItem->objectData)->font );

	if ( theStatText->pixelWidth == 0 )
		pixelWidth = MeasureSegaText( theStatText->myCString );
		else
 			pixelWidth = theStatText->pixelWidth;
 			
 	pixelWidth += (theStatText->animationData).xPos;

	if ( pixelWidth )						// don't try to render zero-width strings
		{
		cellWidth = (pixelWidth+7) >> 3;
	
		if (theStatText->textXCenter)
		{
			// For centered text, we want an even number of cells
			cellWidth = (cellWidth+1) & ~1;
			textRect.left = (theItem->xPos >> 3) - (cellWidth >> 1);
		}
		else
		{
			textRect.left = (theItem->xPos >> 3);
		}
	
		textRect.right = textRect.left + cellWidth;
			
		textRect.top = (theItem->yPos >> 3);
		height = GetCurFontHeight();
		adjheight = height >> 3;
		if ( (adjheight << 3) < height )
			adjheight++;
		textRect.bottom = textRect.top + adjheight;
	
		SetCurrentDevice( kScrollA );
		state->textPattern = LinearizeScreenArea( &textRect, theStatText->palette );
		SetupTextGDevice( &textRect, state->textPattern );
		
		SetFontColors( theStatText->color0, theStatText->color1, theStatText->color2, theStatText->color3 );

		if (theStatText->textXCenter)
		{
			DrawCenteredClippedSegaText(cellWidth<<2, (theStatText->animationData).yPos, 
												pixelWidth, theStatText->myCString );
		}
		else
		{
			DrawClippedSegaText( (theStatText->animationData).xPos,
								 (theStatText->animationData).yPos, 
								 pixelWidth, theStatText->myCString );
		}

		SetCurrentDevice ( kScrollA );
		}
		
	SetCurFont ( oldFont );
	VDPIdle();
}


doDITLItemProc GetStatTextItemPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLStatText + 2));

	size = (*(long *) ((Ptr) GetStatTextItemPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for StatText proc" );
	
	if ( data )
		{
#ifndef SIMULATOR
		CheckCodeForJTRefs((void *)DoDITLStatText, size);
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) DoDITLStatText;
#endif
		SegaBlockMove ( (Ptr) srcFunction, data, size );
		}
	
	return (doDITLItemProc)data;
}
