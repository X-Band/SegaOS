/*
	File:		FindOpponentPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/11/94	JBH		Moved XBand logo and sparks into an abstract type in sprites
		 <8>	 8/11/94	JBH		Added sprites for XBand logo. Added space for TheAnimator
									persistent data to fix sparks
		 <7>	  8/6/94	JBH		Added messagePatterns to find opponent globals.
		 <6>	 7/26/94	JBH		Added mazeInProgress flag.
		 <5>	 7/25/94	KON		Remove return icon ref since it's handled purely by the news
									goop now.
		 <4>	 7/25/94	KON		Get rid of server connect pattern strucuture, or whatever that
									fucking thing was.
		 <3>	 7/24/94	JBH		Added sparks to find opponent screen
		 <2>	  7/1/94	KON		Add stuff for pattern management.
		 <1>	 6/10/94	KON		first checked in

	To Do:
*/

#ifndef __Sprites__
#include "Sprites.h"
#endif

#ifndef __Animation__
#include "Animation.h"
#endif

typedef struct 
{
	XBandLogoRef		xbandLogo;
	long				textPatterns;
	long				messagePatterns;
	Boolean				mazeInProgress;
} FindOpponentRefCon;
