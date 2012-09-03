/*
	File:		VersusScreenPriv.h

	Contains:	xxx put contents here xxx

	Written by:	Joe Britt

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<15>	 8/23/94	ADS		killed unused patterns
		<14>	 8/21/94	JOE		added a flag for "local user on left"
		<13>	 8/11/94	JOE		added some stuff to the refcon struct for the new taunt scroller
		<12>	  8/8/94	HEC		kScreenMinDisplayTime was too big!
		<11>	  8/7/94	JOE		add support for amulet & line graphics
		<10>	  8/7/94	HEC		Added buttonPressed and peerConnectErr global.
		 <9>	  8/5/94	SAH		Added support for a vbl task for gametlak.
		 <8>	  8/3/94	SAH		Do remote impatience.
		 <7>	 7/23/94	JOE		update Laid enum
		 <6>	 7/20/94	JOE		add TextBoxReferences for user jizzles
		 <5>	 7/20/94	JOE		added icon spriterefs
		 <4>	 7/20/94	JOE		work in progress
		 <3>	 7/19/94	JOE		add jizzling box
		 <2>	 7/19/94	JOE		fix mistake
		 <1>	 7/19/94	JOE		first checked in

	To Do:
*/


#define	kTauntPatHalfWidth		(50 >> 3)
#define	kTauntPatHeight			6
#define	kScreenMinDisplayTime	3*60

typedef struct 
{
	DITLItemList 		*theDitlList;
	ControlTable 		*myControlTable;
	long				leftTauntTextPat;
	long				rightTauntTextPat;
	TextBoxReference	myTextBoxMems;
	TextBoxReference	leftUserJizzle;
	TextBoxReference	rightUserJizzle;
	SpriteRef			leftIcon;
	SpriteRef			rightIcon;
	Boolean				impatient;
	Boolean				localPlayerOnLeft;
	SpriteRef			XGraphic;
	SpriteRef			jizzCircle;
	GraphicReference	vertLine;
	GraphicReference	horzLine;
	long				pointsTextPats;
	long				minDisplayTime;
	LooseSession *		lsession;
	long				saveVBL;
	long				saveGTSession;
	long				buttonPressed;
	short				peerConnectErr;
	short				brk1;
	short				brk2;
	short				curTauntCharOffset;
	short				tauntLen;
	short				curLineCharOffset;
	char				*tauntLine1;
	char				*tauntLine2;
	char				*tauntLine3;
	long				pixWidth;
} VersusViewRefCon;


// Items in account info DITL

enum {
	kGameNameText,			// 1

	kLeftPlayerIcon,		// 2	- really a static string, referencing the null
	kRightPlayerIcon,		// 3	  string desc.  we just use it for position.
	
	kLeftPlayerHandle,		// 4
	kRightPlayerHandle,		// 5
	
	kLeftPlayerTown,		// 6
	kRightPlayerTown,		// 7
	
	kLeftPlayerTaunt,		// 8
	kRightPlayerTaunt		// 9
};

