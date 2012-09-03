/*
	File:		TextButt.c

	Contains:	xxx put contents here xxx

	Written by:	Konstantin Othmer

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<27>	 8/26/94	CMY		Make the Sine chortle less jerky.
		<26>	 8/25/94	CMY		Fixed the motion of the Challenge button to make it faster.
		<25>	 8/24/94	CMY		Added accordian chortle proc.
		<24>	 8/22/94	CMY		Don't cache sprites for deselected buttons.
		<23>	 8/18/94	CMY		Multiple chortle procs.
		<22>	 8/17/94	CMY		Added support for chortling text.
		<21>	  8/7/94	CMY		We are now able to place text at any y position, not multiples
									of 8.
		<20>	 7/30/94	CMY		Added code to control caching of the decompressor dictionary.
		<19>	 7/25/94	CMY		Stop cursor animation when dialogs are up.
		<18>	 7/22/94	JOE		Button animations can now have sound effects attached.
		<17>	 7/21/94	CMY		Button animations are now driven by kButtonSequence database
									types.
		<16>	 7/19/94	CMY		TextButts now draw buttonGraphics, not SEGA graphics.
		<15>	 7/19/94	JOE		autosize cell height for text
		<14>	 7/13/94	ADS		Cleanup code copy;  Cleanup empty string case
		<13>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		<12>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<11>	 6/29/94	CMY		Made it Think C debuggable in the Stimulator.
		<10>	 6/27/94	JOE		I suck, try again.
		 <9>	 6/27/94	JOE		in RenderDITLTextButton, init xLeft before using it.
		 <8>	 6/19/94	KON		If animation.bitmap is zero, bail.
		 <7>	 5/31/94	KON		Fix text centering.
		 <6>	 5/28/94	SAH		Added the ROMDB conditional for the rom databse build.
		 <5>	 5/28/94	CMY		Added calls to CheckCodeForJTRefs when adding procs to DB.
		 <4>	 5/28/94	SAH		Really copy the proc rather than just the jump table.
		 <3>	 5/27/94	KON		Made DITL item positions pixel based instead of character based.
		 <2>	 5/27/94	KON		Removed unused stack variable. Yeah!
		 <5>	 5/25/94	HEC		new memory manager
		 <4>	 5/23/94	SAH		Added text x and y offsets. Made all TextButton fields bytes.
		 <3>	 5/21/94	SAH		Updated to make button bitmaps animations that index though
									frames at a defined rate while selected.
		 <2>	 5/20/94	SAH		Removed text selection code ( in favour of using the cursor).
									Made only linearize the pattern memory it needed to (centering
									seems a little weird through). Added font and extra color
									parameters as well as a palette id to the TextDesc record.
		 <1>	 5/18/94	KON		first checked in

	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "TextButt.h"
#include "Errors.h"
#include "SegaText.h"
#include "DBTypes.h"
#include "VDP.h"
#include "SegaIn.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "Animators.h"
#include "MungeCode.h"
#include "GraphicsDB.h"
#include "Time.h"
#include "SegaSound.h"
#include "Decompress.h"
#include "DBConstants.h"
#include "BoxSer.h"
#include "Math.h"
#include "Utils.h"

static void RenderDITLTextButton( DITLItem *theItem );
static void SelectDITLTextButton( DITLItem *theItem );
static void DeselectDITLTextButton( DITLItem *theItem );
static Boolean ClickDITLTextButton( DITLItem *theItem, short controllerRead );

#define kInitialTheta -250

static Boolean DoDITLTextButton( short message, DITLItem *theItem, short keyPress )
{
Boolean	done;
TextButtPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(TextButtPriv) );
			if ( state )
			{
				long chortleType;
				
				theItem->refCon = (long) state;
				state->textPattern = 0;
				state->buttonSprite = 0L;
				state->currentFrame = 0;
				state->buttonID = 0;
				state->animatorProc = nil;
				state->toKeepDictCached = nil;
				
				DBGetConstant( kChortleTypeConst, &chortleType );
				state->chortleProc = DBGetItem( kChortleProcType, chortleType );
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
			state = (TextButtPriv *) theItem->refCon;
			if ( state )
			{
				if ( state->textPattern )
				{
					FreePatternBlock ( state->textPattern );
				}
				
				if ( state->animatorProc)
				{
					RemoveTimeRequest( state->animatorProc );
					state->animatorProc = nil;
				}
				
				if ( state->buttonSprite )
				{
					DisposeSprite ( state->buttonSprite );	
				}
				
				if ( state->toKeepDictCached )
				{
					Decompressor dec = CreateDecompressor(
						GetDBButtonFrame( state->buttonID, kAnimationFrames ), 0, 0 );

					ReleaseDecompressorCache( dec, state->toKeepDictCached );
					DisposeDecompressor( dec );
				}

				if ( state->characterSprites )
				{
					short index;
					
					for (index=0; index<state->spriteCount; ++index)
						DisposeSprite( state->characterSprites[index] );
					
					DisposeMemory( state->characterSprites );
					state->characterSprites = nil;
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
TextButtPriv *	state;
AnimationRef	animation;
short			height;
short			adjheight;
char 			*frameSequence;
short			finalY;

	state = (TextButtPriv *) theItem->refCon;
	if ( !state )
		return;
//
// Draw the bitmap
//
	
	theTextButt = (TextButtonDesc *)theItem->objectData;

	if( theTextButt->animationData.bitmap == 0 )
		return;
	
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
	
	state->buttonID = theTextButt->buttonID;
	state->buttonSprite = CreateSprite( theTextButt->animationData.bitmap );
	MoveSprite( state->buttonSprite, theItem->xPos+128, theItem->yPos+128 );
	DrawSprite( state->buttonSprite );		
	SetCurrentDevice( kScrollA );
		
//
// Figure out how to draw the text using the info in the refCon
//
	clipWidth = theTextButt->pixelWidth;

	state->textPattern = 0;
	
	/* Does the button have a title? If so, draw it */
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
	
		textRect.left = xLeft>>3;
		textRect.right = textRect.left + cellWidth;
		
		if ( theTextButt->textYTop == 0 )
		{
			finalY = theTextButt->animationData.bitmap->height << 3;
		}
		else
		{
			finalY = theTextButt->textYTop;
		}
		
		finalY += theItem->yPos;
		textRect.top = finalY >> 3;
		textRect.bottom = (finalY + GetCurFontHeight() + 7) >> 3;
		
		SetCurrentDevice( kScrollA );
		state->textPattern = LinearizeScreenArea( &textRect, 0 );
		if (state->textPattern)	{
			SetupTextGDevice( &textRect, state->textPattern );
		
			state->textRect = textRect;
			state->xLeft = xLeft;
			state->finalY = finalY & 7;
			state->yTop = textRect.top << 3;
			state->characterCount = GetStringLength( theTextButt->myCString );
			state->spriteCount = 0;
			state->theString = theTextButt->myCString;
			state->characterSprites = nil;
			state->theTextButt = theTextButt;
			xLeft &= 0x7;
				
			SetFontColors( theTextButt->color0, theTextButt->color1, theTextButt->color2, theTextButt->color3 );
			DrawClippedSegaText( xLeft, finalY & 7, clipWidth, theTextButt->myCString );
		}
		else
		{
			WARNING_MESG("couldn't get text pattern");
			state->characterCount = 0;
		}
		SetCurFont ( oldFont );
	}
	else
	{
		/* No title for the button. Clear out the character animation fields */
		state->characterCount = 0;
		state->characterSprites = nil;
	}
	SetCurrentDevice ( kScrollA );

	VDPIdle();
}

static long TextButtAnimatorProc(long time, long data)
{
	short			iii;
	TextButtPriv	*state = (TextButtPriv *)data;
	short			delay = 1;
	
	if (GetAnimationSuspendLevel() >= kDialogAnimationPriority)
		return time+10;
	
	if ( state->textStep == false || (state->buttonID == 1) )
	{
		if ( state->fxSequence )
			for (iii = 0; iii != (state->fxSequence)->count; iii++ )
				if ( (state->fxSequence)->FX[iii].frame == (state->currentFrame) )
					PlayDBFX( (state->fxSequence)->FX[iii].theFXID, 0, 0 );
		
		SetSpriteImage( state->buttonSprite, GetDBButtonFrame( state->buttonID,
			kAnimationFrames + state->frameSequence[state->currentFrame] ));
		DrawSprite( state->buttonSprite );
		state->currentFrame = (state->currentFrame+1) % state->frameCount;
	}
	
	/* if we aren't animating text, bail here */
	if (state->characterCount == 0 ||				/* No characters to animate */
		((state->theTextButt->flags & 1) == 0) ||	/* or animation not enabled for this button */
		(state->chortleProc == nil) )				/* or no chortle proc */
	{
		state->textStep = false;
		return (state->buttonID == 1) ? time+2 : time+4;
	}
	state->textStep = !state->textStep;
		
	if ( state->characterSprites == nil )
	{
		state->characterSprites = (SpriteRef *)NewMemory( kTemp, state->characterCount * sizeof(SpriteRef) );
	}
	if ( state->spriteCount == 0 )
		state->xPos = state->xLeft;

	if (state->spriteCount == state->characterCount && state->theta == kInitialTheta)
	{
		/* Erase the existing text */
		SetupTextGDevice( &state->textRect, state->textPattern );
		EraseTextGDevice( 0 );
	}
		
	if (state->spriteCount < state->characterCount)
	{
		segaCharRect charBox;
		SpriteRef sprite;
		short theChar = ((short)state->theTextButt->myCString[state->spriteCount]) << 8;
		short x = 0, y = 0;
		short width;
		
		SetCurFont ( state->theTextButt->font );
		width = GetCharWidth( theChar >> 8 );
		
		charBox.top = charBox.left = 0;
		charBox.bottom = (GetCurFontHeight() + 7) >> 3;
		charBox.right = 2;
		
		sprite = CreateTextSprite( &charBox, 0 );
		SetFontColors( state->theTextButt->color0, state->theTextButt->color1,
			state->theTextButt->color2, state->theTextButt->color3 );
		DrawSegaString(&x, &y, (char *)&theChar);
		MoveSprite(sprite, state->xPos+128, state->yTop+128);
		DrawSprite( sprite );
		
		state->xPos += width;
		state->characterSprites[ state->spriteCount ] = sprite;
		++state->spriteCount;
	}
	else
	{
		ASSERT( state->chortleProc );
		(*state->chortleProc)( state );
	}
	
	return time+2;
}

void SelectDITLTextButton( DITLItem *theItem )
{
TextButtPriv *	state;
TextButtonDesc *theTextButt = (TextButtonDesc *)theItem->objectData;

	state = (TextButtPriv *) theItem->refCon;
	if ( !state )
		return;
		
	if (state->frameSequence)
	{
		Decompressor dec = CreateDecompressor(
			GetDBButtonFrame( state->buttonID, kAnimationFrames ), 0, 0 );
			
		state->toKeepDictCached = GetDecompressorCache( dec );
		DisposeDecompressor( dec );

		state->currentFrame = 0;
		state->theta = kInitialTheta;
		state->animatorProc = AddTimeRequest(TextButtAnimatorProc, (long)state);
	}
}

void DeselectDITLTextButton( DITLItem *theItem )
{
TextButtPriv *	state;
short 			charCount;
TextButtonDesc	*theTextButt = (TextButtonDesc *)theItem->objectData;

	state = (TextButtPriv *) theItem->refCon;
	if ( !state )
		return;
	
	if (state->animatorProc)
	{
		Decompressor dec = CreateDecompressor(
			GetDBButtonFrame( state->buttonID, kAnimationFrames ), 0, 0 );

		ReleaseDecompressorCache( dec, state->toKeepDictCached );
		DisposeDecompressor( dec );
		state->toKeepDictCached = nil;

		RemoveTimeRequest( state->animatorProc );
		state->animatorProc = nil;
		
		if ( state->characterCount )
		{
			state->theta = kInitialTheta;
		}
	}
	state->currentFrame = 0;
	state->textStep = false;
	TextButtAnimatorProc(0, (long)state);
	
	if ( state->spriteCount )
	{
		short index;
		
		if ( state->spriteCount == state->characterCount )
		{
			SetupTextGDevice( &state->textRect, state->textPattern );
			SetFontColors( theTextButt->color0, theTextButt->color1, theTextButt->color2, theTextButt->color3 );
			DrawClippedSegaText( state->xLeft & 7, state->finalY, theTextButt->pixelWidth, theTextButt->myCString );
		}
		
		for (index=0; index<state->spriteCount; ++index)
			DisposeSprite( state->characterSprites[index] );
			
		state->spriteCount = 0;
	}

}

Boolean ClickDITLTextButton( DITLItem *theItem, short controllerRead )
{
// signal done if any button was pressed
	if( controllerRead & kButtonPressed )
		return true;	//signal we are done
	else
		return false;
}

doDITLItemProc GetTextButtonPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLTextButton + 2));

	size = (*(long *) ((Ptr) GetTextButtonPtr + 2)) - srcFunction;
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

/* Called by the secret key manager to set the chortle proc DBID */
static Boolean SetChortleType( short type )
{
	DBAddConstant( kChortleTypeConst, type );
	
	return false;	// Tell the secret manager to stop
}

Ptr GetSecretChortleProc( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) SetChortleType + 2));

	size = (*(long *) ((Ptr) GetSecretChortleProc + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Secret Chortle proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) SetChortleType;
#else
		CheckCodeForJTRefs((void *)SetChortleType, size);
		SegaBlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}

/* Chortles the text as a sine wave */
#define kAngleIncrement 32
void SineChortle( TextButtPriv *state);
void SineChortle( TextButtPriv *state)
{
	short theta = state->theta;
	short index;
	short xPos = state->xLeft;
	char *title = state->theString;
	
	state->theta = state->theta+kAngleIncrement;
	if (state->theta > 255)
		state->theta -= 256;
	
	for (index=0; index<state->characterCount; ++index)
	{
		short theChar = title[index]<<8;
		short offset = (theta >= 0) ? (Cosine(theta) >> 6) : 0;
		
		MoveSprite( state->characterSprites[index], xPos+128, state->yTop+128+offset );
		DrawSprite( state->characterSprites[index] );
		xPos += GetCharWidth( title[index] );
		theta = theta+kAngleIncrement;
		if (theta > 255)
			theta -= 256;
	}
}

Ptr GetSineChortleProc( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) SineChortle + 2));

	size = (*(long *) ((Ptr) GetSineChortleProc + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for Sine chortle proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) SineChortle;
#else
		CheckCodeForJTRefs((void *)SineChortle, size);
		SegaBlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}

/* Chortles the text with a random vertical offset */
void PalsyChortle( TextButtPriv *state);
void PalsyChortle( TextButtPriv *state)
{
	short index;
	short xPos = state->xLeft;
	char *title = state->theString;
	
	for (index=0; index<state->characterCount; ++index)
	{
		short theChar = title[index]<<8;
		short offset = state->theta == kInitialTheta ? 0 : (RandomShort() & 0x7) - 3;
		
		MoveSprite( state->characterSprites[index], xPos+128, state->yTop+128+offset );
		DrawSprite( state->characterSprites[index] );
		xPos += GetCharWidth( title[index] );
	}
	
	state->theta = 0;
}

static Ptr GetPalsyChortleProc