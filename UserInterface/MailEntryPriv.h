/*
	File:		MailEntryPriv.h

	Contains:	xxx put contents here xxx

	Written by:	KON

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <8>	  8/3/94	CMY		Added a line
		 <7>	  8/3/94	CMY		Added a separate graphic for the background envelope.
		 <6>	 7/22/94	CMY		Added small envelope graphic
		 <5>	 7/19/94	KON		Add textboxmems field to support jizzlin borders.
		 <4>	 7/18/94	KON		Add textPattern2 field.
		 <3>	 6/12/94	SAH		Made pattern happy.
		 <2>	 6/11/94	KON		Add theMail ref so the entry screen can find and delete this
									piece of mail.
		 <1>	  6/9/94	KON		first checked in

	To Do:
*/


typedef struct 
{
	DITLItemList 	*theDitlList;
	ControlTable 	*myControlTable;
	Mail			*theMail;
	long			textPattern1;
	long			textPattern2;
	IconReference	iconRef;
	TextBoxReference	myTextBoxMems;
	GraphicReference	envelopeRef;
	GraphicReference	line2;
} MailEntryRefCon;
