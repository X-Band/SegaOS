/*
	File:		MainScreenPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <4>	 8/19/94	CMY		Added a string to indicate when the box is registered.
		 <3>	  8/5/94	JOE		Support Jizzlation disable
		 <2>	  8/2/94	CMY		Support blinkies
		 <1>	 6/10/94	KON		first checked in

	To Do:
*/



typedef struct 
{
	DITLItemList 	*theDitlList;
	ControlTable 	*myControlTable;
	SpriteRef		blinky;
	long			blinkyTime;
	SecretSequence	*jizzleDisableSeq;
	long			clearPattern;
	long			registeredPatterns;
	segaCharRect	patternRect;
} MainScreenRefCon;
