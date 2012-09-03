/*
	File:		AddressBookPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<11>	 8/10/94	JBH		Removed unused fields. Added CLUT number to cycle, as we no
									longer cycle an entry in clut 0.
		<10>	  8/3/94	CMY		Added a line.
		 <9>	  8/3/94	CMY		Addded a separate graphic for the players in the background.
		 <8>	 7/29/94	JOE		added vblRef for clut-cycling VBL proc, removed colot cycling
									time variable (no longer needed)
		 <7>	 7/22/94	CMY		>>>
		 <6>	 7/22/94	KON		Add part 2 of clut cycling.
		 <5>	 7/22/94	CMY		Addded clut cycling for selected address book entry.
		 <4>	 7/19/94	KON		Add textBoxMems to support jizzlin borders.
		 <3>	  7/4/94	KON		Add sorting.
		 <2>	 6/12/94	SAH		Made pattern happy.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/

typedef struct
{
	short	entry;
	char	*name;
} AddressSortType;



typedef struct 
{
	DITLItemList 		*addressItemDitlList;
	ControlTable 		*addressItemControlTable;
	DITLItemList 		*textButtDitlList;
	ControlTable 		*textButtControlTable;
	long				addressBookPatterns;
	AddressSortType 	*sortedAddresses;
	TextBoxReference	myTextBoxMems;
	AddressItemDesc		*disposeAddressDesc;
	GraphicReference	line2;
	segaCharRect 		textRect;
	short				colorCycleClut;
	short				colorCycleFrame;
	short				colorCycleColors[16];
	VBLProcRef			vblRef;
} AddressBookRefCon;
