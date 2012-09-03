/*
	File:		ScreenLists.h

	Contains:	Central lists of screens and DITL's

	Written by:	Andy Stadler

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 8/22/94	CMY		Removed an obsolete screen
		<11>	 8/13/94	JBH		Added maze screen
		<10>	  8/4/94	CMY		Added new player screen.
		 <9>	 7/28/94	JOE		add sekrit sound test screen
		 <8>	 7/19/94	JOE		add versus screen
		 <7>	 7/18/94	SGR		added IntroAnimation screen ID for sounds and LEDs
		 <6>	 7/17/94	JOE		add account info screen
		 <5>	 7/17/94	ADS		Add new ditl to about box
		 <4>	 7/15/94	ADS		Add password entry screen
		 <3>	 7/14/94	CMY		Added XBand setup screen
		 <2>	 7/10/94	CMY		Added  AKA screens
		 <1>	  7/6/94	ADS		first checked in

	To Do:
*/




#ifndef _SCREENLISTS_H_
#define _SCREENLISTS_H_



///------------------------------------------------------------------------------
///
///	Any time you add a new screen to the system, come to this file
/// to make sure you've got all the right stuff in place.
///
///
///	There are two major groupings in this file.  Sections 1-4 must be defined
///	for EVERY screen.  Then, if a particular screen is based on the dialog
///	manager, you must also add entries to sections 5-7.
///
/// Collecting all per-screen identifiers in this file is intended to simplify
///	the process of adding a new screen.
///
///------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//	#1.  Screen maps
//
//  These entries are used by the screens map to control the links
//  from one screen to another.  They are the "logical" ID numbers
//  of each screen.
//
//	Ref. Screens.c

enum
{
	kChoosePlayerScreen = 1,
	kMainScreen = 2,
	kFindOpponent = 3,
	kAddressBookScreen = 4,
	kAddressEntryScreen = 5,
	kHistoryViewScreen = 6,
	kDisplayMailScreen = 7,
	kMailEntryScreen = 8,
	kDisplayOptionsScreen = 9,
	k4WayMailScreen = 10,
	k4WayOptionsScreen = 11,
	kNewsScreen = 12,
	kMailEditScreen = 13,
	kAddressBookNewScreen = 14,
	kAboutScreen = 15,
	kPlayerInfoOptionsScreen = 16,
	kChooseHandleScreen = 17,
	kChooseIconScreen = 18,
	kChooseTauntScreen = 19,
	kChooseInfoTextScreen = 20,
	kChoosePasswordScreen = 21,
	kXBandSetupScreen = 22,
	kIntroAnimationScreen = 23,
	kVersusScreen = 24,
	kSoundTestScreen = 25,
	kNewPlayerScreen = 26,
	kMazeScreen = 27
};


//-------------------------------------------------------------------------------
//	#2.  Screen code resource setup
//
//  These entries are used to build up the database of screen dispatchers
//
//	Ref. Screens.c

Ptr GetAddressBookDispatchPtr( void );
Ptr GetAddressEntryDispatchPtr( void );
Ptr GetHistoryViewDispatchPtr( void );
Ptr GetDisplayMailDispatchPtr( void );
Ptr GetMailEntryDispatchPtr( void );
Ptr GetMailEditDispatchPtr( void );
Ptr GetNewAddressDispatchPtr( void );
Ptr GetMainScreenDispatchPtr( void );
Ptr GetChoosePlayerDispatchPtr( void );
Ptr GetOptionsDispatchPtr( void );
Ptr GetFindOpponentDispatchPtr( void );
Ptr GetFourWayMailDispatchPtr( void );
Ptr GetFourWayOptionsDispatchPtr( void );
Ptr GetNewsScreenDispatchPtr( void );
Ptr GetAboutBoxDispatchPtr( void );
Ptr GetPlayerInfoOptsDispatchPtr( void );
Ptr GetChooseHandleDispatchPtr( void );
Ptr GetChooseIconDispatchPtr( void );
Ptr GetChooseTauntDispatchPtr( void );
Ptr GetChooseInfoDispatchPtr( void );
Ptr GetChoosePasswordDispatchPtr( void );
Ptr GetXSetupDispatchPtr( void );
Ptr GetVersusViewDispatchPtr( void );
Ptr GetSoundTestDispatchPtr( void );
Ptr GetNewPlayerDispatchPtr( void );
Ptr	GetMazeDispatchPtr( void );

//-------------------------------------------------------------------------------
//	#3.  Screen code resource loading
//
//  Be sure to add a section to Screens.c, in the function _InitScreens(), which
//	uses the above "GetxxxDispatchPtr" function call.
//
//	Ref. Screens.c


//-------------------------------------------------------------------------------
//	#4.  Screen navigation tables
//
//  Be sure to add a new entry to ScreenStateTables.r, using the above ID numbers
//
//	Ref. Screens.c



//-------------------------------------------------------------------------------
//	#5.  Screen layout identifiers
//
//  If your screen uses the standard dialog manager, you need an identifier
//  here to map to it.  These ID's are NOT the same as the above per-screen list,
//  since not all screens use ditl's to describe them.
//
//  If you aren't using this, then you can skip #6 and #7 below.
//
//	Ref. DITLItemSetup.c

enum
{
	kMailLayout,				// 0
	kMailRead,
	kAddressBookLayout,
	kAddressRead,
	kChoose4PlayerScreen,
	kTopLevelInterfaceScreen,	// 5
	kOptionsScreen,
	kFourWayMailScreen,
	kOBSOLETE_SCREEN,
	kRankingScreen,
	kEmptyMailLayout,			// 10
	kEmptyAddressBookLayout,
	kPlayerInfoOptsLayout,
	kAboutBoxLayout,
	kAccountInfoScreen,
	kVersusScreenLayout,		// 15
	kSoundTestScreenLayout			
};



//-------------------------------------------------------------------------------
//	#6.  Screen layout resources
//
//  Be sure to add a "Laid" resource to describe your dialog screen
//
//	Ref. DITLItemSetup.c



//-------------------------------------------------------------------------------
//	#7.  Screen layout control table resources
//
//  Be sure to add a "Cntl" resource to describe your dialog screen
//
//	Ref. DITLItemSetup.c




#endif			// _SCREENLISTS_H_








