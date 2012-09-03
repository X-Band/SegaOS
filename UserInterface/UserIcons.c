/*
	File:		UserIcons.c

	Contains:	User Icons (animated or static) screen layout item stuff

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <6>	 8/12/94	JOE		new icon stuff
		 <5>	  8/3/94	JOE		update for pers mgr changes
		 <4>	  7/9/94	CMY		Made it Think C debuggable.
		 <3>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <2>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <1>	  7/3/94	JOE		first checked in
	To Do:
*/

#ifdef	ROMDB
#undef	SIMULATOR
#endif

#include "UserIcons.h"
#include "Errors.h"
#include "VDP.h"
#include "SegaIn.h"
#include "PatchDBHeaps.h"
#include "Heaps.h"
#include "MungeCode.h"
#include "SegaScrn.h"
#include "SegaBM.h"
#include "UsrConfg.h"
#include "Personification.h"

static void RenderDITLUserIcon( DITLItem *theItem );


static Boolean DoDITLUserIcon( short message, DITLItem *theItem, short keyPress )
{
Boolean		   done;
UserIconPriv * state;

	done = false;
	switch ( message )
	{
		case kInitDITLElement:
			theItem->refCon = 0;
			state = NewMemory ( false, sizeof(UserIconPriv) );
			if ( state )
			{
				theItem->refCon = (long) state;
				state->iconRef = nil;
			}
			break;
			
		case kRenderDITLElement:
			RenderDITLUserIcon( theItem );		
			break;
				
		case kSelectDITLElement:
			break;
			
		case kDeselectDITLElement:
			break;
			
		case kClickDITLElement:
			done = (keyPress & kButtonPressed) != 0;
			break;
			
		case kCloseDITLElement:
			state = (UserIconPriv *) theItem->refCon;
			if ( state )
			{
				if ( state->iconRef )
				{
					DisposeIconReference ( state->iconRef );
				}
								
				DisposeMemory (state );
			}
			break;
			
		default:
			ERROR_MESG( "Got an illegal selector in DoDITLUserIcon" );
			
	}
		
	return done;
}

void RenderDITLUserIcon( DITLItem *theItem )
{
UserIconPriv 			*state;
userIdentification		*curUser;

	state = (UserIconPriv *) theItem->refCon;
	if ( !state )
		return;
	
	curUser = GetCurrentLocalUser();			
	
	SetCurrentDevice( kScrollA );

	state->iconRef = DrawPlayerIcon( curUser, theItem->xPos, theItem->yPos, 0 );
		
	SetCurrentDevice( kScrollA );
	VDPIdle();
}


doDITLItemProc GetUserIconItemPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) DoDITLUserIcon + 2));

	size = (*(long *) ((Ptr) GetUserIconItemPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for UserIcon proc" );
	
	if ( data )
		{
#ifndef SIMULATOR
		CheckCodeForJTRefs((void *)DoDITLUserIcon, size);
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) DoDITLUserIcon;
#endif
		SegaBlockMove ( (Ptr) srcFunction, data, size );
		}
	
	return (doDITLItemProc)data;
}
