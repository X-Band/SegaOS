/*
	File:		PlayerInfoOpts.c

	Contains:	xxx put contents here xxx

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<10>	  8/7/94	CMY		Cache the dictionary so the icons draw and navigate faster.
		 <9>	 7/29/94	KON		Add decompressor options to SetBackgroundID.
		 <8>	 7/15/94	ADS		Moved password verify to ChoosePW preflight
		 <7>	 7/11/94	CMY		Added choose icon screen.
		 <6>	 7/11/94	CMY		Added taunt and info text screens
		 <5>	 7/10/94	CMY		Made it call the Choose Handle screen
		 <4>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <3>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <2>	  7/6/94	ADS		Ask for PW before changing (WRONG PLACE)
		 <1>	  7/6/94	ADS		first checked in

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
#include "Decompress.h"
#include "Time.h"
#include "DITLItemSetup.h"
#include "MailEdit.h"
#include "PlayerInfoOptsPriv.h"
#include "MungeCode.h"
#include "Screens.h"
#include "Backdrops.h"
#include "DialogMgr.h"


static long PlayerInfoOptsDispatch( short action, long command, long *refCon, ScreenParams *params );
static long PlayerInfoOptsPreflight( long initValue );
static void PlayerInfoOptsReset( long refCon );
static long PlayerInfoOptsSetup( ScreenParams *params );
static long PlayerInfoOptsCommand( long command, long refCon, ScreenParams *params );
static void PlayerInfoOptsKill( long refCon );

static long PlayerInfoOptsDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = PlayerInfoOptsPreflight( params->initValue );
			break;

		case kDoScreenReset:
			PlayerInfoOptsReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = PlayerInfoOptsSetup( params );
			break;

		case kDoScreenCommand:
			result = PlayerInfoOptsCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			PlayerInfoOptsKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in FourWay Options Dispatch" );
			break;
			
		}
	return result;
}

static void PlayerInfoOptsReset( long refCon )
{
PlayerInfoOptsRefCon	*optionsRef;

	optionsRef = (PlayerInfoOptsRefCon *)refCon;
	ClearDITLDone( optionsRef->theDitlList );
}

static long PlayerInfoOptsPreflight( long initValue )
{
	return 0;
}

static long PlayerInfoOptsSetup( ScreenParams *params )
{
DITLItemList 	*theDitlList;
ControlTable 	*theControlTable;
PlayerInfoOptsRefCon	*optionsRef;
Decompressor	dec;

	optionsRef = (PlayerInfoOptsRefCon *)NewMemory( kTemp, sizeof(PlayerInfoOptsRefCon) );

	EraseGDevice ( kScrollA );
	SetBackdropID ( kBackdrop, true, 0 );
	SetCurrentDevice( kScrollA );

	dec = CreateDecompressor( GetDBButtonFrame( 11, kAnimationFrames ), 0, 0 );
	optionsRef->cache = GetDecompressorCache( dec );
	DisposeDecompressor( dec );
	
	theDitlList = SetupDITLItemList( kPlayerInfoOptsLayout );	
	theControlTable = SetupControlTable( kPlayerInfoOptsLayout );
	theControlTable->selection = params->selection;
	
	NewDITL( theControlTable, theDitlList );
	
	optionsRef->theDitlList = theDitlList;
	optionsRef->theControlTable = theControlTable;
	
	return (long)optionsRef;
}	
	
static long PlayerInfoOptsCommand( long command, long refCon, ScreenParams *params )
{
PlayerInfoOptsRefCon	*optionsRef;
Boolean			done;
long			result;

	optionsRef = (PlayerInfoOptsRefCon *)refCon;
	result = 0;		//not done, keep looping
	params->initValue = 0;
	
	done = GiveDITLTime( optionsRef->theControlTable, optionsRef->theDitlList, command );
	
	params->selection = GetDITLItem( optionsRef->theControlTable );

	if( done )
	{
		result = params->selection;
		
		// special cases in here....
			
		result = result + 1;	//Buttons are numbered 1-N in the real world
	}
	return result;
}


static void PlayerInfoOptsKill( long refCon )
{
PlayerInfoOptsRefCon	*optionsRef;
Decompressor			dec;
	optionsRef = (PlayerInfoOptsRefCon *)refCon;

	dec = CreateDecompressor( GetDBButtonFrame( 11, kAnimationFrames ), 0, 0 );
	ReleaseDecompressorCache( dec, optionsRef->cache );
	DisposeDecompressor( dec );

	DisposeDITL( optionsRef->theDitlList );
	DisposeControlTable( optionsRef->theControlTable );
	DisposeDITLItemList( optionsRef->theDitlList );

	VDPIdle();
	DisposeMemory( (Ptr)refCon );
}


Ptr GetPlayerInfoOptsDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) PlayerInfoOptsDispatch + 2));
	size = (*(long *) ((Ptr) GetPlayerInfoOptsDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for FourWay Options proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) PlayerInfoOptsDispatch;
#else
		CheckCodeForJTRefs((void *)PlayerInfoOptsDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}

