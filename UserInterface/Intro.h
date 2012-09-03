/*
	File:		Intro.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	 8/19/94	HEC		Added param to DoIntroStuff.
		 <7>	 7/16/94	CMY		Added constants for the intro animation
		 <6>	 6/10/94	CMY		Added reset code to simulator.
		 <5>	  6/5/94	KON		Rearrange options and enhancements screen for DW.
		 <4>	  6/4/94	DJ		I think nothing... but what?
		 <3>	  6/3/94	KON		Reordered top interface screen elements.
		 <2>	  6/2/94	KON		Add enum for the six screens.

	To Do:
*/

#ifndef __Intro__
#define __Intro__

enum
{
	kDoFindOpponentScreen,
	kDoAddressBookScreen,
	kDoMailScreen,
	kDoRankingScreen,
	kDoOptionsScreen,
	kDoEnhancementScreen		// really the Select Player screen for demo
};

#define kIntroAnimationX	48
#define kIntroAnimationY	72
#define kIntroAnimationDelay 5
#ifdef SIMULATOR

extern void DoIntroStuff( Boolean justPlayedGame, WindowPtr thisWindow, Rect *windowRect );
void ResetSimulator( void );
extern void StopIntroStuff( void );

#else

extern void DoIntroStuff( Boolean justPlayedGame );

#endif SIMULATOR

#endif __Intro__