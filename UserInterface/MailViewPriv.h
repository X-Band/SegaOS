/*
	File:		MailViewPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		<12>	 8/10/94	JBH		We now use a real CLUT (not the fixed CLUT 0) to do the text
									color animation.
		<11>	  8/9/94	CMY		Tossed the graphicsRef to the envelope BG graphic. KON's aux
									backdrop shit handles it now.
		<10>	  8/4/94	JBH		Modified mail sorting to sort by more fields if dates are equal.
		 <9>	  8/3/94	CMY		Added a line.
		 <8>	  8/3/94	CMY		Added a separate graphic for the background envelope
		 <7>	 7/30/94	JOE		add VBLProcRef to MailViewRefCon
		 <6>	 7/22/94	KON		Add color cycling.
		 <5>	 7/19/94	KON		Add textboxmems to support jizzlin borders.
		 <4>	  7/3/94	KON		Fix mail sorting.
		 <3>	  7/2/94	KON		Add mail sorting.
		 <2>	 6/12/94	SAH		Made pattern happy.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/

typedef struct
{
	short	entry;
	Mail	*theMail;
} MailViewSortType;


typedef struct 
{
	DITLItemList 		*mailItemDitlList;
	ControlTable 		*mailItemControlTable;
	DITLItemList 		*textButtDitlList;
	ControlTable 		*textButtControlTable;
	long				textPatterns;
	MailViewSortType 	*sortedMail;
	TextBoxReference	myTextBoxMems;
	GraphicReference	line2;
	MailItemDesc 		*disposeMailDesc;
	segaCharRect 		textRect;
	short				colorCycleClut;
	short				colorCycleFrame;
	short				colorCycleColors[16];
	VBLProcRef			vblRef;
} MailViewRefCon;

