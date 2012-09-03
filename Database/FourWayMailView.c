/*
	File:		FourWayMailView.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<16>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<15>	 7/20/94	KON		Rearrange the news.
		<14>	 7/19/94	CMY		Don't fuck up my graphics in my DB entry.
		<13>	 7/10/94	KON		Added support for news read and unread graphics.
		<12>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<11>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		<10>	  7/4/94	ADS		New interface for screen control
		 <9>	 6/30/94	KON		Add newsScreen.
		 <8>	 6/20/94	KON		Update for CES: look and dialogs.
		 <7>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <6>	 6/13/94	KON		Make mail icon reflect amount of mail.
		 <5>	 6/13/94	KON		Make selection parameter to screens a long.
		 <4>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <3>	 6/12/94	SAH		Use backdrop manager and be pattern happy.
		 <2>	 6/11/94	KON		Remember cursor position.
		 <1>	 6/11/94	KON		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "vdp.h"
#include "Mail.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "Errors.h"
#include "DITLMgr.h"
#include "Heaps.h"
#include "SegaIn.h"
#include "Events.h"
#include "TextUtls.h"
#include "StringDB.h"
#include "TextButt.h"
#include "GraphicsDB.h"
#include "Time.h"
#include "DITLItemSetup.h"
#include "MailEdit.h"
#include "FourWayMailPriv.h"
#include "MungeCode.h"
#include "Screens.h"
#include "Backdrops.h"
#include "DialogMgr.h"
#include "DBTypes.h"
#include "MailView.h"
#include "News.h"

static long FourWayMailDispatch( short action, long command, long *refCon, ScreenParams *params );
static long FourWayMailPreflight( long initValue );
static void FourWayMailReset( long refCon );
static long FourWayMailSetup( ScreenParams *params );
static long FourWayMailCommand( long command, long refCon, ScreenParams *params );
static void FourWayMailKill( long refCon );

static long FourWayMailDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = FourWayMailPreflight( params->initValue );
			break;

		case kDoScreenReset:
			FourWayMailReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = FourWayMailSetup( params );
			break;

		case kDoScreenCommand:
			result = FourWayMailCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			FourWayMailKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in FourWay Mail Dispatch" );
			break;
			
		}
	return result;
}

static void FourWayMailReset( long refCon )
{
FourWayRefCon	*mailRef;

	mailRef = (FourWayRefCon *)refCon;
	ClearDITLDone( mailRef->theDitlList );
}

static long FourWayMailPreflight( long initValue )
{
	return 0;
}

static long FourWayMailSetup( ScreenParams *params )
{
DITLItemList 	*theDitlList;
ControlTable 	*theControlTable;
FourWayRefCon	*mailRef;
short			count;
DBID			myMailGraphicID;
DBID			myGeneralNewsGraphicID;
DBID			myCatapultNewsGraphicID;

	mailRef = (FourWayRefCon *)NewMemory( kTemp, sizeof(FourWayRefCon) );

	EraseGDevice ( kScrollA );
	SetBackdropID ( kBackdrop, true, 0 );
	SetCurrentDevice( kScrollA );

	theDitlList = SetupDITLItemList( kFourWayMailScreen );

	myGeneralNewsGraphicID = GetNewsGraphicsID( kGeneralNews );
	myCatapultNewsGraphicID = GetNewsGraphicsID( kCatapultNews );
	myMailGraphicID = GetInBoxGraphicID( k4WayMailScreen );

#if 0			
	//
	// update buttons for news read status and number of mail items
	//
	((TextButtonDesc*)theDitlList->items[0].objectData)->animationData.bitmap = GetDBGraphics( myGeneralNewsGraphicID );	
	((TextButtonDesc*)theDitlList->items[1].objectData)->animationData.bitmap = GetDBGraphics( myCatapultNewsGraphicID );	
	((TextButtonDesc*)theDitlList->items[2].objectData)->animationData.bitmap = GetDBGraphics( myMailGraphicID );	
#endif		
	
	theControlTable = SetupControlTable( kFourWayMailScreen );
	theControlTable->selection = params->selection;
	
	NewDITL( theControlTable, theDitlList );
	
	mailRef->theDitlList = theDitlList;
	mailRef->theControlTable = theControlTable;
	
	return (long)mailRef;
}	
	
static long FourWayMailCommand( long command, long refCon, ScreenParams *params )
{
FourWayRefCon	*mailRef;
Boolean			done;
long			result;

	mailRef = (FourWayRefCon *)refCon;
	result = 0;		//not done, keep looping
	params->initValue = 0;
	
	done = GiveDITLTime( mailRef->theControlTable, mailRef->theDitlList, command );

	params->selection = GetDITLItem( mailRef->theControlTable );

	if( done )
	{
		result = params->selection;
		
		if( result == 2 )
		{
			params->initValue = kOtherNews;
		}
		else
		if( result == 1 )
		{
			params->initValue = kDailyNews;
		}

		result = result + 1;	//Buttons are numbered 1-5 in the real world
	}
	return result;
}


static void FourWayMailKill( long refCon )
{
FourWayRefCon	*mailRef;

	mailRef = (FourWayRefCon *)refCon;

	DisposeDITL( mailRef->theDitlList );
	DisposeControlTable( mailRef->theControlTable );
	DisposeDITLItemList( mailRef->theDitlList );

	DisposeMemory( (Ptr)refCon );
}


Ptr GetFourWayMailDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) FourWayMailDispatch + 2));
	size = (*(long *) ((Ptr) GetFourWayMailDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for FourWay Mail proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) FourWayMailDispatch;
#else
		CheckCodeForJTRefs((void *)FourWayMailDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}

