/*
	File:		OptionsPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <7>	 8/12/94	HEC		Added cardDetected global.
		 <6>	  8/8/94	KON		Add graphRef so the account icon can be drawn in scrollB.
		 <5>	 7/22/94	JOE		Joey-ified
		 <4>	 7/17/94	JOE		make smartcard credits live
		 <3>	 7/17/94	JOE		update
		 <2>	 6/12/94	SAH		Made pattern happy.
		 <1>	 6/10/94	KON		first checked in

	To Do:
*/


typedef struct OptionsRefCon
{
	DITLItemList 	*theDitlList;
	ControlTable 	*myControlTable;
	long			textPatterns;
	segaCharRect	textRect;
	TextBoxReference	myTextBoxMems;
	GraphicReference	graphRef;
	Boolean			cardDetected;
} OptionsRefCon;


// Items in account info DITL

enum {
	kRestrictionsText,			// 1
		
	kPlayingFieldText,			// 2
	kPlayingField,				// 3
	
	kHoursOfPlayText,			// 4
	kHoursOfPlay1,				// 5
	kHoursOfPlay2,				// 6
	
	kCreditsText,				// 7
	
	kTotalCreditsUsedText,		// 8
	kTotalCreditsUsed,			// 9
	
	kPrepaidCreditsText,		// 10
	
	kAccountCreditsText,		// 11
	kAccountCredits,			// 12
	
	kSmartCardCreditsText,		// 13
	kSmartCardCredits,			// 14
	
	kReturnButton				// 15
	};
	
	