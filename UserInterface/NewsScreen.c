/*
	File:		NewsScreen.c

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	  8/9/94	KON		Added parameter to KillNewsPage to indicate we are leaving news.
		 <7>	 7/31/94	KON		Fade the first page of news in.
		 <6>	 7/25/94	KON		Add control hiliting when pages flip and page turning sounds.
		 <5>	  7/8/94	SAH		Pass real proc ptr into CheckCodeForJTRefs.
		 <4>	  7/8/94	SAH		Call CheckCodeForJTRefs for the ROMDB build.
		 <3>	  7/4/94	ADS		New interface for screen control
		 <2>	 6/30/94	KON		Added new dialogs for no news.
		 <1>	 6/30/94	KON		first checked in

	To Do:
*/


#ifdef	ROMDB
#undef	SIMULATOR
#endif


#include "vdp.h"
#include "SegaBM.h"
#include "SegaScrn.h"
#include "SegaText.h"
#include "Errors.h"
#include "Heaps.h"
#include "SegaIn.h"
#include "Events.h"
#include "TextUtls.h"
#include "StringDB.h"
#include "GraphicsDB.h"
#include "Time.h"
#include "NewsScreenPriv.h"
#include "MungeCode.h"
#include "Screens.h"
#include "Backdrops.h"
#include "DialogMgr.h"
#include "News.h"
#include "DBTypes.h"
#include "NewsUtilsPriv.h"
#include "SegaSound.h"

static long NewsScreenDispatch( short action, long command, long *refCon, ScreenParams *params );
static long NewsScreenPreflight( long initValue );
static void NewsScreenReset( long refCon );
static long NewsScreenSetup( ScreenParams *params );
static long NewsScreenCommand( long command, long refCon, ScreenParams *params );
static void NewsScreenKill( long refCon );

static long NewsScreenDispatch( short action, long command, long *refCon, ScreenParams *params )
{
long	result;

	result = 0;
	
	switch( action )
		{
		case kDoScreenPreflight:
			result = NewsScreenPreflight( params->initValue );
			break;

		case kDoScreenReset:
			NewsScreenReset( *refCon );
			break;

		case kDoScreenSetupCommand:
			*refCon = NewsScreenSetup( params );
			break;

		case kDoScreenCommand:
			result = NewsScreenCommand( command, *refCon, params );
			break;
		
		case kDoScreenKill:
			NewsScreenKill( *refCon );
			break;
			
		default:
			ERROR_MESG( "Got bad selector in News Screen Dispatch" );
			break;
			
		}
	return result;
}

static void NewsScreenReset( long refCon )
{

}

static long NewsScreenPreflight( long initValue )
{
DBType	thisNewsType;

	thisNewsType = initValue;
	
	if( GetNumNewsPages( thisNewsType ) == 0 )
	{
		if( thisNewsType == kOtherNews )
		{
			DoDialog( kNoOtherNews );
		}
		else
		if( thisNewsType == kDailyNews )
		{
			DoDialog( kNoDailyNews );
		}
		else
		{
			ERROR_MESG( "Got unrecognized news type in news screen preflight. You suck." );
		}
		
		return -1;			//no news for this user, bail
	}
	else
		return 0;
}

static long NewsScreenSetup( ScreenParams* params )
{
DBType				thisNewsType;
NewsScreenPrivates	*newsRefCon;

	thisNewsType = params->initValue;

	newsRefCon = (NewsScreenPrivates *) NewMemory( kTemp, sizeof( NewsScreenPrivates ) );
	newsRefCon->curPageNumber = 1;
	newsRefCon->newsType = thisNewsType;

	DrawNewsPage( newsRefCon->curPageNumber, newsRefCon->newsType, true );
	return (long)newsRefCon;
}	
	
static long NewsScreenCommand( long command, long refCon, ScreenParams* params )
{
NewsScreenPrivates	*newsRefCon;
Boolean			done;
long			result;

	newsRefCon = (NewsScreenPrivates *)refCon;
	result = 0;		//not done, keep looping
	params->initValue = 0;
	
	done = false;

	if( command & kRIGHT )
	{
		if( newsRefCon->curPageNumber < GetNumNewsPages( newsRefCon->newsType ) )
		{
			ShowLeftRightPageControls( kRIGHT );	//Hilite
			PlayDBFX( kXBandPageFlipSnd, 0, 0 );
			newsRefCon->curPageNumber++;
			DrawNewsPage( newsRefCon->curPageNumber, newsRefCon->newsType, false );
		}
	}
	else	
	if( command & kLEFT )
	{
		if( newsRefCon->curPageNumber > 1 )
		{
			ShowLeftRightPageControls( kLEFT );	//Hilite
			PlayDBFX( kXBandPageFlipSnd, 0, 0 );
			newsRefCon->curPageNumber--;
			DrawNewsPage( newsRefCon->curPageNumber, newsRefCon->newsType, false );
		}
	}
	else
	if( command & kButtonPressed ) 
		done = true;

	if( done )
	{
		result = kPopScreen;
	}
	return result;
}


static void NewsScreenKill( long refCon )
{
	DisposeMemory( (Ptr)refCon );
	KillCurNewsPage( true );
}


Ptr GetNewsScreenDispatchPtr( void )
{
long	size;
Ptr		data;
long	srcFunction;

	srcFunction = (*(long *) ((Ptr) NewsScreenDispatch + 2));
	size = (*(long *) ((Ptr) GetNewsScreenDispatchPtr + 2)) - srcFunction;
	data = NewMemory ( kTemp, size );
	ASSERT_MESG( data, "Cannot allocate memory for News Screen proc" );
	
	if ( data )
		{
#ifdef SIMULATOR
		*(short *)data = kJmpLongOpcode;
		*(long *)((long)data+2) = (long) NewsScreenDispatch;
#else
		CheckCodeForJTRefs((void *)NewsScreenDispatch, size);
		BlockMove ( (Ptr) srcFunction, data, size );
#endif
		}
	
	return data;
}
