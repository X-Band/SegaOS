/*
	File:		TextButtGraphics.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<18>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<17>	  8/8/94	CMY		Always draw the buttons into ScrollA, not whatever is the
									current screen!
		<16>	 7/30/94	CMY		Added code to control caching of the decompressor dictionary.
		<15>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		<14>	 7/27/94	CMY		Cache the master dictionary when animating a button.
		<13>	 7/25/94	CMY		Stop cursor animation when dialogs are up.
		<12>	 7/22/94	JOE		Animations can now have attached sound effects.
		<11>	 7/21/94	CMY		Draw the text in a sprite so it don't get squarshed by the bean.
		<10>	 7/20/94	CMY		The selected bean now animates.
		 <9>	 7/19/94	JOE		autosize cell height for text
		 <8>	 7/13/94	ADS		Cleanup code copy;  Cleanup empty string case
		 <7>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <6>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <5>	 6/28/94	CMY		Updated to use palette manager.
		 <4>	 6/27/94	JOE		I suck, try again.
		 <3>	 6/27/94	JOE		in RenderDITLTextButton, init xLeft before using it.
		 <2>	 6/21/94	KON		Check for nil text string and skip all text code if there's not
									text to draw.
		 <1>	 6/20/94	KON		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "Errors.h"
#include "SegaText.h"
#include "VDP.h"
#include "SegaIn.h"
#include "DBTypes.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "Animators.h"
#include "MungeCode.h"
#include "GraphicsDB.h"
#include "TextButtGraphics.h"
#include "SegaSound.h"

static void RenderDITLTextButton( DITLItem *theItem );
static void SelectDITLTextButton( DITLItem *theItem );
static void DeselectDITLTextButton( DITLItem *theItem );
static Boolean ClickDITLTextButton( DITLItem *theItem, short controllerRead );

static Boolean DoDITLTextButton( short message, DITLItem *theItem, short keyPress )
{
Boolean	done;
TextButtGraphicsPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(TextButtGraphicsPriv) );
			if ( state )
			{
				theItem->refCon = (long) state;
				state->textSprite = 0;
				state->buttReference = nil;
				state->animatorProc = nil;
				state->buttonID = 0;
				state->currentFrame = 0;
				state->toKeepDictCached = nil;
			}
			break;
			
		case kRenderDITLElement:
			RenderDITLTextButton( theItem );		
			break;
				
		case kSelectDITLElement:
			SelectDITLTextButton( theItem );
			break;
			
		case kDeselectDITLElement:
			DeselectDITLTextButton( theItem );
			break;
			
		case kClickDITLElement:
			done = ClickDITLTextButton( theItem, keyPress );
			break;
			
		case kCloseDITLElement:
			state = (TextButtGraphicsPriv *) theItem->refCon;
			if ( state )
			{
				if (state->animatorProc)
				{
					RemoveTimeRequest( state->animatorProc );
					state->animatorProc = nil;
				}

				if ( state->textSprite )
				{
					DisposeSprite ( state->textSprite );
				}
				
				if ( state->buttReference )
				{
					DisposeGraphicReference ( state->buttReference );	
				}
				
				if ( state->toKeepDictCached )
				{
					Decompressor dec = CreateDecompressor(
						GetDBButtonFrame( state->buttonID, kAnimationFrames ), 0, 0 );

					ReleaseDecompressorCache( dec, state->toKeepDictCached );
					DisposeDecompressor( dec );
				}
				
				DisposeMemory (state );
			}
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLTextButton" );
			
	}
		
	return done;
}

void RenderDITLTextButton( DITLItem *theItem )
{
short			pixelWidth;
short			clipWidth;
short			xCenter;
short			xLeft;
short			xCharOffset;
short			yCharOffset;
TextButtonDesc	*theTextButt;
segaCharRect	textRect;
short			cellWidth;
short			oldFont;
TextButtGraphicsPriv *	state;
AnimationRef	animation;
short			height;
short			adjheight;
char			*frameSequence;

	state = (TextButtGraphicsPriv *) theItem->refCon;
	if ( !state )
		return;
//
// Draw the bitmap
//
	
	theTextButt = (TextButtonDesc *)theItem->objectData;

	if( theTextButt->animationData.bitmap == 0 )
		return;
	
	state->buttonID = theTextButt->buttonID;
	state->currentFrame = 0;
	state->x = theTextButt->animationData.xPos/8;
	state->y = theTextButt->animationData.yPos/8;
	frameSequence = DBGetItem( kButtonSequenceType, theTextButt->buttonID );
	if (frameSequence)
	{
		state->frameCount = *(short *)frameSequence;
		state->frameSequence = frameSequence+sizeof(short);
	}
	else
	{
		state->frameSequence = nil;
	}
	
	state->fxSequence = DBGetItem( kButtonSequenceFXType, theTextButt->buttonID );
	
	SetCurrentDevice( kScrollA );
	state->buttReference = DrawGraphic( theTextButt->animationData.bitmap, theTextButt->animationData.xPos/8, theTextButt->animationData.yPos/8, 0, 0, 0 );
//
// Figure out how to draw the text using the info in the refCon
//
	
	clipWidth = theTextButt->pixelWidth;

	state->textSprite = 0;
	if( theTextButt->myCString[0] != 0 )
	{

		oldFont = GetCurFont ();
		SetCurFont ( ((TextButtonDesc *) theItem->objectData)->font );

		pixelWidth = MeasureSegaText( theTextButt->myCString );

		if ( theTextButt->textXCenter == 0 )
		{
			xLeft = theTextButt->animationData.bitmap->width << 2;
		}
		else
		{
			xLeft = theTextButt->textXCenter;
		}
			
		xLeft += theItem->xPos;
		xLeft -= (pixelWidth >> 1);

		cellWidth = ( (pixelWidth+7) + (xLeft & 7) ) >> 3;
		cellWidth = (cellWidth+3) & ~3;

		textRect.left = xLeft>>3;
		textRect.right = textRect.left + cellWidth;
		
		if ( theTextButt->textYTop == 0 )
		{
			textRect.top = theTextButt->animationData.bitmap->height;
		}
		else
		{
			textRect.top = ( theTextButt->textYTop + 4 ) >> 3;
		}
			
		textRect.top += (theItem->yPos >> 3);
		height = GetCurFontHeight();
		adjheight = height >> 3;
		if ( (adjheight << 3) < height )
			adjheight++;
		textRect.bottom = textRect.top + adjheight;
		
		SetCurrentDevice( kScrollA );
		state->textSprite = CreateTextSprite( &textRect, 0 );
		
		if (state->textSprite)
		{
			SetFontColors( theTextButt->color0, theTextButt->color1, theTextButt->color2, theTextButt->color3 );
			DrawClippedSegaText( 0, 0, clipWidth, theTextButt->myCString );
			MoveSprite( state->textSprite, xLeft+128, (textRect.top<<3) + 128);
			DrawSprite( state->textSprite );
		}
		
		SetCurFont ( oldFont );
	}

	SetCurrentDevice ( kScrollA );

	VDPIdle();
}

static long TextButtGraphicsAnimatorProc(long time, long data)
{
	short	iii;
	TextButtGraphicsPriv *state = (TextButtGraphicsPriv *)data;
	SegaBitMap	*image;
	GraphicReference ref;
	short saveDevice = GetCurrentDevice();
	
	if (GetAnimationSuspendLevel() >= kDialogAnimationPriority)
		return time+10;

	if ( state->fxSequence )
		for (iii = 0; iii != (state->fxSequence)->count; iii++ )
			if ( (state->fxSequence)->FX[iii].frame == (state->currentFrame) )
				PlayDBFX( (state->fxSequence)->FX[iii].theFXID, 0, 0 );
	
	image = GetDBButtonFrame( state->buttonID, kAnimationFrames +
		state->frameSequence[state->currentFrame] );
	
	ref = state->buttReference;
	SetCurrentDevice( kScrollA );
	state->buttReference = DrawGraphic( image, state->x, state->y, 0, 0, 0 );
	DisposeGraphicReference(ref);
	SetCurrentDevice( saveDevice );
	
	state->currentFrame = (state->currentFrame+1) % state->frameCount;
	return time+4;
}

void SelectDITLTextButton( DITLItem *theItem )
{
	TextButtGraphicsPriv *state = (TextButtGraphicsPriv *) theItem->refCon;

	if (!state)
		return;
		
	if (state->frameSequence)
	{
		Decompressor dec = CreateDecompressor(
			GetDBButtonFrame( state->buttonID, kAnimationFrames ), 0, 0 );
			
		state->toKeepDictCached = GetDecompressorCache( dec );
		DisposeDecompressor( dec );
		
		state->currentFrame = 0;
		state->animatorProc = AddTimeRequest(TextButtGraphicsAnimatorProc, (long)state);
	}
}

void DeselectDITLTextButton( DITLItem *theItem )
{
	TextButtGraphicsPriv *state = (TextButtGraphicsPriv *) theItem->refCon;

	if (state->animatorProc)
	{
		Decompressor dec = CreateDecompressor(
			GetDBButtonFrame( state->buttonID, kAnimationFrames ), 0, 0 );

		ReleaseDecompressorCache( dec, state->toKeepDictCached );
		DisposeDecompressor( dec );
		state->toKeepDictCached = nil;
		RemoveTimeRequest( state->animatorProc );
		state->animatorProc = nil;
	}
	state->currentFrame = 0;
	TextButtGraphicsAnimatorProc(0, (long)state);
}

Boolean ClickDITLTextButton( DITLItem *theItem, short controllerRead )
{
// signal done if any button was pressed
	if( controllerRead & kButtonPressed )
		return true;	//signal we are done
	else
		return false;
}

doDITLItemProc GetTextButtonGraphicsPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLTextButton + 2));

	size = (*(long *) ((Ptr) GetTextButtonGraphicsPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Button proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) DoDITLTextButton;
#else
		CheckCodeForJTRefs((void *)DoDITLTextButton, size);
		SegaBlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return (doDITLItemProc)data;
}
