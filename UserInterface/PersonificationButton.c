/*
	File:		PersonificationButton.c

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 8/12/94	JOE		new icon stuff
		 <6>	  8/3/94	JOE		update for new pers mgr
		 <5>	  8/3/94	CMY		Make the non-selected button's text gray.
		 <4>	 7/28/94	KON		Fix font colors.
		 <3>	 7/21/94	CMY		The icons draw in gray when not selected. The text draws as a
									sprite so it don't get munged.
		 <2>	 7/20/94	JOE		cleaned up a bit while half asleep
		 <1>	  7/9/94	CMY		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "Errors.h"
#include "SegaText.h"
#include "SegaBM.h"
#include "VDP.h"
#include "SegaIn.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "Animators.h"
#include "MungeCode.h"
#include "SegaCluts.h"
#include "PersonificationButton.h"
#include "UsrConfg.h"


static void RenderDITLPersonButton( DITLItem *theItem );
static void SelectDITLPersonButton( DITLItem *theItem );
static void DeselectDITLPersonButton( DITLItem *theItem );
static Boolean ClickDITLPersonButton( DITLItem *theItem, short controllerRead );

static Boolean DoDITLPersonButton( short message, DITLItem *theItem, short keyPress )
{
Boolean	done;
PersonButtonPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(PersonButtonPriv) );
			if ( state )
			{
				theItem->refCon = (long) state;
				state->textSprite = 0;
				state->iconReference = 0L;
			}
			break;
			
		case kRenderDITLElement:
			RenderDITLPersonButton( theItem );		
			break;
				
		case kSelectDITLElement:
			SelectDITLPersonButton( theItem );
			break;
			
		case kDeselectDITLElement:
			DeselectDITLPersonButton( theItem );
			break;
			
		case kClickDITLElement:
			done = ClickDITLPersonButton( theItem, keyPress );
			break;
			
		case kCloseDITLElement:
			state = (PersonButtonPriv *) theItem->refCon;
			if ( state )
			{
				if ( state->textSprite )
				{
					DisposeSprite ( state->textSprite );
				}
				
				if ( state->iconReference )
				{
					DisposeIconReference ( state->iconReference );	
				}
				if ( state->textSpriteState )
				{
					DisposeTextStateReference( state->textSpriteState );
				}
				DisposeMemory (state );
			}
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLPersonButton" );
			
	}
		
	return done;
}

void RenderDITLPersonButton( DITLItem *theItem )
{
short				pixelWidth;
short				xLeft;
short				xCenter;
PersonButtonDesc	*thePersonButton;
segaCharRect		textRect;
short				cellWidth;
short				oldFont;
PersonButtonPriv	*state;
IconReference		userIcon;
short				height;
short				adjheight;
userIdentification	user;

	state = (PersonButtonPriv *) theItem->refCon;
	if ( !state )
		return;

	/* Get the button data */
	thePersonButton = (PersonButtonDesc *)theItem->objectData;		

	/* Draw the user icon */
	SetCurrentDevice( kScrollA );
	FillInUserIdentification( thePersonButton->userID, &user );
	state->iconReference = userIcon = DrawPlayerIcon( &user, theItem->xPos, theItem->yPos, kGrayIconFlag );
			
	/* Draw the user's name */
	oldFont = GetCurFont ();
	SetCurFont ( kMain2BitFont );
	
	pixelWidth = MeasureSegaText( thePersonButton->handle );
		
	xCenter = (kPersonificationIconWidth >> 1) + theItem->xPos;
	
	xLeft = xCenter - (pixelWidth >> 1);
			
	cellWidth = (pixelWidth+7) >> 3;
	cellWidth = (cellWidth+3) & ~3;			// Due to our fuqdup sprite allocator

	textRect.left = (xLeft>>3);
	textRect.right = textRect.left + cellWidth;
	textRect.top = (theItem->yPos + kPersonificationIconHeight + 2) >> 3;
	height = GetCurFontHeight();
	adjheight = height >> 3;
	if ( (adjheight << 3) < height )
		adjheight++;
	textRect.bottom = textRect.top + adjheight;
		
	SetCurrentDevice( kScrollA );
#if 0
	state->textSprite = LinearizeScreenArea( &textRect, 0 );
	SetupTextGDevice( &textRect, state->textPattern );
#else
	state->textSprite = CreateTextSprite( &textRect, 0 );
#endif	
	SetFontColors( 0, kGray3Color, kGray4Color, 0 );

	DrawClippedSegaText( 0, 0, 128, thePersonButton->handle );
	state->textSpriteState = GetTextStateReference();
	MoveSprite( state->textSprite, xLeft+128, theItem->yPos + kPersonificationIconHeight + 130);
	DrawSprite( state->textSprite );

	SetCurFont ( oldFont );
	SetCurrentDevice ( kScrollA );

	VDPIdle();
}

void SelectDITLPersonButton( DITLItem *theItem )
{
PersonButtonPriv	*state;
IconReference		iconRef;
PersonButtonDesc	*thePersonButton;
userIdentification	user;
	
	thePersonButton = (PersonButtonDesc *)theItem->objectData;
	state = (PersonButtonPriv *) theItem->refCon;	
	if ( !state )
		return;
	
	iconRef = state->iconReference;
	FillInUserIdentification( ((PersonButtonDesc *)theItem->objectData)->userID, &user );
	state->iconReference = DrawPlayerIcon( &user, theItem->xPos, theItem->yPos, 0 );
	RestoreTextState( state->textSpriteState );
	SetFontColors( 0, kDarkYellowColor, kYellowColor, 0 );
	DrawClippedSegaText( 0, 0, 128, thePersonButton->handle );
	DisposeIconReference( iconRef );
}

void DeselectDITLPersonButton( DITLItem *theItem )
{
PersonButtonPriv *state;
IconReference		iconRef;
PersonButtonDesc	*thePersonButton;
userIdentification	user;
	
	thePersonButton = (PersonButtonDesc *)theItem->objectData;
	state = (PersonButtonPriv *) theItem->refCon;
	if ( !state )
		return;
	
	iconRef = state->iconReference;
	FillInUserIdentification( ((PersonButtonDesc *)theItem->objectData)->userID, &user );
	state->iconReference = DrawPlayerIcon( &user, theItem->xPos, theItem->yPos, kGrayIconFlag );
	RestoreTextState( state->textSpriteState );
	SetFontColors( 0, kGray3Color, kGray4Color, 0 );
	DrawClippedSegaText( 0, 0, 128, thePersonButton->handle );
	DisposeIconReference( iconRef );
}

Boolean ClickDITLPersonButton( DITLItem *theItem, short controllerRead )
{
// signal done if any button was pressed
	if( controllerRead & kButtonPressed )
		return true;	//signal we are done
	else
		return false;
}

doDITLItemProc GetPersonButtonPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLPersonButton + 2));

	size = (*(long *) ((Ptr) GetPersonButtonPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for PersonButton proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) DoDITLPersonButton;
#else
		CheckCodeForJTRefs((void *)DoDITLPersonButton, size);
		SegaBlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return (doDITLItemProc)data;
}
