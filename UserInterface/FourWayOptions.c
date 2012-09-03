/*
	File:		FourWayOptions.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<13>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		<12>	 7/14/94	CMY		Added XBand setup screen.
		<11>	  7/8/94	CMY		Changed call to CheckJTRefs to pass JT ptr, not code ptr.
		<10>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <8>	  7/6/94	ADS		Remember the selection
		 <8>	  7/6/94	ADS		Remove "player info" CES dialog
		 <7>	  7/4/94	ADS		New interface for screen control
		 <6>	 6/20/94	KON		Add CES dialogs and make screen four way as advertised.
		 <5>	 6/15/94	SAH		Added fade parameter to SetBackdropID.
		 <4>	 6/13/94	KON		Make selection parameter to screens a long.
		 <3>	 6/13/94	SAH		Add the ROMDB conditional for the ROM database build.
		 <2>	 6/12/94	SAH		Updated to backdrop manager and made pattern happy.
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
#include "FourWayOptionsPriv.h"
#include "MungeCode.h"
#include "Screens.h"
#include "Backdrops.h"
#include "DialogMgr.h"


static long FourWayOptionsDispatch( short action, long command, long *refCon, ScreenParams *params );
static long FourWayOptionsPreflight( long initValue );
static void FourWayOptionsReset( long refCon );
static long FourWayOptionsSetup( ScreenParams *params );
static long FourWayOptionsCommand( long command, long refCon, ScreenParams *params );
static void FourWayOptionsKill( long refCon );

static long FourWayOptionsDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = FourWayOptionsPreflight( params->initValue );
			break;

		case kDoScreenReset:
			FourWayOptionsReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = FourWayOptionsSetup( params );
			break;

		case kDoScreenCommand:
			result = FourWayOptionsCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			FourWayOptionsKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in FourWay Options Dispatch" );
			break;
			
		}
	return result;
}

static void FourWayOptionsReset( long refCon )
{
FourWayOptionsRefCon	*optionsRef;

	optionsRef = (FourWayOptionsRefCon *)refCon;
	ClearDITLDone( optionsRef->theDitlList );
}

static long FourWayOptionsPreflight( long initValue )
{
	return 0;
}

static long FourWayOptionsSetup( ScreenParams *params )
{
DITLItemList 	*theDitlList;
ControlTable 	*theControlTable;
FourWayOptionsRefCon	*optionsRef;

	optionsRef = (FourWayOptionsRefCon *)NewMemory( kTemp, sizeof(FourWayOptionsRefCon) );

	EraseGDevice ( kScrollA );
	SetBackdropID ( kBackdrop, true, 0 );
	SetCurrentDevice( kScrollA );

	theDitlList = SetupDITLItemList( kOptionsScreen );	
	theControlTable = SetupControlTable( kOptionsScreen );
	theControlTable->selection = params->selection;
	
	NewDITL( theControlTable, theDitlList );
	
	optionsRef->theDitlList = theDitlList;
	optionsRef->theControlTable = theControlTable;
	
	return (long)optionsRef;
}	
	
static long FourWayOptionsCommand( long command, long refCon, ScreenParams *params )
{
FourWayOptionsRefCon	*optionsRef;
Boolean			done;
long			result;

	optionsRef = (FourWayOptionsRefCon *)refCon;
	result = 0;		//not done, keep looping
	params->initValue = 0;
	
	done = GiveDITLTime( optionsRef->theControlTable, optionsRef->theDitlList, command );
	
	params->selection = GetDITLItem( optionsRef->theControlTable );

	if( done )
	{
		result = params->selection + 1;	//Buttons are numbered 1-N in the real world
	}
	return result;
}


static void FourWayOptionsKill( long refCon )
{
FourWayOptionsRefCon	*optionsRef;

	optionsRef = (FourWayOptionsRefCon *)refCon;

	DisposeDITL( optionsRef->theDitlList );
	DisposeControlTable( optionsRef->theControlTable );
	DisposeDITLItemList( optionsRef->theDitlList );

	VDPIdle();
	DisposeMemory( (Ptr)refCon );
}


Ptr GetFourWayOptionsDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) FourWayOptionsDispatch + 2));
	size = (*(long *) ((Ptr) GetFourWayOptionsDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for FourWay Options proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) FourWayOptionsDispatch;
#else
		CheckCodeForJTRefs((void *)FourWayOptionsDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}

