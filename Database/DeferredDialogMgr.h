/*
	File:		DeferredDialogMgr.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/17/94	HEC		kDDASAFP is now combination of kDD's but NOT kDDBoot.
		 <8>	 8/17/94	HEC		Bit field dialog when id's.
		 <7>	 8/15/94	JBH		Added EnableDefDialogs, DisableDefDialogs
		 <6>	  8/9/94	KON		Add count deferred dialogs.
		 <5>	 7/15/94	dwh		unix-ise.
		 <4>	  7/7/94	KON		Change kDDASAFP to kDDMainEventLoop.
		 <3>	  7/1/94	KON		Add minTime and maxTime to dialog q-ing.
		 <2>	  7/1/94	KON		added kDDServerConnectDoneImmediate.
		 <1>	 6/20/94	KON		first checked in

	To Do:
*/

#ifndef __DeferredDialogMgr__
#define __DeferredDialogMgr__

#define kDDMainEventLoop				0x00000001
#define kDDBoot							0x00000002
#define kDDGameOver						0x00000004
#define kDDServerConnect				0x00000008
#define kDDServerConnectDoneImmediate	0x00000010
#define kDDASAFP (kDDMainEventLoop|kDDGameOver|kDDServerConnect|kDDServerConnectDoneImmediate)

#ifndef	unix
void QDefDialog( short when, char *cString, DBID dialogTemplate, short minTime, short maxTime ) =
	CallDispatchedFunction( kQDefDialog );

static void ShowDefDialogs( short when ) =
	CallDispatchedFunction( kShowDefDialogs );

static short CountDefDialogs( short when ) =
	CallDispatchedFunction( kCountDefDialogs );
	
static void	DisableDefDialogs( void ) =
	CallDispatchedFunction( kDisableDefDialogs );
	
static void EnableDefDialogs( void ) =
	CallDispatchedFunction( kEnableDefDialogs );

#endif	/* ! unix */

#endif __DeferredDialogMgr__

