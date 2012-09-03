/*
	File:		GraphicsDB.h

	Contains:	xxx put contents here xxx

	Written by:	xxx put writers here xxx

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<56>	 8/27/94	JOE		Added kStatsIconFrame, kSegaLogo, kBlockbusterLogo
		<55>	 8/25/94	CMY		Added a return key for the chat keyboard.
		<54>	 8/24/94	CMY		Added a disabled cancel key.
		<53>	 8/22/94	JBH		Added password box graphic.
		<52>	 8/21/94	KON		Rename kVerticalLine to kVerticalLineGraphic so it doesn't
									conflict with everyone.
		<51>	 8/21/94	KON		Added HueShift call.
		<50>	 8/20/94	KON		Add bold10 and bold12 lines for the newspaper.
		<49>	 8/18/94	CMY		kDisabledSwitchKey -> kEnabledSwitchKey
		<48>	 8/13/94	CMY		Added disabled "switch" key for keyboard.
		<47>	 8/13/94	JBH		Added letters for XBand news, xband news background, return icon
									with default clut
		<46>	 8/11/94	KON		Added a bunch of newspaper icons.
		<45>	 8/11/94	KON		Add snowboard graphic.
		<44>	  8/9/94	CMY		Added kPasswordBox.
		<43>	  8/9/94	CMY		Added bullet.
		<42>	  8/8/94	JBH		Added horizontal and vertical shiners
		<41>	  8/7/94	CMY		Added checkmark for read mail.
		<40>	  8/7/94	JOE		added versus screen amulet graphics & linez
		<39>	  8/5/94	CMY		Chagned shit.
		<38>	  8/3/94	CMY		Added new graphics for the 1st time setup screens.
		<37>	  8/3/94	CMY		Added new graphics. Removed old graphics. The cycle of life...
		<36>	  8/1/94	CMY		Add new intro animation
		<35>	 7/27/94	KON		Add big XBAND logo, get rid of old modem.
		<34>	 7/25/94	KON		Add enable, disable, and hilite versions of page control
									graphics.
		<33>	 7/25/94	CMY		Added boxes for handle and taunt entry.
		<32>	 7/24/94	CMY		Added QWERTY keyboards and left/righte buttons
		<31>	 7/22/94	CMY		Added small envelope and player graphics.
		<30>	 7/21/94	CMY		Added enums for the backgrounds
		<29>	 7/19/94	CMY		Added support for ButtonGraphics.
		<28>	 7/18/94	KON		Updated to support new backgrounds.
		<27>	 7/17/94	CMY		Update the frame numbers for the intro animation
		<26>	 7/16/94	CMY		Added intro animation frames.
		<25>	 7/13/94	CMY		Fixed the numbers. Added ranking graphic to the DB.
		<24>	 7/12/94	CMY		Added upper and lowercase keyboard graphics
		<23>	 7/11/94	JBH		Added chat background graphics ID.
		<22>	 7/10/94	KON		Added graphics for news read and unread.
		<21>	 7/10/94	CMY		Added id's for AKA screen backgrounds
		<20>	  7/2/94	CMY		Added keyboard layouts
		<19>	 6/30/94	KON		Add right arrow, left arrow, and mail buttons.
		<18>	 6/28/94	CMY		Added graphics references to handle clut and pattern management.
		<17>	 6/21/94	KON		Add stats background.
		<16>	 6/20/94	KON		Add options background screen.
		<15>	 6/20/94	KON		Add CES player slaves.
		<14>	 6/20/94	KON		Add graphics for personal mail empty, regular and full.
		<13>	 6/13/94	SAH		Managerized.
		<12>	 6/13/94	KON		Add new mail icons to reflect amount of mail.
		<11>	  6/5/94	KON		Added modem connecting strings...
		<10>	  6/5/94	KON		Add some more graphics to the DB and remove lots of stale ones.
		 <9>	  6/5/94	KON		Add a bunch of names to the graphics DB.
		 <8>	  6/4/94	KON		Add new DBID's for mail and address backgrounds. Temporarily
									using the granite background as a replacement till RAM story is
									figured out.
		 <7>	  6/3/94	DJ		added screen as a param to DrawDBGraphic
		 <6>	  6/3/94	DJ		News and animated news icon for Jams
		 <5>	  6/3/94	KON		Add kAddressBookBackdrop.
		 <4>	  6/2/94	KON		Added names for keyboard cursors and granite backdrop.
		 <3>	 5/29/94	HEC		Added DrabDBGraphicAt()
		 <2>	 5/29/94	KON		Fucking thing got trashed on checkin. KON 3.
		 <1>	 5/29/94	KON		first checked in

	To Do:
*/

#ifndef __GraphicsDB__
#define __GraphicsDB__

#ifndef __DataBase__
#include "DataBase.h"
#endif __DataBase__

#ifndef __SegaBM__
#include "SegaBM.h"
#endif __SegaBM__

enum
{
	kPageLeftDisableControl = 4,
	kPageLeftEnableControl,
	kPageLeftHiliteControl,
	kPageRightDisableControl,
	kPageRightEnableControl,
	kPageRightHiliteControl,
	kSmallReturnArrow,
	kBackdrop,
	kFindOpponentIcon,
	kRankingIcon,
	kMailIcon,			//14
	kAddressBookIcon,
	kEnhancementsIcon,
	kOptionsIcon,
	kMailCursor,		//18
	kThinKeyboardCursor,
	kWideKeyboardCursor,

	kCatapultNewsBackground = 23,
	kNBAJamLogo,		//24
	kOBSOLETE_BACKGROUND1,
	kPersonalMailIcon = 28,
	kLittleDeleteIcon,
	kPlayerProfileIcon,
	kReturnIcon,			//32
	kOBSOLETE_BACKGROUND2,	//33
	kLittleReadIcon,
	kConnectIcon,			//35
	kLittleNewIcon = 37,
	kBigReturnIcon,
	kLittleReplyIcon,
	kXBandLogoBig,				//40
	kNoMail,
	kOverflowingMail,
	kReadMailButton,
	kPersonalOverflowingMailIcon,
	kPersonalNoMailIcon,
	kReturnKey,
	kOptionsBackground,
	kStatsBackground,
	kMailReplyButton,
	kMailDeleteButton,		//50
	kMailNewButton,
	kMailExitButton,
	kRightArrow,
	kLeftArrow,				//54
	kGeneralNewsUnreadIcon,
	kGeneralNewsReadIcon,
	kCatapultNewsUnreadIcon,
	kCatapultNewsReadIcon,
	kChatBackground,
	kTextEditCaretImage,
	kRankingGraphic,
	kAboutBoxBall,
	kAboutBoxPaddle,
	kKeyboardABCLower,
	kKeyboardABCUpper,
	kOBSOLETE2,
	kOBSOLETE,
	kType2PlainBackground,	//68
	kBlackBackground = kType2PlainBackground,
	kYellowXBandLogo = 73,		//73
	kKeyboardQWERTYLower,
	kKeyboardQWERTYUpper,
	kHandleEntryField,
	kTauntEntryField,
	
	kFirstBlinky = 86,
	kLastBlinky = 98,
	
	kFirstXFrame = 99,
	kFirstAFrame = 107,
	kLetterB = 123,
	kLetterA,
	kLetterN,
	kLetterD,
	kLetterTM,
	
	kEnvelopeBGGraphic,
	kPlayersBGGraphic,
	
	kStatsLine,
	kMailEditFields,
	kGrayHorizontalLine,
	
	kPersonalInfoBox,
	
	kPreGameCircle,			// 134
	kPreGameVertLine,		// 135
	kPreGameHorzLine,		// 136
	
	kCheckMark,
	kHorizontalShiner,
	kVerticalShiner,
	kBullet,
	kPasswordBox,
	kLetterE,
	kLetterS,
	kLetterW,
	kLetterX,
	kXBandNewsBackground,
	kSmallReturnArrowDefaultClut,
	kEnabledSwitchKey,
	kDisabledCancelKey,
	
	kXMailIcon = 201,
	kAccountInfoIcon,
	kSettingsIcon,
	kStatsIcon,
	kPlayerListIcon,

	kSnowboardIcon = 207,
	kMovies2Icon,
	kMovies1Icon,
	kMovies3Icon,
	kThumbsUpIcon,
	kThumbsDownIcon,
	kThumbsUpDownIcon,
	kSportsIcon,
	kNEWIcon,
	kMusicIcon,
	
	kBold10LinesGraphic = 220,
	kBold12LinesGraphic = 221,
	kVerticalLineGraphic = 222,
	
	kPasswordBoxForAnimation,
	kBlockbusterLogo,				// 224
	kSegaLogo,						// 225
	kStatsIconFrame,				// 226
	
	/* keep this one as last */
	kLastGraphic
};

/* Frame offsets for button graphics */
enum
{
	kFlatFrame = 0,
	kFlipUp1Frame,
	kFlipUp2Frame,
	kFlipUp3Frame,
	kAnimationFrames
};

#ifndef __SERVER__

SegaBitMapPtr GetDBGraphics( DBID graphicNum ) =
	CallDispatchedFunction( kGetDBGraphics );

SegaBitMapPtr GetDBButtonFrame( DBID buttonNumber, DBID frame ) =
	CallDispatchedFunction( kGetDBButtonFrame );
	
GraphicReference DrawDBGraphic( long memToUse, DBID theID, short theScreen ) =
	CallDispatchedFunction( kDrawDBGraphic );

GraphicReference DrawDBGraphicAt( long memToUse, DBID theID, short xPos, short yPos, short theScreen ) =
	CallDispatchedFunction( kDrawDBGraphicAt );

short HueShift( short color, unsigned char shiftAmount ) =
	CallDispatchedFunction( kHueShift );


#endif __SERVER__

#endif __GraphicsDB__