/*
	File:		HistViewPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	 8/27/94	JOE		changed for joey
		 <8>	  8/3/94	CMY		Added a banner graphic
		 <7>	 7/26/94	JOE		new UI poo update
		 <6>	 7/20/94	JOE		added jizzlin box
		 <5>	  7/7/94	JOE		use ranking manager
		 <4>	  7/1/94	JOE		add support for multiple ranking pages.
		 <3>	 6/29/94	JOE		History page is now a DITL with static fields.
		 <2>	 6/12/94	SAH		Made pattern happy.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/


#define	kGameNamePatWidth	30

#define	kCurPointsPatWidth	15
#define	kCurLevelPatWidth	15
#define	kNextPointsPatWidth	4
#define	kNextLevelPatWidth	15


#define	kFlipLeftDim		4
#define	kFlipLeftNorm		5
#define	kFlipLeftHilite		6

#define	kFlipRightDim		7
#define	kFlipRightNorm		8
#define	kFlipRightHilite	9



typedef struct 
{
	DITLItemList 		*theDitlList;
	ControlTable 		*myControlTable;
	TextBoxReference	myTextBoxMems;
	GraphicReference	statsLine;
	SpriteRef			iconFrame;
	
	SpriteRef			rightButtRef;
	SpriteRef			leftButtRef;
	char				rightButtState;
	char				leftButtState;
	
	SpriteRef			playerIcon;

	long				playerNamePats;
	segaCharRect		playerNameRect;

	long				playerTownPats;
	segaCharRect		playerTownRect;
	
	long				curPointsTextPats;			

	long				curLevelTextPats;

	long				nextPointsTextPats;
	segaCharRect		nextPointsTextRect;

	long				nextLevelTextPats;
	segaCharRect		nextLevelTextRect;


	long				gameNamePats;				// dynamic fields
	segaCharRect		gameNameRect;
	
	long				curPointsPats;
	segaCharRect		curPointsRect;
	
	long				curLevelPats;
	segaCharRect		curLevelRect;
	
	segaCharRect		botRect;					// this is used for Next & Secret
	long				botPats;					

	Boolean				secretRankDisplayed;
	
} HistoryViewRefCon;



// Items in History Screen's SLAY

#define kHistViewJizzleRect				0
#define kHistViewGameNameRect			1
#define kHistViewPlayerNameRect			2
#define	kHistViewPlayerTownRect			3		// this is for a sprite, so T,L = 0,0
#define	kHistViewCurPointsRect			4
#define	kHistViewCurLevelRect			5
#define	kHistViewBotRect				6		// Next Level/Points, also Secret Rankings

#define	kHistViewStatsLineCoord			0
#define	kHistViewPlayerIconCoord		1
#define	kHistViewStatsFrameCoord		2
#define	kHistViewLeftFlipperCoord		3
#define	kHistViewRightFlipperCoord		4
#define	kHistViewPointsNeededXCoord		5		// X offset for "Points Needed:", RELATIVE TO
												//  THE LEFT EDGE OF botRect!!!
#define	kHistViewNextLevelYAndWidth		6		// Y offset & pixel width
#define	kHistViewPointsNeededYAndWidth	7		// Y offset & pixel width

#define	kHistViewXBandLevelString		0
#define kHistViewXBandPointsString		1
#define	kHistViewNextLevelString		2
#define	kHistViewPointsNeededString		3


// History Screen's Laid just has the return thing

#define	kRankReturn						0


