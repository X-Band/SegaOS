/*
	File:		SegaIcons.c

	Contains:	xxx put contents here xxx

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<22>	 8/23/94	JBH		(for Joe) removed an old test for custom icons that was causing
									versus screen to use ROM icon instead.
		<21>	 8/22/94	KON		Get the hue shift from the userIdentification when drawing the
									icon, rather than from the current user's personification.
		<20>	 8/21/94	JOE		Add support for alternate hue (aka custom ROM clut)
		<19>	 8/20/94	JOE		PlayerRAMIconExists now returns kDBReservedItemType for no RAM
									icon present
		<18>	 8/20/94	JOE		change Custom Icon logic; always draw a RAM icon if present
		<17>	 8/20/94	KON		DrawGraphic now takes a clutID.
		<16>	 8/16/94	JOE		For GetPlayerIconBitmap() if we can't find a valid RAM or ROM
									bitmap, give 'em the kNewPlayer icon
		<15>	 8/16/94	CMY		If RAM icon was blown away, draw New Player icon using the
									konstant.
		<14>	 8/12/94	JOE		put back in Kon's GetIconBitMap() call
		<13>	 8/12/94	JOE		Reworked for better Custom Icon scheme
		<12>	 8/10/94	KON		Add getIconBitMap call.
		<12>	 8/10/94	KON		Add GetIconBitMap call.
		<11>	  8/3/94	JOE		new icon routines
		<10>	  8/3/94	JOE		Added DrawDBIconWithoutIREF
		 <9>	 7/29/94	KON		Add decompressor options to DrawGraphic.
		 <8>	 7/21/94	CMY		Now we can draw them in grayscale and save ourselves a palette.
		 <7>	 7/18/94	KON		Return IconReference when drawing an icon.
		 <6>	 7/18/94	KON		Fix up the icon drawing code.
		 <5>	 7/18/94	KON		Added drawing and disposing of icons by reference.
		 <4>	 7/10/94	CMY		Start animations when drawing player icons with more than 1
									frame.
		 <3>	  7/5/94	JOE		added NukeRAMDBIcon()
		 <2>	  7/3/94	JOE		revamp for IDSC's and ISEG's
		 <1>	  7/2/94	JOE		first checked in
	To Do:
*/

#include "Sega.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaVDP.h"
#include "Heaps.h"
#include "vdp.h"
#include "SegaOS.h"
#include "Decompress.h"
#include "Errors.h"
#include "Decompressors.h"
#include "MiscDBItemsPriv.h"
#include "DBTypes.h"
#include "SegaIcons.h"
#include "Personification.h"


static IconReference DecodeAndDraw( SegaBitMap *bitmap, DBID theID, short xPos, short yPos, unsigned char hue, Boolean grayFlag );


/*
	Draw the specified player's icon at xPos,yPos.  The icon can be an ISEG (ROM icon) or a
	RAIC (RAM Icon).  If a RAIC exists with the same boxID & userID as the passed in 
	userIdentification *AND* the ROMID is kDBReservedItemType, the RAM icon is drawn.
	Otherwise, the ROM Icon specified in the userIdentification is drawn.
	
	If an unknown ROM ID is specified, ROM Icon 0 (the Unknown Player) is used.  This way, a Pro
	version of the box can have more icons and degrade nicely when talking to an older box.
	
	If an IDSC with the same ID as the ISEG or RAIC is present, SpawnAnimation() is used.
	
	ROM icons can have a user-associated hue shift!
	
	NOTE: The current device should be set before calling this.
*/


IconReference _DrawPlayerIcon( userIdentification *user, short xPos, short yPos, long flags )
{
DBID			ramID;
DBID			prevID;
RAMIconBitMap 	*ramIconBM;
SegaBitMap		*romBM;

	ramIconBM = 0;
	
	ramID = DBGetFirstItemID( kRAMIconType );
	
	if ( ramID != kDBIDErrorValue )
	{
		do 
		{
			ramIconBM = (RAMIconBitMap *)DBGetItem( kRAMIconType, ramID );
			prevID = ramID;
			ramID = DBGetNextItemID( kRAMIconType, ramID );
		} 
		while ( ramIconBM && 
				!( ( ramIconBM->box.region == user->box.region ) && 
				   ( ramIconBM->box.box == user->box.box ) && 
				   ( ramIconBM->userID == user->userID ) ) 
			  );
	}

	if ( ramIconBM )		/* did we find a match? */
	{
		return( DecodeAndDraw( &(ramIconBM->bitmap), prevID, xPos, yPos, 0, (flags & kGrayIconFlag) ) );
	}
	else
	{
		romBM = (SegaBitMap *)DBGetItem( kIconBitMapType, user->ROMIconID );
		if ( romBM )
		{
			return( DecodeAndDraw( romBM, user->ROMIconID, xPos, yPos, user->colorTableID, (flags & kGrayIconFlag) ) );
		}
		else	
		{																/* we don't know this one */
			romBM = (SegaBitMap *)DBGetItem( kIconBitMapType, kNewPlayerIcon );
			return( DecodeAndDraw( romBM, 0, xPos, yPos, 0, (flags & kGrayIconFlag) ) );
		}
	}
}


/*
	If there is a RAM icon for this user/opponent, delete it (and an associated IDSC, if there is one)
 */
 
void _NukePlayerRAMIcon( userIdentification *user )
{
DBID			ramID;
DBID			prevID;
RAMIconBitMap 	*ramIconBM;

	ramIconBM = 0;
	
	ramID = DBGetFirstItemID( kRAMIconType );
	
	if ( ramID != kDBIDErrorValue )
	{
		do 
		{
			ramIconBM = (RAMIconBitMap *)DBGetItem( kRAMIconType, ramID );
			prevID = ramID;
			ramID = DBGetNextItemID( kRAMIconType, ramID );
		} 
		while ( ramIconBM && 
				!( ( ramIconBM->box.region == user->box.region ) && 
				   ( ramIconBM->box.box == user->box.box ) && 
				   ( ramIconBM->userID == user->userID ) ) 
			  );
	}

	if ( ramIconBM )		/* did we find a match? */
	{
		DBDeleteItem( kRAMIconType, prevID );
		DBDeleteItem( kIconDescType, prevID );
	}
}


/*
	Is there a RAM icon for this player?
 */
 
DBID _PlayerRAMIconExists( userIdentification *user )
{
DBID			ramID;
DBID			prevID;
RAMIconBitMap 	*ramIconBM;

	ramIconBM = 0;
	
	ramID = DBGetFirstItemID( kRAMIconType );
	
	if ( ramID != kDBIDErrorValue )
	{
		do 
		{
			ramIconBM = (RAMIconBitMap *)DBGetItem( kRAMIconType, ramID );
			prevID = ramID;
			ramID = DBGetNextItemID( kRAMIconType, ramID );
		} 
		while ( ramIconBM && 
				!( ( ramIconBM->box.region == user->box.region ) && 
				   ( ramIconBM->box.box == user->box.box ) && 
				   ( ramIconBM->userID == user->userID ) ) 
			  );
	}

	if ( ramIconBM )			/* did we find a match? */
		return ( prevID );
	else
		return ( kDBReservedItemType );
}


SegaBitMapPtr _GetPlayerRAMIconBitMap( userIdentification *user )
{
DBID 			theID;
RAMIconBitMap 	*ramIconBM;

	theID = PlayerRAMIconExists( user );
	
	if ( theID != kDBReservedItemType )
	{
		ramIconBM = (RAMIconBitMap *)DBGetItem( kRAMIconType, theID );
		return ( &(ramIconBM->bitmap) );
	}
	else
		return ( 0 );
}


SegaBitMapPtr _GetPlayerIconBitMap( userIdentification *user )
{
SegaBitMapPtr	sbmp;
SegaBitMap		*romBM;

	sbmp = GetPlayerRAMIconBitMap( user );
	
	if ( sbmp )
		return( sbmp );
	else
	{
		romBM = (SegaBitMap *)DBGetItem( kIconBitMapType, user->ROMIconID );
		
		if ( !romBM )
			romBM = (SegaBitMap *)DBGetItem( kIconBitMapType, kNewPlayerIcon );
		
		return( romBM );
	}
}

 
SegaBitMap * _GetIconBitMap( DBID iconID )
{
	return( (SegaBitMap *)DBGetItem( kIconBitMapType, iconID ) );
}

/*
	theID refers to an ISEG or RAIC.  If an IDSC with the same ID is present, then this is an
	animated icon.

	IDSC's just contain a word frame delay value.
	
	The graphicsType field in the iconReference indicates whether it's an animation
	or static, and the reference becomes either the AnimationRef or GraphicsReference,
	accordingly.
	
	If we have trouble getting any memory or DB items, we bail.
*/

static IconReference DecodeAndDraw( SegaBitMap *bitmap, DBID theID, short xPos, short yPos, unsigned char hue, Boolean grayFlag )
{
IconReference 	ref;
IconSetup		*iconSetup;

	ref = (IconReference) NewMemory( kTemp, sizeof(IconReferenceRec) );
	
	if ( ref )
		{
		ref->animationData.bitmap = bitmap;
		
		if ( ref->animationData.bitmap )
			{
			iconSetup = (IconSetup *)DBGetItem( kIconDescType, theID );				// is it animated?

			ref->animationData.xPos	= xPos;		
			ref->animationData.yPos = yPos;

			if ( !iconSetup )														// static graphic 
				{
				ref->graphicsType = kStaticGraphicIcon;
				if (grayFlag)
					ref->reference = (long)DrawGraphicGray( ref->animationData.bitmap, 
														ref->animationData.xPos >> 3, 	// div 8 
														ref->animationData.yPos >> 3, 	// div 8 
														0, 0 );
				else
					ref->reference = (long)DrawGraphic( ref->animationData.bitmap, 
														ref->animationData.xPos >> 3, 	// div 8 
														ref->animationData.yPos >> 3, 	// div 8 
														0, 0, hue );
				}
				else																// animation 
					{
					ref->graphicsType = kAnimationIcon;
					ref->animationData.frameDelay = iconSetup->frameDelay;			

					ref->reference = (long)SpawnAnimation( kFrameAnimate, 
																(long) &ref->animationData );
					if ( ref->reference )
						StartAnimation ( (AnimationRef)ref->reference );
						else
							goto errorBail;
					}
			}
			else
				goto errorBail;
		}
		else
			goto errorBail;
		
	return ( ref );
	
errorBail:
	if ( ref )
		DisposeMemory( ref );
	return ( 0 );
}



void _DisposeIconReference( IconReference ref )
{
	if ( ref )
		{
		if (ref->graphicsType == kStaticGraphicIcon )
			DisposeGraphicReference ( (GraphicReference)ref->reference );
			else
				DisposeAnimation ( (AnimationRef)ref->reference );
		
		DisposeMemory( ref );
		}
}


