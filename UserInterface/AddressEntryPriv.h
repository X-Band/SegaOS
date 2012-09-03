/*
	File:		AddressEntryPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <9>	  8/3/94	CMY		Added a separate graphic for the players         in the
									background.
		 <8>	  8/3/94	CMY		Added a separate graphic for the players in the background.
		 <7>	 7/22/94	CMY		Added small player graphic.
		 <6>	 7/19/94	KON		Add textboxmems to support jizzlin borders.
		 <5>	 7/19/94	KON		Support two text blocks and icon drawing.
		 <4>	 6/16/94	KON		Add the address so the entry can find the address to delete.
		 <3>	 6/12/94	SAH		Made pattern happy.
		 <2>	 6/10/94	KON		Fixed fucked up first time check-in.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/


typedef struct 
{
	DITLItemList 		*theDitlList;
	ControlTable 		*myControlTable;
	long				textPatterns1;
	long				textPatterns2;
	PlayerInfo			*theAddress;
	IconReference		iconRef;
	TextBoxReference	myTextBoxMems;
	GraphicReference	playersRef;
	GraphicReference	line2;
} AddressEntryRefCon;
