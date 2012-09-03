/*
	File:		PeerConnectPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<11>	 8/28/94	JOE		add id for chat screen LED animation
		<10>	 8/23/94	HEC		Added peer vbls
		 <9>	 8/11/94	JBH		Moved xband logo and sparks into sprites manager.
		 <8>	  8/7/94	HEC		Added displayUserInterface, graphicRef, and textPatterns
									globals.
		 <7>	  8/4/94	HEC		Added some globals.
		 <6>	  8/1/94	HEC		Added reconnectTimeout global and peer connect state defines.
		 <5>	 7/31/94	HEC		Added spark globals
		 <4>	 7/24/94	JBH		Added constants for number of sparks during peer connect.
		 <3>	 7/15/94	SAH		Moved Chat to PeerConnect.h
		 <2>	 7/15/94	JBH		Moved kChat to OSManagers.h
		 <1>	 7/15/94	ADS		Stripped from Intro.c

	To Do:
*/




#ifndef __PeerConnectPriv__
#define __PeerConnectPriv__

#ifndef __Sprites__
#include "Sprites.h"
#endif

#ifndef __PeerConnect__
#include "PeerConnect.h"
#endif

#define	kChatScreenLEDs			130		// since we don't really have a screenID

#define kTauntState 	1
#define kGameState 		2
#define kChatState		3

typedef struct PeerConnGlobals	{
	XBandLogoRef		xbandLogo;
	long				reconnectTimeout;
	unsigned char		lostconnections;
	unsigned char		maxlostconnections;
	unsigned char		displayUserInterface;
	unsigned char		vblInstalled;
	long				textPatterns;
	ControllerType		controlType;
	short				controlValues[5];
	long				oldVBLHandler;
} PeerConnGlobals;


void _PeerStartVBL( void ) =
	CallDispatchedFunction ( kPeerStartVBL );

void _PeerStopVBL( void ) =
	CallDispatchedFunction ( kPeerStopVBL );

void _PeerVBLHandler( void ) =
	CallDispatchedFunction ( kPeerVBLHandler );



#ifdef SIMULATOR
#define	MANAGERGLOBALTYPE		PeerConnGlobals
#else
extern PeerConnGlobals gPeerConnect;
#endif



#endif		// __PeerConnectPriv__