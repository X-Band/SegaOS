/*
	File:		PeerConnect.h

	Contains:	xxx put contents here xxx

	Written by:	Ted, Shannon, Andy

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<21>	 8/27/94	SAH		Exchange peer data returns the opponent's verification tag.
		<20>	 8/23/94	HEC		Lose the string param in DoPeerDialog.
		<19>	 8/23/94	SAH		PeerConnectionDropped does not return an error.
		<18>	 8/22/94	ADS		Got rid of obsolete call
		<17>	 8/21/94	HEC		ConnectToPeer takes new params.
		<16>	 8/19/94	HEC		Removed PeerConnectTimeout
		<15>	 8/14/94	HEC		Changing prototypes.
		<14>	 8/11/94	JBH		Moved spark-related poo out of here!
		<13>	  8/6/94	HEC		Added DoPeerDialog
		<12>	  8/4/94	HEC		Fixed a typo.
		<11>	  8/4/94	HEC		Removed and changes some poo.
		<10>	  8/3/94	SAH		Made PeerConnectionDropped return an error.
		 <9>	  8/3/94	SAH		DisplayPeerInfo now take a gametalk session.
		 <8>	  8/2/94	HEC		Happy Teddis.
		 <7>	  8/1/94	HEC		Removed obsolete protos.
		 <6>	  8/1/94	HEC		Added new routines, delete old ones.
		 <5>	 7/31/94	HEC		Added new routines, delete old ones.
		 <4>	 7/22/94	DJ		added error return from DoPeerDataExchange
		 <3>	 7/18/94	ADS		Dispatcher cleanup
		 <2>	 7/15/94	SAH		Made Chat Pubic.
		 <1>	 7/15/94	ADS		Stripped from Intro.c

	To Do:
*/



#ifndef __PeerConnect__
#define __PeerConnect__

#include "TransportStructs.h"
#include "SegaOS.h"
#include "GT.h"
#include "MessDisp.h"


void		DoPeerConnection( void ) =
	CallDispatchedFunction ( kDoPeerConnection );

OSErr		ConnectToPeer( Boolean master, Boolean setupListen, long retries, long timeout ) =
	CallDispatchedFunction ( kConnectToPeer );

void		DisplayPeerInfo( GTSession * session ) =
	CallDispatchedFunction ( kDisplayPeerInfo );

OSErr		DoSlavePeerConnect( long pmodemFlags, long retries, long timeout ) =
	CallDispatchedFunction ( kDoSlavePeerConnect );

OSErr		DoMasterPeerConnect( long pmodemFlags, long retries, long timeout ) =
	CallDispatchedFunction ( kDoMasterPeerConnect );

void		DoPeerRestoreOS( void ) = 
	CallDispatchedFunction ( kDoPeerRestoreOS );

void		PeerConnectionDropped( OSErr err ) =
	CallDispatchedFunction ( kPeerConnectionDropped );

OSErr		DoExchangePeerData( GTSession *gtSession, long * opponentTag ) =
	CallDispatchedFunction ( kDoExchangePeerData );

void 		Chat (GTSession *session, Boolean master) =
	CallDispatchedFunction ( kChat );

short		DoPeerDialog( DBID dialogID, long timeout ) =
	CallDispatchedFunction ( kDoPeerDialog );

#endif	__PeerConnect__


